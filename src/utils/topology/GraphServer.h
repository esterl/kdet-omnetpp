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


#ifndef __KDET_GRAPHSERVER_H_
#define __KDET_GRAPHSERVER_H_

#include <omnetpp.h>
#include <set>
#include "NeighborsAnnouncement_m.h"
#include "CoresUpdate_m.h"
#include <string>

namespace kdet{
/**
 * GraphServer allows nodes to register their neighbors and computes the
 * cores, boundaries and gives the starting seed for cores monitoring.
 */
typedef std::vector<std::vector<int>> intListList;
typedef std::vector<std::set<inet::IPv4Address>> IPSetList;

class GraphServer: public cSimpleModule {
public:
    std::set<inet::IPv4Address> getNeighbors(inet::IPv4Address addr);
    std::set<inet::IPv4Address> getNeighbors(inet::IPv4Address, unsigned);
    IPSetList getCores(inet::IPv4Address);
    IPSetList getAllCores();
    IPSetList getAllBoundaries();
protected:
    std::map<int, int> index_to_ip;
    std::map<int, int> ip_to_index;
    std::map<int, int> index_to_gate;
    int k;
    std::vector<intListList> cores;
    std::vector<intListList> boundaries;
    std::vector<std::vector<unsigned>> secrets;
    IPSetList allCores;
    IPSetList allBoundaries;
    std::vector<unsigned> allSecrets;
    bool dirty;
    std::vector<std::set<int>> networkGraph;
    std::string networkString;
    virtual void initialize();
    virtual void finish();
    virtual void handleMessage(cMessage *msg);
    void updateGraph(NeighborsAnnouncement* msg);
    std::set<int> getNeighbors(int node);
    std::set<int> getNeighborhood(std::set<int> nodes);
    int getVertixIndex(inet::IPv4Address node);
    std::vector<std::vector<int>> getCores(int node);
    void sendUpdate();
    void updateCores();
    std::vector<std::vector<int>> getCoresRecursive(unsigned k,
            std::vector<int> partialCore, std::set<int> &possibleNodes,
            std::set<int> usedNodes);
    std::vector<int> getBoundary(std::vector<int> core);
    IPSetList indexToIPList(intListList cores);
};
}
#endif
