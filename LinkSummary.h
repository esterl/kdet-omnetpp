//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public License
// along with this program.  If not, see http://www.gnu.org/licenses/.
// 

#ifndef LINKSUMMARY_H_
#define LINKSUMMARY_H_

#include <IPvXAddress.h>
#include <IPv4Datagram.h>

class LinkSummary {
public:
    LinkSummary(IPv4Address hostIP = IPv4Address::UNSPECIFIED_ADDRESS,
            IPv4Address neighborIP = IPv4Address::UNSPECIFIED_ADDRESS) {host=hostIP; neighbor=neighborIP;};
    virtual ~LinkSummary() { };
    virtual IPv4Address getHost(){ return host; };
    virtual IPv4Address getNeighbor(){ return neighbor; };
    virtual void updateSummaryPreRouting(IPv4Datagram* pkt) = 0;
    virtual void updateSummaryPostRouting(IPv4Datagram* pkt) = 0;
    virtual void clear() = 0;
    virtual LinkSummary* copy() const = 0;
    virtual void add(LinkSummary* other) = 0;
    virtual double estimateDrop(std::set<IPv4Address> core) = 0;
    virtual double getBytes() = 0;
    virtual void optimizeSummary(std::set<IPv4Address> coreNodes) = 0;
protected:
    IPv4Address host;
    IPv4Address neighbor;
};

#endif /* LINKSUMMARY_H_ */
