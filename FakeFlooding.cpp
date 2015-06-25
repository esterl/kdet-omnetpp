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

#include "FakeFlooding.h"
#include "Ieee802Ctrl_m.h"
#include "ARP.h"
#include "IPvXAddressResolver.h"
#include "IPSocket.h"
#include "GraphServer.h"
#include "IPv4ControlInfo.h"
#include "kdet_defs.h"

Define_Module(FakeFlooding);

void FakeFlooding::initialize() {
    IP = IPv4Address::UNSPECIFIED_ADDRESS;
    cModule* graphSrvModule = getModuleByPath("graphServer");
    graphServer = check_and_cast<GraphServer*>(graphSrvModule);
    timeout = par("timeout");
    TTL = int(par("k")) + 2;
}

void FakeFlooding::finish() {
    for (auto it = messages.begin(); it != messages.end(); it++) {
        cancelAndDelete(*it);
    }
    for (auto it = timeouts.begin(); it != timeouts.end(); it++) {
        cancelAndDelete(*it);
    }
}

void FakeFlooding::handleMessage(cMessage *msg) {
    if (msg->arrivedOn("in")) {
        // Deliver message to app
        send(msg->dup(), "out");
        // Flood to the rest
        newReport(check_and_cast<Report*>(msg));
    } else if (!msg->isSelfMessage()) {
        // Is for me?
        IPv4Datagram* ipPkt = check_and_cast<IPv4Datagram*>(msg);
        if (ipPkt->getDestAddress() == getIP()) {
            cPacket* inMsg = decapsulate(ipPkt);
            if (inMsg->getKind() == KDET_REPORT_MSG) {
                processReport(check_and_cast<Report*>(inMsg));
            } else { // inMsg->getKind() == KDET_ACK_MSG
                processAck(check_and_cast<ReportAck*>(inMsg));
            }
        } else {
            delete msg;
        }
    } else {
        // Timeout Expired:
        timeoutExpired(check_and_cast<ReportIdMsg*>(msg));
    }
}

void FakeFlooding::newReport(Report *report) {
    // Set IP
    report->setReporter(getIP());
    // Set id
    report->setId(id++);
    // Set TTL
    report->setTTL(TTL);
    // Deliver locally
    send(report->dup(), "out");
    // Reliably Flood
    reliablyFlood(report);
}

void FakeFlooding::processReport(Report *report) {
    // Send ACK
    sendAck(report);
    // Deliver locally if is new
    if (isNew(report)) {
        send(report->dup(), "out");
    }
    // Update TTL
    int ttlReport = report->getTTL();
    report->setTTL(--ttlReport);
    if (ttlReport > 0) {
        reliablyFlood(report);
    } else {
        delete report;
    }
}

void FakeFlooding::reliablyFlood(Report* report) {
    IPv4Address addr = report->getReporter();
    int index;
    if (IPToIndex.count(addr.getInt()) == 0) {
        index = messages.size();
        messages.push_back(encapsulate(report));
        sendTo.push_back(graphServer->getNeighbors(getIP()));
        scheduleTimeout(index, addr);
    } else {
        index = IPToIndex[addr.getInt()];
        Report* localReport = check_and_cast<Report*>(
                messages[index]->getEncapsulatedPacket());
        // Is it an older or newer report?
        if (report->getId() == localReport->getId()) {
            // Update TTL
            if (report->getTTL() > localReport->getTTL())
                localReport->setTTL(report->getTTL());
            delete report;
        } else if (report->getId() < localReport->getId()) {
            delete report;
        } else {
            delete messages[index];
            messages[index] = encapsulate(report);
            sendTo[index] = graphServer->getNeighbors(getIP());
            scheduleTimeout(index, addr);
        }
    }
}

bool FakeFlooding::isNew(Report* report) {
    IPv4Address addr = report->getReporter();
    int index;
    if (IPToIndex.count(addr.getInt()) == 0) {
        return true;
    } else {
        index = IPToIndex[addr.getInt()];
        Report* localReport = check_and_cast<Report*>(
                messages[index]->getEncapsulatedPacket());
        // Is it an older or newer report?
        if (report->getId() > localReport->getId())
            return true;
    }
    return false;
}

IPv4Datagram* FakeFlooding::encapsulate(Report* report) {
    IPv4Datagram* msg = new IPv4Datagram("Sketches report");
    msg->setSrcAddress(getIP());
    msg->setTransportProtocol(KDET_PROTOCOL_NUMBER);
    // TODO do I want something different?
    msg->setTimeToLive(16);
    msg->encapsulate(report);
    return msg;
}

cPacket* FakeFlooding::decapsulate(IPv4Datagram* ipPkt) {
    cPacket* inMsg = ipPkt->decapsulate();
    IPv4ControlInfo* ctrlInfo = new IPv4ControlInfo();
    ctrlInfo->setDestAddr(ipPkt->getDestAddress());
    ctrlInfo->setSrcAddr((ipPkt->getSrcAddress()));
    inMsg->setControlInfo(ctrlInfo);
    delete ipPkt;
    return inMsg;
}

void FakeFlooding::scheduleTimeout(int index, IPv4Address addr) {
    ReportIdMsg* timeoutMsg = new ReportIdMsg("Reliable flooding timeout");
    timeoutMsg->setAddress(addr);
    if (index >= timeouts.size()) {
        timeouts.push_back(timeoutMsg);
    } else {
        cancelAndDelete(timeouts[index]);
        timeouts[index] = timeoutMsg;
    }
    scheduleAt(simTime(), timeoutMsg);
}

void FakeFlooding::timeoutExpired(ReportIdMsg* idMsg) {
    int index = IPToIndex[idMsg->getAddress().getInt()];
    for (auto it = sendTo[index].begin(); it != sendTo[index].end(); it++) {
        IPv4Datagram *msg = messages[index]->dup();
        msg->setDestAddress(*it);
        send(msg, "othersOut");
    }
    if (sendTo[index].size() > 0)
        scheduleAt(simTime() + timeout, idMsg);
}

void FakeFlooding::sendAck(Report* report) {
    ReportAck* ack = new ReportAck("Flooding ACK");
    ack->setId(report->getId());
    ack->setReporter(report->getReporter());
    IPv4Datagram* ipPkt = new IPv4Datagram("Flooding ACK");
    IPv4ControlInfo* ctrlInfo = check_and_cast<IPv4ControlInfo*>(
            report->getControlInfo());
    ipPkt->setDestAddress(ctrlInfo->getSrcAddr());
    ipPkt->setSrcAddress(getIP());
    ipPkt->setTransportProtocol(KDET_PROTOCOL_NUMBER);
    ipPkt->setTimeToLive(16);
    ipPkt->encapsulate(ack);
    send(ipPkt, "othersOut");
}

void FakeFlooding::processAck(ReportAck* ack) {
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
IPv4Address FakeFlooding::getIP() {
    if (IP.isUnspecified()) {
        IP = IPvXAddressResolver().addressOf(
                getParentModule()->getParentModule(),
                IPvXAddressResolver::ADDR_PREFER_IPv4).get4();
    }
    return IP;
}
