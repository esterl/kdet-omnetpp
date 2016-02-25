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

namespace kdet{
/**
 * Base class for the traffic monitoring. It mainly implements the hook
 * to the net layer.
 */
class TrafficMonitor: public cSimpleModule, public inet::INetfilter::IHook {
public:
    virtual inet::INetfilter::IHook::Result datagramPreRoutingHook(
            inet::INetworkDatagram* datagram, const inet::InterfaceEntry* inIE,
            const inet::InterfaceEntry*& outIE, inet::L3Address& nextHopAddr);
    virtual inet::INetfilter::IHook::Result datagramForwardHook(
            inet::INetworkDatagram* datagram, const inet::InterfaceEntry* inIE,
            const inet::InterfaceEntry*& outIE, inet::L3Address& nextHopAddr);
    virtual inet::INetfilter::IHook::Result datagramPostRoutingHook(
            inet::INetworkDatagram *datagram,
            const inet::InterfaceEntry *inputInterfaceEntry,
            const inet::InterfaceEntry *& outputInterfaceEntry,
            inet::L3Address& nextHopAddress);
    virtual inet::INetfilter::IHook::Result datagramLocalInHook(
            inet::INetworkDatagram* datagram, const inet::InterfaceEntry* inIE);
    virtual inet::INetfilter::IHook::Result datagramLocalOutHook(
            inet::INetworkDatagram* datagram,
            const inet::InterfaceEntry*& outIE, inet::L3Address& nextHopAddr);
    virtual void setCores(std::vector<std::set<inet::IPv4Address>>);
    virtual void setShareSummaries(std::vector<bool>);

protected:
    virtual void initialize(int stage);
    virtual int numInitStages() const {
        return inet::NUM_INIT_STAGES;
    }
    ;
    virtual void handleMessage(cMessage *msg) = 0;
    virtual void finish();
    virtual void resetSummaries() = 0;
    virtual std::vector<Summary*> findSummaries(inet::IPv4Address addr) = 0;
    inet::IPv4Address getIPAddress(inet::INetworkDatagram* datagram);
    inet::IPv4Address IP;
    bool faulty;
    std::vector<std::set<inet::IPv4Address>> cores;
    std::vector<bool> shareSummaries;

};
}
#endif
