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


#include "Dropper.h"
#include "DropperReport_m.h"
#include "IPv4.h"
#include "kdet_defs.h"

Define_Module(Dropper);

INetfilter::IHook::Result Dropper::datagramPreRoutingHook(
        IPv4Datagram* datagram, const InterfaceEntry* inIE,
        const InterfaceEntry*& outIE, IPv4Address& nextHopAddr) {
    return IHook::ACCEPT;
}

// TODO added just for now
#include <cstdlib>
INetfilter::IHook::Result Dropper::datagramForwardHook(IPv4Datagram* datagram,
        const InterfaceEntry* inIE, const InterfaceEntry*& outIE,
        IPv4Address& nextHopAddr) {
    // Drop with some prob.
    if (datagram->getTransportProtocol() == DATA_PROTOCOL_NUMBER) {
        inPackets++;
        totalIn++;
        if (std::rand() < double(par("dropProbability")) * double(RAND_MAX)) {
            droppedPackets++;
            totalDropped++;
            return IHook::DROP;
        }
        outPackets++;
        totalOut++;
    }
    return IHook::ACCEPT;
}

INetfilter::IHook::Result Dropper::datagramPostRoutingHook(
        IPv4Datagram* datagram, const InterfaceEntry* inIE,
        const InterfaceEntry*& outIE, IPv4Address& nextHopAddr) {
    return IHook::ACCEPT;
}

INetfilter::IHook::Result Dropper::datagramLocalInHook(IPv4Datagram* datagram,
        const InterfaceEntry* inIE) {
    return IHook::ACCEPT;
}

// TODO this should be done by another class
INetfilter::IHook::Result Dropper::datagramLocalOutHook(IPv4Datagram* datagram,
        const InterfaceEntry*& outIE, IPv4Address& nextHopAddr) {
    return IHook::ACCEPT;
}

void Dropper::initialize() {
    faulty = par("faulty");
    // Register Hook
    IPv4* ipLayer = check_and_cast<IPv4*>(findModuleWhereverInNode("ip", this));
    // TODO check the priority that should have
    ipLayer->registerHook(1, this);
    inPackets = 0;
    outPackets = 0;
    droppedPackets = 0;
}

void Dropper::finish() {
    recordScalar("DropProbability", double(par("dropProbability")));
    recordScalar("HostFaulty", bool(par("faulty")));
    recordScalar("TotalDropped", totalDropped);
    recordScalar("TotalIn", totalIn);
}
void Dropper::handleMessage(cMessage *msg) {
    // Send report
    DropperReport* report = new DropperReport("Dropper report");
    report->setDroppedPackets(droppedPackets);
    report->setOutPackets(outPackets);
    report->setInPackets(inPackets);
    report->setFaulty(faulty);
    send(report, "out");
    // Reset counters
    droppedPackets = 0;
    outPackets = 0;
    inPackets = 0;
    // Delete message
    delete msg;
}

