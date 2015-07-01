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

#include "SketchMonitor.h"
#include "IPvXAddressResolver.h"
#include "Ieee802Ctrl_m.h"
#include "TCPSegment.h"
#include "ByteArray.h"
#include "ARP.h"
#include "kdet_defs.h"

Define_Module(SketchMonitor);

LinkSummary* SketchMonitor::getLinkSummary(IPv4Address address) {
    if (summaries.count(address.getInt()) == 0) {
        summaries[address.getInt()] = new SketchSummary();
    }
    return summaries[address.getInt()];
}

INetfilter::IHook::Result SketchMonitor::datagramPreRoutingHook(
        IPv4Datagram* datagram, const InterfaceEntry* inIE,
        const InterfaceEntry*& outIE, IPv4Address& nextHopAddr) {
    // Update the summary of the related link:
    if (datagram->getTransportProtocol() == DATA_PROTOCOL_NUMBER) {
        IPv4Address addr = getIPAddress(datagram);
        LinkSummary* summary = findSummary(addr);
        summary->updateSummaryPreRouting(datagram);
    }
    return IHook::ACCEPT;
}

// TODO added just for now
#include <cstdlib>
INetfilter::IHook::Result SketchMonitor::datagramForwardHook(
        IPv4Datagram* datagram, const InterfaceEntry* inIE,
        const InterfaceEntry*& outIE, IPv4Address& nextHopAddr) {
    return IHook::ACCEPT;
}

INetfilter::IHook::Result SketchMonitor::datagramPostRoutingHook(
        IPv4Datagram* datagram, const InterfaceEntry* inIE,
        const InterfaceEntry*& outIE, IPv4Address& nextHopAddr) {
    // Update the summary of the related link if is a data packet:
    if (datagram->getTransportProtocol() == DATA_PROTOCOL_NUMBER) {
        LinkSummary* summary = findSummary(nextHopAddr);
        summary->updateSummaryPostRouting(datagram);
    }
    return IHook::ACCEPT;
}

INetfilter::IHook::Result SketchMonitor::datagramLocalInHook(
        IPv4Datagram* datagram, const InterfaceEntry* inIE) {
    return IHook::ACCEPT;
}

// TODO this should be done by another class
INetfilter::IHook::Result SketchMonitor::datagramLocalOutHook(
        IPv4Datagram* datagram, const InterfaceEntry*& outIE,
        IPv4Address& nextHopAddr) {
    // Set ip address
    if (datagram->getTransportProtocol() == DATA_PROTOCOL_NUMBER) {
        datagram->setSrcAddress(getIP());
    }
    return IHook::ACCEPT;
}

void SketchMonitor::initialize() {
    // Set the BaseSketch
    SketchSummary::setBaseSketch(this);
    // Register Hook
    ipLayer = check_and_cast<IPv4*>(findModuleWhereverInNode("ip", this));
    // TODO check the priority that should have
    ipLayer->registerHook(1, this);
    // Initialize the rest of parameters
    IP = IPvXAddressResolver().addressOf(getParentModule()->getParentModule(),
            IPvXAddressResolver::ADDR_PREFER_IPv4).get4();
    faulty = par("faulty");
}

void SketchMonitor::handleMessage(cMessage *msg) {
    // Send report
    Report* report = new Report();
    report->setReporter(getIP());
    report->setSummaries(summaries);
    report->setBogus(faulty);
    send(report, "reports");
    // Clear sketches
    resetSummaries();
    delete msg;
}

void SketchMonitor::finish() {
    // TODO check what else to delete
    // TODO delete summaries
    for (auto it = summaries.begin(); it != summaries.end(); it++) {
        delete it->second;
    }
    summaries.clear();
}
void SketchMonitor::resetSummaries() {
    for (auto it = summaries.begin(); it != summaries.end(); it++) {
        it->second->clear();
    }
}

IPv4Address SketchMonitor::getIPAddress(IPv4Datagram* datagram) {
    Ieee802Ctrl *ctrl = check_and_cast<Ieee802Ctrl*>(
            datagram->getControlInfo());
    ARP* arp = ArpAccess().get();
    if (datagram->getControlInfo() == NULL)
        datagram->setControlInfo(ctrl);
    return arp->getIPv4AddressFor(ctrl->getSrc());
}

SketchSummary* SketchMonitor::findSummary(IPv4Address addr) {
    if (summaries.count(addr.getInt()) == 0) {
        summaries[addr.getInt()] = new SketchSummary(getIP(), addr);
    }
    return (SketchSummary*) summaries[addr.getInt()];
}

IPv4Address SketchMonitor::getIP() {
    if (IP.isUnspecified()) {
        IP = IPvXAddressResolver().addressOf(
                getParentModule()->getParentModule(),
                IPvXAddressResolver::ADDR_PREFER_IPv4).get4();
    }
    return IP;
}
