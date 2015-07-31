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

#include <IPvXAddress.h>
#include <IPv4Datagram.h>
#include "Summary.h"

/**
 * Traffic summary based on the second strategy: it keeps information about
 * all the traffic that comes from and to a given link, as well as which node
 * is the source or destination of the packet.
 */
class LinkSummary : public Summary{
public:
    LinkSummary(IPv4Address hostIP = IPv4Address::UNSPECIFIED_ADDRESS,
            IPv4Address neighborIP = IPv4Address::UNSPECIFIED_ADDRESS) {host=hostIP; neighbor=neighborIP;};
    virtual ~LinkSummary() { };
    virtual IPv4Address getHost(){ return host; };
    virtual IPv4Address getNeighbor(){ return neighbor; };
    virtual void updateSummaryPreRouting(IPv4Datagram* pkt) = 0;
    virtual void updateSummaryPostRouting(IPv4Datagram* pkt) = 0;
    virtual void clear() = 0;
    virtual Summary* copy() const = 0;
    virtual void add(Summary* other) = 0;
    virtual double estimateDrop(std::set<IPv4Address> core) = 0;
    virtual double estimateIn(std::set<IPv4Address> core) = 0;
    virtual double estimateOut(std::set<IPv4Address> core) = 0;
    virtual double getBytes() = 0;
    virtual void optimizeSummary(std::set<IPv4Address> coreNodes) = 0;
protected:
    IPv4Address host;
    IPv4Address neighbor;
};

#endif /* LINKSUMMARY_H_ */
