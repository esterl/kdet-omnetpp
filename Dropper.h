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

#ifndef __KDET_DROPPER_H_
#define __KDET_DROPPER_H_

#include <omnetpp.h>
#include <INetfilter.h>

/**
 * TODO - Generated class
 */
class Dropper: public cSimpleModule, public INetfilter::IHook {
public:
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
    virtual void finish();
    virtual void handleMessage(cMessage *msg);
    unsigned inPackets;
    unsigned outPackets;
    unsigned droppedPackets;
    bool faulty;
};

#endif
