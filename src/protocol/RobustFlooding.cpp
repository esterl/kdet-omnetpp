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
#include "L3Address.h"
#include "L3AddressResolver.h"
#include "IPv4ControlInfo.h"
#include "IPSocket.h"

namespace kdet{
Define_Module(RobustFlooding);

std::string printSet(std::set<inet::IPv4Address> ipSet) {
    std::stringstream ss;
    for (auto it = ipSet.begin(); it != ipSet.end(); it++) {
        ss << *it << ",";
    }
    return ss.str();
}

void RobustFlooding::initialize(int stage) {
    if (stage == inet::INITSTAGE_LAST) {
        sendTimeout = NULL;
        IP = inet::L3AddressResolver().addressOf(
                getParentModule()->getParentModule(),
                inet::L3AddressResolver::ADDR_IPv4).toIPv4();
        cModule* graphSrvModule = getModuleByPath("graphServer");
        graphServer = check_and_cast<GraphServer*>(graphSrvModule);
        TTL = int(par("k")) + 1;
        // TODO parameter?
        timeout = 0.5;
        overhead.setName("Overhead");

        // Register protocol:
        inet::IPSocket socket(gate("othersOut"));
        socket.registerProtocol(KDET_PROTOCOL_NUMBER);

        // Monitoring
        WATCH_PTRVECTOR(messages);
        WATCH_VECTOR(sendToStr);
        WATCH_PTR(sendTimeout);
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
    if (sendTimeout != NULL)
        cancelAndDelete(sendTimeout);
}

void RobustFlooding::handleMessage(cMessage *msg) {
    if (msg->arrivedOn("in")) {
        // Flood to the rest
        newReport(check_and_cast<Report*>(msg));
    } else if (!msg->isSelfMessage()) {
        if (msg->getKind() == KDET_REPORT_MSG_AGGR) {
            if (dynamic_cast<ReportAggregation*>(msg))
                processReport(check_and_cast<ReportAggregation*>(msg));
            delete msg;
        } else if (msg->getKind() == KDET_ACK_MSG) {
            if (dynamic_cast<ReportAck*>(msg))
                processAck(check_and_cast<ReportAck*>(msg));
            else
                delete msg;
        } else {
            std::cout << simTime() << "handle unknown msg" << endl;
            delete msg;
        }
    } else if (msg->getKind() == 0) {
        // Timeout Expired:
        timeoutExpired(check_and_cast<ReportIdMsg*>(msg));
    } else {
        flushQueues();
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

void RobustFlooding::processReport(ReportAggregation *reportAggr) {
    // Send ACK
    sendAck(reportAggr);
    // Process each individual report:
    for (unsigned i = 0; i < reportAggr->getReportsArraySize(); i++) {
        Report* report = reportAggr->getReports(i);
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
}

void RobustFlooding::processAck(ReportAck* ack) {
    inet::IPv4ControlInfo* ctrlInfo = check_and_cast<inet::IPv4ControlInfo*>(
            ack->getControlInfo());
    inet::IPv4Address addr = ctrlInfo->getSrcAddr();
    //
    for (unsigned i = 0; i < ack->getIdsArraySize(); i++) {
        int index = ack->getIds(i);
        long version = ack->getVersions(i);
        if (index < messages.size()) {
            // Same version?
            Report* report = messages[index];
            if (report->getVersion() == version) {
                // Delete addr from sentTo
                sendTo[index].erase(addr);
                sendToStr[index] = printSet(sendTo[index]);
            }
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
            sendTo.push_back(std::set<inet::IPv4Address>());
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
                sendTo[index] = std::set<inet::IPv4Address>();
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

void RobustFlooding::setControlInfo(ReportAggregation* report,
        inet::IPv4Address dst) {
    // Report should not have ControlInfo
    inet::IPv4ControlInfo *controlInfo = new inet::IPv4ControlInfo();
    controlInfo->setSrcAddr(IP);
    controlInfo->setDestAddr(dst);
    controlInfo->setProtocol(KDET_PROTOCOL_NUMBER);
    controlInfo->setTimeToLive(16);
    report->setControlInfo(controlInfo);
}

void RobustFlooding::sendAck(ReportAggregation* reportAggr) {
    ReportAck* ack = new ReportAck("Flooding ACK");
    unsigned numReports = reportAggr->getReportsArraySize();
    ack->setIdsArraySize(numReports);
    ack->setVersionsArraySize(numReports);
    for (unsigned i = 0; i < numReports; i++) {
        Report* report = reportAggr->getReports(i);
        ack->setIds(i, report->getIndex());
        ack->setVersions(i, report->getVersion());
        delete report;
    }

    // Set controlInfo
    inet::IPv4ControlInfo *ctrlReport, *ctrlAck;
    // TODO removeCtrlInfo?
    ctrlReport = check_and_cast<inet::IPv4ControlInfo*>(
            reportAggr->getControlInfo());
    ctrlAck = new inet::IPv4ControlInfo();
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
        //Report* report = messages[index]->dup();
        //setControlInfo(report, IPv4Address::ALL_ROUTERS_MCAST);
        //sendDelayed(report, jitter(), "othersOut");
        queueReport(index, inet::IPv4Address::ALL_ROUTERS_MCAST.getInt());
        //if (report->getBytes() != 0)
        //    overhead.record(report->getBytes());
    }
}
void RobustFlooding::sendUnicast(ReportIdMsg* idMsg) {
    int index = idMsg->getId();
    for (auto it = sendTo[index].begin(); it != sendTo[index].end(); it++) {
        //Report* report = messages[index]->dup();
        //setControlInfo(report, *it);
        //sendDelayed(report, jitter(), "othersOut");
        queueReport(index, it->getInt());
        //if (report->getBytes() != 0)
        //    overhead.record(report->getBytes());
    }
}

void RobustFlooding::queueReport(int reportIndex, int addr) {
    if (reportQueues.count(addr) == 0) {
        reportQueues[addr] = std::vector<int>();
    }
    reportQueues[addr].push_back(reportIndex);
    if (sendTimeout == NULL) {
        sendTimeout = new cMessage("send Timeout", 1);
        scheduleAt(simTime() + timeout / 4, sendTimeout);
    }
}

void RobustFlooding::flushQueues() {
    for (auto queue = reportQueues.begin(); queue != reportQueues.end();
            queue++) {
        auto index = queue->second.begin();
        // Remove Reports that have been already ack'd
        while (index != queue->second.end()) {
            if (sendTo[*index].size() == 0) {
                index = queue->second.erase(index);
            } else {
                index++;
            }
        }
        // Create Aggregated report
        if (queue->second.size() > 0) {
            ReportAggregation* reportAggr = new ReportAggregation();
            reportAggr->setReportsArraySize(queue->second.size());
            for (unsigned i = 0; i < queue->second.size(); i++) {
                Report* report = messages[queue->second[i]]->dup();
                if (report->getBytes() != 0)
                    overhead.record(report->getBytes());
                reportAggr->setReports(i, report);
            }
            setControlInfo(reportAggr, inet::IPv4Address(queue->first));
            // TODO check if delayed or directly
            sendDelayed(reportAggr, jitter(), "othersOut");
            // Clear queue
            queue->second.clear();
        }

    }
    delete sendTimeout;
    sendTimeout = NULL;
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
}
