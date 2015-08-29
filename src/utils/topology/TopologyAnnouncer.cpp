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


#include "TopologyAnnouncer.h"
#include "NeighborsAnnouncement_m.h"
#include "RoutingTableAccess.h"

Define_Module(TopologyAnnouncer);

void TopologyAnnouncer::initialize() {
    interval = par("updateInterval");
    rt = RoutingTableAccess().get();
    scheduler = new cMessage("Scheduler");
    scheduleAt(simTime(), scheduler);
    seqNumber = 0;
}

template <typename iterator>
void printIter(iterator begin, iterator end){
    for (iterator it=begin; it != end; it++){
        std::cout << (*it) << " ";
    }
    std::cout << endl;
}

void TopologyAnnouncer::handleMessage(cMessage *msg) {
    std::set<IPv4Address> new_neigh = neighbors;
    seqNumber++;
    for (int i = 0; i < rt->getNumRoutes(); i++) {
        IPv4Route *route = rt->getRoute(i);
        if (route->getDestination() == route->getGateway()) {
            //1-hop node
            new_neigh.insert(route->getDestination());
        }
    }

    // Has it changed?
    if(new_neigh != neighbors){
        // Send new message
        NeighborsAnnouncement* announcement = new NeighborsAnnouncement("neighborsAnnouncement");
        announcement->setNode(rt->getRouterId());
        announcement->setNeighbors(new_neigh);
        announcement->setSeqNumber(seqNumber);
        send(announcement, "out");
        // Update neighbors
        neighbors = new_neigh;
    }

    // Re-schedule msg
    scheduleAt(simTime()+interval, msg);
}

TopologyAnnouncer::~TopologyAnnouncer(){
    cancelAndDelete(scheduler);
}
