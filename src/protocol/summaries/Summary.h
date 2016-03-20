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


#ifndef SUMMARY_H_
#define SUMMARY_H_

#include <L3Address.h>
#include <IPv4Datagram.h>
#include <vector>
#include <set>

namespace kdet{

// TODO do we want sets or vectors?
/**
 * Base class for traffic summaries.
 */
class Summary {
public:
    Summary(inet::IPv4Address reporterIP = inet::IPv4Address::UNSPECIFIED_ADDRESS) {reporter=reporterIP;};
    virtual ~Summary() { };
    virtual inet::IPv4Address getReporter(){ return reporter; };
    virtual void updateSummaryPreRouting(inet::INetworkDatagram* pkt) = 0;
    virtual void updateSummaryPostRouting(inet::INetworkDatagram* pkt) = 0;
    virtual void clear() = 0;
    virtual Summary* copy() const = 0;
    virtual void print() = 0;
    virtual void add(Summary* other) = 0;
    virtual double estimateDrop(std::set<inet::IPv4Address> core) = 0;
    virtual double estimateIn(std::set<inet::IPv4Address> core) = 0;
    virtual double estimateOut(std::set<inet::IPv4Address> core) = 0;
    virtual double getDrop(std::set<inet::IPv4Address> core) = 0;
    virtual double getIn(std::set<inet::IPv4Address> core) = 0;
    virtual double getOut(std::set<inet::IPv4Address> core) = 0;
    virtual double getBytes() = 0;
    virtual double getOptimizedBytes() = 0;
    virtual std::vector<inet::IPv4Address> getID() = 0;
    virtual std::set<inet::IPv4Address> getSendTo(inet::IPv4Address localIP,
            std::set<inet::IPv4Address> neighbors) = 0;

protected:
    inet::IPv4Address reporter;
};
}
#endif /* SUMMARY_H_ */
