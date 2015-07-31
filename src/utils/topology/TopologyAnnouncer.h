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

#ifndef __KDET_TOPOLOGYANNOUNCER_H_
#define __KDET_TOPOLOGYANNOUNCER_H_

#include <omnetpp.h>
#include "IPv4Address.h"
#include <set>
#include "IRoutingTable.h"
/**
 * Periodically checks whether a node's neighbors have changed, and if so
 * reports the GraphServer about the new neighbors.
 */
class TopologyAnnouncer : public cSimpleModule
{
private:
    cMessage* scheduler;
    int seqNumber;
  protected:
    simtime_t interval;
    IRoutingTable* rt;
    std::set<IPv4Address> neighbors;
    IPv4Address IPAddress;
    virtual void initialize();
    virtual void handleMessage(cMessage *msg);
    virtual ~TopologyAnnouncer();
};

#endif
