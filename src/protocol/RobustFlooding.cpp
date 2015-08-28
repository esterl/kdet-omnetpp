/***************************************************************************
 *   Copyright (C) 2015 by Ester Lopez                                     *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/


#include "RobustFlooding.h"
#include "IPvXAddressResolver.h"
#include "IPv4ControlInfo.h"
#include "IPSocket.h"

Define_Module(RobustFlooding);

std::string printSet(std::set<IPv4Address> ipSet) {
    std::stringstream ss;
    for (auto it = ipSet.begin(); it != ipSet.end(); it++) {
        ss << *it << ",";
    }
    return ss.str();
}

void RobustFlooding::initialize(int stage) {
    if (stage == 3) {
        IP = IPvXAddressResolver().addressOf(
                getParentModule()->getParentModule(),
                IPvXAddressResolver::ADDR_PREFER_IPv4).get4();
        cModule* graphSrvModule = getModuleByPath("graphServer");
        graphServer = check_and_cast<GraphServer*>(graphSrvModule);
        TTL = int(par("k")) + 1;
        timeout = 0.5;
        overhead.setName("Overhead");

        // Register protocol:
        IPSocket socket(gate("othersOut"));
        socket.registerProtocol(KDET_PROTOCOL_NUMBER);

        // Monitoring
        WATCH_PTRVECTOR(messages);
        WATCH_VECTOR(sendToStr);
    }
}

void RobustFlooding::finish() {
    for (auto it = messages.begin(); it != messages.end(); it++) {
        cancelAndDelete(*it);
    }
    for (auto it = timeouts.begin(); it != timeouts.end(); it++) {
        if (*it != NULL)
            cancelAndDelete(*it);
    }

}

void RobustFlooding::handleMessage(cMessage *msg) {
    if (msg->arrivedOn("in")) {
        // Flood to the rest
        newReport(check_and_cast<Report*>(msg));
    } else if (!msg->isSelfMessage()) {
        if (msg->getKind() == KDET_REPORT_MSG) {
            if (dynamic_cast<Report*>(msg))
                processReport(check_and_cast<Report*>(msg));
            else
                delete msg;
        } else { // msg->getKind() == KDET_ACK_MSG
            if (dynamic_cast<ReportAck*>(msg))
                processAck(check_and_cast<ReportAck*>(msg));
            else
                delete msg;
        }
    } else {
        // Timeout Expired:
        timeoutExpired(check_and_cast<ReportIdMsg*>(msg));
    }

}

void RobustFlooding::newReport(Report *report) {
    // Set Report's parameters:
    setParameters(report);
    // Deliver locally
    send(report->dup(), "out");
    // Reliably Flood
    reliablyFlood(report);
}

void RobustFlooding::processReport(Report *report) {
    // Send ACK
    sendAck(report);
    // Deliver locally if is new
    bool newReport = isNew(report);
    if (newReport) {
        send(report->dup(), "out");
    }
    // Update TTL
    int ttlReport = report->getTTL();
    report->setTTL(--ttlReport);
    reliablyFlood(report);
}

void RobustFlooding::processAck(ReportAck* ack) {
    IPv4ControlInfo* ctrlInfo = check_and_cast<IPv4ControlInfo*>(
            ack->getControlInfo());
    IPv4Address addr = ctrlInfo->getSrcAddr();
    //
    int index = ack->getId();
    if (index < messages.size()) {
        // Same version?
        Report* report = messages[index];
        if (report->getVersion() == ack->getVersion()) {
            // Delete addr from sentTo
            sendTo[index].erase(addr);
            sendToStr[index] = printSet(sendTo[index]);
        }
    }
    delete ack;
}

void RobustFlooding::reliablyFlood(Report *report) {
    int index;
    if (reportToIndex.count(report) == 0) {
        index = messages.size();
        report->setIndex(index);
        reportToIndex[report] = index;
        //setControlInfo(report);
        messages.push_back(report);
        if (report->getTTL() > 0) {
            sendTo.push_back(report->sendTo(IP, graphServer->getNeighbors(IP)));
            scheduleTimeout(index);
        } else {
            sendTo.push_back(std::set<IPv4Address>());
            // Null timeout
            timeouts.push_back(NULL);
        }
        sendToStr.push_back(printSet(sendTo[index]));
    } else {
        index = reportToIndex[report];
        Report* localReport = check_and_cast<Report*>(messages[index]);
        // Is it an older or newer report?
        if (report->getVersion() == localReport->getVersion()) {
            // Update TTL
            if (report->getTTL() > localReport->getTTL()) {
                // Resend with the new TTL
                sendTo[index] = report->sendTo(IP,
                        graphServer->getNeighbors(IP));
                sendToStr[index] = printSet(sendTo[index]);
                localReport->setTTL(report->getTTL());
                scheduleTimeout(index);
            }
            delete report;
        } else if (report->getVersion() < localReport->getVersion()) {
            delete report;
        } else {
            reportToIndex.erase(messages[index]);
            reportToIndex[report] = index;
            if (localReportVersions.count(messages[index]) != 0) {
                long tmp = localReportVersions[messages[index]];
                localReportVersions.erase(messages[index]);
                localReportVersions[report] = tmp;
            }
            delete messages[index];
            //setControlInfo(report);
            messages[index] = report;
            report->setIndex(index);
            if (report->getTTL() > 0) {
                sendTo[index] = report->sendTo(IP,
                        graphServer->getNeighbors(IP));
                scheduleTimeout(index);
            } else {
                sendTo[index] = std::set<IPv4Address>();
            }
            sendToStr[index] = printSet(sendTo[index]);
        }
    }
}

// TODO everywhere: ojo con el getId que ahora es otra cosa
bool RobustFlooding::isNew(Report* report) {
    int index;
    if (reportToIndex.count(report) == 0) {
        return true;
    } else {
        index = reportToIndex[report];
        Report* localReport = messages[index];
        // Is it an older or newer report?
        if (report->getVersion() > localReport->getVersion()) {
            return true;
        }
    }
    return false;
}

void RobustFlooding::setControlInfo(Report* report, IPv4Address dst) {
    // Report should not have ControlInfo
    IPv4ControlInfo *controlInfo = new IPv4ControlInfo();
    controlInfo->setSrcAddr(IP);
    controlInfo->setDestAddr(dst);
    controlInfo->setProtocol(KDET_PROTOCOL_NUMBER);
    controlInfo->setTimeToLive(16);
    report->setControlInfo(controlInfo);
}

void RobustFlooding::sendAck(Report* report) {
    ReportAck* ack = new ReportAck("Flooding ACK");
    ack->setId(report->getIndex());
    ack->setVersion(report->getVersion());

    // Set controlInfo
    IPv4ControlInfo *ctrlReport, *ctrlAck;
    // TODO removeCtrlInfo?
    ctrlReport = check_and_cast<IPv4ControlInfo*>(report->getControlInfo());
    ctrlAck = new IPv4ControlInfo();
    ctrlAck->setSrcAddr(IP);
    ctrlAck->setDestAddr(ctrlReport->getSrcAddr());
    ctrlAck->setProtocol(KDET_PROTOCOL_NUMBER);
    ctrlAck->setTimeToLive(16);
    ack->setControlInfo(ctrlAck);

    // Send Ack
    sendDelayed(ack, jitter(), "othersOut");
}

void RobustFlooding::scheduleTimeout(int index) {
    ReportIdMsg* timeoutMsg = new ReportIdMsg("Reliable flooding timeout");
    timeoutMsg->setId(index);
    if (index >= timeouts.size()) {
        timeouts.push_back(timeoutMsg);
    } else {
        if (timeouts[index] != NULL)
            cancelAndDelete(timeouts[index]);
        timeouts[index] = timeoutMsg;
    }
    scheduleAt(simTime(), timeoutMsg);
}

void RobustFlooding::timeoutExpired(ReportIdMsg* idMsg) {
    int index = idMsg->getId();

    if (sendTo[index].size() > 0) {
        if (!par("multicast").boolValue() or sendTo[index].size() == 1) {
            sendUnicast(idMsg);
        } else {
            sendMulticast(idMsg);
        }
        scheduleAt(simTime() + timeout, idMsg);
    }
}

void RobustFlooding::sendMulticast(ReportIdMsg* idMsg) {
    int index = idMsg->getId();
    int tries = idMsg->getTries();
    if (tries < 0) {
        sendUnicast(idMsg);
    } else {
        idMsg->setTries(--tries);
        Report* report = messages[index]->dup();
        setControlInfo(report, IPv4Address::ALL_ROUTERS_MCAST);
        sendDelayed(report, jitter(), "othersOut");
        if (report->getBytes() != 0)
            overhead.record(report->getBytes());
    }
}
void RobustFlooding::sendUnicast(ReportIdMsg* idMsg) {
    int index = idMsg->getId();
    for (auto it = sendTo[index].begin(); it != sendTo[index].end(); it++) {
        Report* report = messages[index]->dup();
        setControlInfo(report, *it);
        sendDelayed(report, jitter(), "othersOut");
        if (report->getBytes() != 0)
            overhead.record(report->getBytes());
    }
}

simtime_t RobustFlooding::jitter() {
    return uniform(0, timeout / 4);
}

void RobustFlooding::setParameters(Report* report) {
    report->setReporter(IP);
    report->setTTL(TTL);
    report->setNonce(intuniform(0, UINT_MAX));

    std::stringstream name;
    report->setReporter(IP);
    name << report->getName();

    long version = 0;
    if (localReportVersions.count(report) != 0) {
        version = localReportVersions[report];
    }
    localReportVersions[report] = version + 1;
    name << " : " << version;
    report->setVersion(version);
    report->setName(name.str().c_str());
}
