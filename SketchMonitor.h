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

#ifndef __KDET_SKETCHMONITOR_H_
#define __KDET_SKETCHMONITOR_H_

#include <unordered_map>
#include <omnetpp.h>
#include <INetfilter.h>
#include <IPv4.h>
#include <IPv4Address.h>
#include "SketchSummary.h"
#include "Report.h"
class SketchMonitor: public cSimpleModule, public INetfilter::IHook {
public:
    LinkSummary* getLinkSummary(IPv4Address address);
    virtual INetfilter::IHook::Result datagramPreRoutingHook(IPv4Datagram* datagram,
            const InterfaceEntry* inIE, const InterfaceEntry*& outIE,
            IPv4Address& nextHopAddr);
    virtual INetfilter::IHook::Result datagramForwardHook(IPv4Datagram* datagram,
            const InterfaceEntry* inIE, const InterfaceEntry*& outIE,
            IPv4Address& nextHopAddr);
    virtual INetfilter::IHook::Result datagramPostRoutingHook(IPv4Datagram* datagram,
            const InterfaceEntry* inIE, const InterfaceEntry*& outIE,
            IPv4Address& nextHopAddr);
    virtual INetfilter::IHook::Result datagramLocalInHook(IPv4Datagram* datagram,
            const InterfaceEntry* inIE);
    virtual INetfilter::IHook::Result datagramLocalOutHook(IPv4Datagram* datagram,
            const InterfaceEntry*& outIE, IPv4Address& nextHopAddr);

protected:
    virtual void initialize();
    virtual void handleMessage(cMessage *msg);
    virtual void finish();
    virtual void resetSummaries();
    IPv4Address getIPAddress(IPv4Datagram* datagram);
    SketchSummary* findSummary(IPv4Address addr);
    IPv4Address getIP();
    LinkSummariesHash summaries;
    IPv4* ipLayer;
    IPv4Address IP;
    bool faulty;
};

#endif
