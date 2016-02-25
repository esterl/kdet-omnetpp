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


#ifndef __KDET_DROPPER_H_
#define __KDET_DROPPER_H_

#include <omnetpp.h>
#include <INetfilter.h>

namespace kdet{
/**
 * The dropper module, drops a percentage of the forwarding packets as
 * configured in the *.ini file.
 */
class Dropper: public cSimpleModule, public inet::INetfilter::IHook {
public:
    virtual inet::INetfilter::IHook::Result datagramPreRoutingHook(inet::INetworkDatagram* datagram,
            const inet::InterfaceEntry* inIE, const inet::InterfaceEntry*& outIE,
            inet::L3Address& nextHopAddr);
    virtual inet::INetfilter::IHook::Result datagramForwardHook(inet::INetworkDatagram* datagram,
            const inet::InterfaceEntry* inIE, const inet::InterfaceEntry*& outIE,
            inet::L3Address& nextHopAddr);
    virtual inet::INetfilter::IHook::Result datagramPostRoutingHook(inet::INetworkDatagram* datagram,
            const inet::InterfaceEntry* inIE, const inet::InterfaceEntry*& outIE,
            inet::L3Address& nextHopAddr);
    virtual inet::INetfilter::IHook::Result datagramLocalInHook(inet::INetworkDatagram* datagram,
            const inet::InterfaceEntry* inIE);
    virtual inet::INetfilter::IHook::Result datagramLocalOutHook(inet::INetworkDatagram* datagram,
            const inet::InterfaceEntry*& outIE, inet::L3Address& nextHopAddr);
protected:
    virtual void initialize();
    virtual void finish();
    virtual void handleMessage(cMessage *msg);
    unsigned inPackets;
    unsigned outPackets;
    unsigned droppedPackets;
    unsigned totalIn;
    unsigned totalOut;
    unsigned totalDropped;
    bool faulty;
    cOutVector dataOverhead;

};
}
#endif
