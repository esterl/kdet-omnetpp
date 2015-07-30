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


#ifndef COREMONITOR_H_
#define COREMONITOR_H_

#include "TrafficMonitor.h"
#include "GraphServer.h"

/**
 * Traffic monitor based on the first strategy of "KDet: Coordinated Detection
 * of Forwarding Faults in Wireless Community Networks". Every time a packet
 * is received or sent, the traffic summaries of the cores it has traversed or
 * will traverse are updated.
 */
class CoreMonitor: public TrafficMonitor {
public:
    virtual void setCores(std::vector<std::set<IPv4Address>>);
protected:
    virtual void initialize(int stage);
    virtual void handleMessage(cMessage *msg);
    virtual void finish();
    virtual void resetSummaries();
    virtual void deleteSummaries();
    virtual std::vector<Summary*> findSummaries(IPv4Address addr);
    std::vector<Summary*> summaries;
};

#endif /* COREMONITOR_H_ */
