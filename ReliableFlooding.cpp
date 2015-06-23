//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public License
// along with this program.  If not, see http://www.gnu.org/licenses/.
// 

#include "ReliableFlooding.h"
#include "Ieee802Ctrl_m.h"
#include "ARP.h"
#include "IPvXAddressResolver.h"
#include "IPSocket.h"
#include "GraphServer.h"
#include "IPv4ControlInfo.h"

#define KDET_PROTOCOL_NUMBER 158
#define KDET_REPORT_MSG 1
#define KDET_ACK_MSG 2

Define_Module(ReliableFlooding);

void ReliableFlooding::initialize() {
    IP = IPv4Address::UNSPECIFIED_ADDRESS;
    cModule* graphSrvModule = getModuleByPath("graphServer");
    graphServer = check_and_cast<GraphServer*>(graphSrvModule);
    timeout = par("timeout");
    TTL = int(par("k")) + 2;
    IPSocket socket(gate("IPout"));
    socket.registerProtocol(KDET_PROTOCOL_NUMBER);
}

void ReliableFlooding::finish() {
    for (auto it = messages.begin(); it != messages.end(); it++) {
        cancelAndDelete(*it);
    }
    for (auto it = timeouts.begin(); it != timeouts.end(); it++) {
        cancelAndDelete(*it);
    }
}

void ReliableFlooding::handleMessage(cMessage *msg) {
    if (msg->arrivedOn("in")) {
        reliablyFlood(check_and_cast<Report*>(msg));
    } else if (!msg->isSelfMessage()) {
        if (msg->getKind() == KDET_REPORT_MSG) {
            // Deliver message
            send(msg->dup(), "out");
            // Acknowledge the reception
            sendAck(check_and_cast<Report*>(msg));
            // Reliably flood it to neighbors
            reliablyFlood(check_and_cast<Report*>(msg));
        } else if (msg->getKind() == KDET_ACK_MSG) {
            processAck(check_and_cast<ReportAck*>(msg));
        } else {
            delete msg;
        }
    } else {
        // Timeout Expired:
        timeoutExpired(check_and_cast<ReportIdMsg*>(msg));
    }
}

void ReliableFlooding::reliablyFlood(Report* report) {
    if (report->getReporter().isUnspecified()
            or report->getReporter() == getIP()) {
        report->setReporter(getIP());
        report->setId(id++);
        report->setTTL(TTL);
        newReport(report);
    } else {
        int ttlReport = report->getTTL();
        ttlReport--;
        report->setTTL(ttlReport);
        long reportId = report->getId();
        if (ttlReport > 0) {
            newReport(report);
        } else {
            delete report;
        }
    }
}

void ReliableFlooding::newReport(Report *report) {
    // Create IPv4 message:
    IPv4Datagram* msg = new IPv4Datagram();
    msg->setTransportProtocol(KDET_PROTOCOL_NUMBER);
    // TODO do I want something different?
    msg->setTimeToLive(16);
    msg->encapsulate(report);
    // Do we have another report for that Reporter?
    IPv4Address addr = report->getReporter();
    int index;
    if (IPToIndex.count(addr.getInt()) == 0) {
        index = messages.size();
        IPToIndex[addr.getInt()] = index;
        messages.push_back(msg);
        sendTo.push_back(graphServer->getNeighbors(getIP()));
        // Schedule timeout:
        ReportIdMsg* timeoutMsg = new ReportIdMsg();
        timeoutMsg->setAddress(report->getReporter());
        timeoutMsg->setId(report->getId());
        timeouts.push_back(timeoutMsg);
        scheduleAt(simTime(), timeoutMsg);
    } else {
        // Older or newer report?
        int index = IPToIndex[addr.getInt()];
        if (report->getId()
                < check_and_cast<Report*>(
                        messages[index]->getEncapsulatedPacket())->getId()) {
            //Delete report
            delete msg;
        } else {
            delete messages[index];
            messages[index] = msg;
        }
    }

}

void ReliableFlooding::timeoutExpired(ReportIdMsg* idMsg) {
    int index = IPToIndex[idMsg->getAddress().getInt()];
    for (auto it = sendTo[index].begin(); it != sendTo[index].end(); it++) {
        IPv4Datagram *msg = messages[index]->dup();
        msg->setDestAddress(*it);
        send(msg, "IPout");
    }
    if (sendTo[index].size() > 0)
        scheduleAt(simTime() + timeout, idMsg);
}

void ReliableFlooding::sendAck(Report* report) {
    ReportAck* ack = new ReportAck();
    ack->setId(report->getId());
    ack->setReporter(report->getReporter());
    IPv4Datagram* ipPkt = new IPv4Datagram();
    IPv4ControlInfo* ctrlInfo = check_and_cast<IPv4ControlInfo*>(
            report->getControlInfo());
    ipPkt->setDestAddress(ctrlInfo->getSrcAddr());
    ipPkt->setTransportProtocol(KDET_PROTOCOL_NUMBER);
    ipPkt->setTimeToLive(16);
    ipPkt->encapsulate(ack);
    send(ipPkt, "IPout");
}

void ReliableFlooding::processAck(ReportAck* ack) {
    IPv4ControlInfo* ctrlInfo = check_and_cast<IPv4ControlInfo*>(
            ack->getControlInfo());
    IPv4Address addr = ctrlInfo->getSrcAddr();
    // Does the report exist?
    if (IPToIndex.count(ack->getReporter().getInt()) != 0) {
        // Same Id?
        int index = IPToIndex[ack->getReporter().getInt()];
        Report* report = check_and_cast<Report*>(
                messages[index]->getEncapsulatedPacket());
        if (report->getId() == ack->getId()) {
            // Delete addr from sentTo
            sendTo[index].erase(addr);
        }
    }
    delete ack;
}
IPv4Address ReliableFlooding::ReliableFlooding::getIP() {
    if (IP.isUnspecified()) {
        IP = IPvXAddressResolver().addressOf(
                getParentModule()->getParentModule(),
                IPvXAddressResolver::ADDR_PREFER_IPv4).get4();
    }
    return IP;
}
