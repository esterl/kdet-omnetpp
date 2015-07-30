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

#ifndef __KDET_TRAFFICMONITOR_H_
#define __KDET_TRAFFICMONITOR_H_

#include <omnetpp.h>
#include "INetfilter.h"
#include <vector>
#include "Summary.h"

/**
 * Base class for the traffic monitoring. It mainly implements the hook
 * to the net layer.
 */
class TrafficMonitor: public cSimpleModule, public INetfilter::IHook {
public:
    virtual INetfilter::IHook::Result datagramPreRoutingHook(
            IPv4Datagram* datagram, const InterfaceEntry* inIE,
            const InterfaceEntry*& outIE, IPv4Address& nextHopAddr);
    virtual INetfilter::IHook::Result datagramForwardHook(
            IPv4Datagram* datagram, const InterfaceEntry* inIE,
            const InterfaceEntry*& outIE, IPv4Address& nextHopAddr);
    virtual INetfilter::IHook::Result datagramPostRoutingHook(
            IPv4Datagram* datagram, const InterfaceEntry* inIE,
            const InterfaceEntry*& outIE, IPv4Address& nextHopAddr);
    virtual INetfilter::IHook::Result datagramLocalInHook(
            IPv4Datagram* datagram, const InterfaceEntry* inIE);
    virtual INetfilter::IHook::Result datagramLocalOutHook(
            IPv4Datagram* datagram, const InterfaceEntry*& outIE,
            IPv4Address& nextHopAddr);
    virtual void setCores(std::vector<std::set<IPv4Address>>);
    virtual void setShareSummaries(std::vector<bool>);

protected:
    virtual void initialize(int stage);
    virtual int numInitStages() const { return 4;};
    virtual void handleMessage(cMessage *msg) = 0;
    virtual void finish();
    virtual void resetSummaries() = 0;
    virtual std::vector<Summary*> findSummaries(IPv4Address addr) = 0;
    IPv4Address getIPAddress(IPv4Datagram* datagram);
    IPv4Address IP;
    bool faulty;
    std::vector<std::set<IPv4Address>> cores;
    std::vector<bool> shareSummaries;

};

#endif
