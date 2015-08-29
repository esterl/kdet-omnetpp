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
#include "IPvXAddressResolver.h"
#include "ModuleAccess.h"
#include "IPv4.h"
#include "Ieee802Ctrl_m.h"
#include "ARP.h"

INetfilter::IHook::Result TrafficMonitor::datagramPreRoutingHook(
        IPv4Datagram* datagram, const InterfaceEntry* inIE,
        const InterfaceEntry*& outIE, IPv4Address& nextHopAddr) {
    // Update related summaries:
    if (datagram->getTransportProtocol() == DATA_PROTOCOL_NUMBER) {
        IPv4Address addr = getIPAddress(datagram);
        std::vector<Summary*> summaries = findSummaries(addr);
        for (auto it = summaries.begin(); it != summaries.end(); it++) {
            (*it)->updateSummaryPreRouting(datagram);
        }
    }
    return IHook::ACCEPT;
}

INetfilter::IHook::Result TrafficMonitor::datagramForwardHook(
        IPv4Datagram* datagram, const InterfaceEntry* inIE,
        const InterfaceEntry*& outIE, IPv4Address& nextHopAddr) {
    return IHook::ACCEPT;
}

INetfilter::IHook::Result TrafficMonitor::datagramPostRoutingHook(
        IPv4Datagram* datagram, const InterfaceEntry* inIE,
        const InterfaceEntry*& outIE, IPv4Address& nextHopAddr) {
    // Update related summaries:
    if (datagram->getTransportProtocol() == DATA_PROTOCOL_NUMBER) {
        std::vector<Summary*> summaries = findSummaries(nextHopAddr);
        for (auto it = summaries.begin(); it != summaries.end(); it++) {
            (*it)->updateSummaryPostRouting(datagram);
        }
    }
    return IHook::ACCEPT;
}

INetfilter::IHook::Result TrafficMonitor::datagramLocalInHook(
        IPv4Datagram* datagram, const InterfaceEntry* inIE) {
    return IHook::ACCEPT;
}

// TODO this should be done by another class
INetfilter::IHook::Result TrafficMonitor::datagramLocalOutHook(
        IPv4Datagram* datagram, const InterfaceEntry*& outIE,
        IPv4Address& nextHopAddr) {
    // Set ip address
    if (datagram->getTransportProtocol() == DATA_PROTOCOL_NUMBER) {
        datagram->setSrcAddress(IP);
    }
    return IHook::ACCEPT;
}

void TrafficMonitor::setCores(std::vector<std::set<IPv4Address>> newCores) {
    cores = newCores;
}

void TrafficMonitor::setShareSummaries(std::vector<bool> newShareSummaries) {
    shareSummaries = newShareSummaries;
}

void TrafficMonitor::initialize(int stage) {
    if (stage == 3) {
        // Register Hook
        IPv4* ipLayer = check_and_cast<IPv4*>(
                findModuleWhereverInNode("ip", this));
        // TODO check the priority that should have
        ipLayer->registerHook(1, this);
        // Initialize the rest of parameters
        IP = IPvXAddressResolver().addressOf(
                getParentModule()->getParentModule(),
                IPvXAddressResolver::ADDR_PREFER_IPv4).get4();
        faulty = par("faulty");
        WATCH_VECTOR(shareSummaries);
    }
}

void TrafficMonitor::finish() {
    recordScalar("HostIP", IP.getInt());
}

IPv4Address TrafficMonitor::getIPAddress(IPv4Datagram* datagram) {
    Ieee802Ctrl *ctrl = check_and_cast<Ieee802Ctrl*>(
            datagram->getControlInfo());
    ARP* arp = ArpAccess().get();
    if (datagram->getControlInfo() == NULL)
        datagram->setControlInfo(ctrl);
    return arp->getIPv4AddressFor(ctrl->getSrc());
}

