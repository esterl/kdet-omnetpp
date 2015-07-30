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

#ifndef SUMMARY_H_
#define SUMMARY_H_

#include <IPvXAddress.h>
#include <IPv4Datagram.h>

class Summary {
public:
    Summary(IPv4Address reporterIP = IPv4Address::UNSPECIFIED_ADDRESS) {reporter=reporterIP;};
    virtual ~Summary() { };
    virtual IPv4Address getReporter(){ return reporter; };
    virtual void updateSummaryPreRouting(IPv4Datagram* pkt) = 0;
    virtual void updateSummaryPostRouting(IPv4Datagram* pkt) = 0;
    virtual void clear() = 0;
    virtual Summary* copy() const = 0;
    virtual void add(Summary* other) = 0;
    virtual double estimateDrop(std::set<IPv4Address> core) = 0;
    virtual double estimateIn(std::set<IPv4Address> core) = 0;
    virtual double estimateOut(std::set<IPv4Address> core) = 0;
    virtual double getBytes() = 0;
protected:
    IPv4Address reporter;
};

#endif /* SUMMARY_H_ */
