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


#ifndef LINKMONITOR_H_
#define LINKMONITOR_H_

#include "TrafficMonitor.h"
#include "Report.h"
#include <unordered_map>

namespace kdet{
/**
 * Traffic monitor based on the second strategy of "KDet: Coordinated Detection
 * of Forwarding Faults in Wireless Community Networks". Only the summaries of
 * the link from where a packet is received and sent are updated. It is the job
 * of the detector to later compose those to create the core's summary.
 */
class LinkMonitor: public TrafficMonitor {
public:
    virtual void setCores(std::vector<std::set<inet::IPv4Address>> cores);
protected:
    virtual void handleMessage(cMessage *msg);
    virtual void finish();
    virtual void resetSummaries();
    virtual std::vector<Summary*> findSummaries(inet::IPv4Address addr);
    //LinkSummariesHash summaries;
    std::unordered_map<int, Summary*> summaries;
};
}
#endif /* LINKMONITOR_H_ */
