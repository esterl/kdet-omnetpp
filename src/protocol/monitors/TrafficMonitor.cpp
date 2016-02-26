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

#include "TrafficMonitor.h"

#include "kdet_defs.h"
#include "L3AddressResolver.h"
#include "ModuleAccess.h"
#include "IPv4.h"
#include <Ieee802Ctrl.h>
#include "ARP.h"

namespace kdet{
inet::INetfilter::IHook::Result TrafficMonitor::datagramPreRoutingHook(
        inet::INetworkDatagram* datagram, const inet::InterfaceEntry* inIE,
        const inet::InterfaceEntry*& outIE, inet::L3Address& nextHopAddr) {
    // Update related summaries:
    if (datagram->getTransportProtocol() == DATA_PROTOCOL_NUMBER) {
        //inet::IPv4Address addr = getIPAddress(datagram);
        // TODO make sure this works
        //inet::IPv4Address addr = datagram->getSourceAddress().toIPv4();
        inet::IPv4Address addr = getIPAddress(datagram);
        std::vector<Summary*> summaries = findSummaries(addr);
        for (auto it = summaries.begin(); it != summaries.end(); it++) {
            (*it)->updateSummaryPreRouting(datagram);
        }
    }
    return IHook::ACCEPT;
}

inet::INetfilter::IHook::Result TrafficMonitor::datagramForwardHook(
        inet::INetworkDatagram* datagram, const inet::InterfaceEntry* inIE,
        const inet::InterfaceEntry*& outIE, inet::L3Address& nextHopAddr) {
    return IHook::ACCEPT;
}

inet::INetfilter::IHook::Result TrafficMonitor::datagramPostRoutingHook(
        inet::INetworkDatagram* datagram, const inet::InterfaceEntry* inIE,
        const inet::InterfaceEntry*& outIE, inet::L3Address& nextHopAddr) {
    // Update related summaries:
    if (datagram->getTransportProtocol() == DATA_PROTOCOL_NUMBER) {
        inet::IPv4Address addr = nextHopAddr.toIPv4();
        if (addr.isUnspecified()) addr = datagram->getDestinationAddress().toIPv4();
        std::vector<Summary*> summaries = findSummaries(addr);
        for (auto it = summaries.begin(); it != summaries.end(); it++) {
            (*it)->updateSummaryPostRouting(datagram);
        }
    }
    return IHook::ACCEPT;
}

inet::INetfilter::IHook::Result TrafficMonitor::datagramLocalInHook(
        inet::INetworkDatagram* datagram, const inet::InterfaceEntry* inIE) {
    return IHook::ACCEPT;
}

// TODO this should be done by another class
inet::INetfilter::IHook::Result TrafficMonitor::datagramLocalOutHook(
        inet::INetworkDatagram* datagram, const inet::InterfaceEntry*& outIE,
        inet::L3Address& nextHopAddr) {
    // Set ip address
    if (datagram->getTransportProtocol() == DATA_PROTOCOL_NUMBER) {
        datagram->setSourceAddress(inet::L3Address(IP));
    }
    return IHook::ACCEPT;
}

void TrafficMonitor::setCores(
        std::vector<std::set<inet::IPv4Address>> newCores) {
    cores = newCores;
}

void TrafficMonitor::setShareSummaries(std::vector<bool> newShareSummaries) {
    shareSummaries = newShareSummaries;
}

void TrafficMonitor::initialize(int stage) {
    if (stage == inet::INITSTAGE_LAST) {
        // Register Hook
        inet::IPv4* ipLayer = check_and_cast<inet::IPv4*>(
                getModuleByPath("^.^.networkLayer.ip"));
        // TODO check the priority that should have
        ipLayer->registerHook(1, this);
        // Initialize the rest of parameters
        IP = inet::L3AddressResolver().addressOf(
                getParentModule()->getParentModule(),
                inet::L3AddressResolver::ADDR_IPv4).toIPv4();
        faulty = par("faulty");
        WATCH_VECTOR(shareSummaries);
    }
}

void TrafficMonitor::finish() {
    recordScalar("HostIP", IP.getInt());
}

inet::IPv4Address TrafficMonitor::getIPAddress(
        inet::INetworkDatagram* datagram) {
    if (dynamic_cast<inet::IPv4Datagram *>(datagram)) {
        inet::IPv4Datagram *dgram = static_cast<inet::IPv4Datagram *>(datagram);
        inet::Ieee802Ctrl *ctrl = check_and_cast<inet::Ieee802Ctrl*>(
                dgram->getControlInfo());
        inet::ARP* arp = check_and_cast<inet::ARP*>(
                getModuleByPath("^.^.networkLayer.arp"));
        if (dgram->getControlInfo() == NULL)
            dgram->setControlInfo(ctrl);
        return arp->getL3AddressFor(ctrl->getSrc()).toIPv4();
    }
    return inet::IPv4Address();
}
}
