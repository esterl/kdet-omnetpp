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

#ifndef CORESUMMARY_H_
#define CORESUMMARY_H_

#include "Summary.h"
#include "TrafficSketch.h"
#include <set>

namespace kdet{
/**
 * TODO Doc
 */
class CoreSummary: public Summary {
public:
    CoreSummary(const inet::IPv4Address reporterIP = inet::IPv4Address::UNSPECIFIED_ADDRESS,
            const std::set<inet::IPv4Address>& coreIPs = std::set<inet::IPv4Address>());
    CoreSummary(const CoreSummary& other);
    virtual ~CoreSummary() {};
    virtual std::set<inet::IPv4Address> getCore() {return core;};
    virtual std::vector<inet::IPv4Address> getID();
    virtual std::set<inet::IPv4Address> getSendTo(inet::IPv4Address localIP,
            std::set<inet::IPv4Address> neighbors);
    CoreSummary& operator=(const CoreSummary& other);
    virtual void updateSummaryPreRouting(inet::INetworkDatagram* pkt);
    virtual void updateSummaryPostRouting(inet::INetworkDatagram* pkt);
    virtual void clear();
    virtual Summary* copy() const;
    virtual void print(){};
    virtual void add(Summary* otherPtr);
    virtual double estimateDrop(std::set<inet::IPv4Address> core);
    virtual double estimateIn(std::set<inet::IPv4Address> core);
    virtual double estimateOut(std::set<inet::IPv4Address> core);
    virtual double getDrop(std::set<inet::IPv4Address> core);
    virtual double getIn(std::set<inet::IPv4Address> core);
    virtual double getOut(std::set<inet::IPv4Address> core);
    virtual double getBytes();
    virtual double getOptimizedBytes();
protected:
    std::set<inet::IPv4Address> core;
    std::set<inet::IPv4Address> boundaryReportsIncluded;
    TrafficSketch sketchIn, sketchOut;

};
}
#endif /* CORESUMMARY_H_ */
