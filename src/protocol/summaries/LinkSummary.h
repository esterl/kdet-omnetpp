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

#ifndef LINKSUMMARY_H_
#define LINKSUMMARY_H_

#include "Summary.h"
#include "TrafficSketch.h"

namespace kdet{

typedef std::map<uint32_t, TrafficSketch> SketchHash;
/**
 * Traffic summary based on the second strategy: it keeps information about
 * all the traffic that comes from and to a given link, as well as which node
 * is the source or destination of the packet.
 */
class LinkSummary : public Summary{
public:
    LinkSummary(inet::IPv4Address reporterIP,
            inet::IPv4Address neighborIP) {reporter=reporterIP; neighbor=neighborIP;};
    LinkSummary(const LinkSummary& other);
    virtual ~LinkSummary(){};
    virtual void updateSummaryPreRouting(inet::INetworkDatagram* pkt);
    virtual void updateSummaryPostRouting(inet::INetworkDatagram* pkt);
    virtual void clear();
    virtual Summary* copy() const;
    virtual void add(Summary* otherPtr);
    virtual double estimateDrop(std::set<inet::IPv4Address> core);
    virtual double estimateIn(std::set<inet::IPv4Address> core);
    virtual double estimateOut(std::set<inet::IPv4Address> core);
    virtual double getIn(std::set<inet::IPv4Address> core);
    virtual double getOut(std::set<inet::IPv4Address> core);
    virtual double getDrop(std::set<inet::IPv4Address> core);
    virtual double getBytes();
    virtual double getOptimizedBytes();
    virtual void print();
    virtual void optimize(std::set<inet::IPv4Address> kHopNodes);
    virtual inet::IPv4Address getNeighbor(){ return neighbor; };
    virtual std::vector<inet::IPv4Address> getID();
    virtual std::set<inet::IPv4Address> getSendTo(inet::IPv4Address localIP,
            std::set<inet::IPv4Address> neighbors);
protected:
    inet::IPv4Address neighbor;
    TrafficSketch from;
    TrafficSketch to;
    SketchHash src;
    SketchHash dst;
};
}
#endif /* LINKSUMMARY_H_ */
