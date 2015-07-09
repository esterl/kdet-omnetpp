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

#ifndef SKETCHSUMMARY_H_
#define SKETCHSUMMARY_H_

#include "LinkSummary.h"
#include "sketches/sketches.h"
typedef Sketch<uint32_t> NetworkSketch;
typedef std::map<uint32_t, NetworkSketch*> SketchHash;

class SketchSummary: public LinkSummary {
public:
    SketchSummary(IPv4Address hostIP = IPv4Address::UNSPECIFIED_ADDRESS,
            IPv4Address neighborIP = IPv4Address::UNSPECIFIED_ADDRESS);
    virtual ~SketchSummary();
    virtual void updateSummaryPreRouting(IPv4Datagram* pkt);
    virtual void updateSummaryPostRouting(IPv4Datagram* pkt);
    virtual void clear();
    virtual LinkSummary* copy()const;
    virtual void add(LinkSummary* otherPtr);
    virtual double estimateDrop(std::set<IPv4Address> core);
    virtual double estimateIn(std::set<IPv4Address> core);
    virtual double estimateOut(std::set<IPv4Address> core);
    virtual double getBytes();
    virtual void optimizeSummary(std::set<IPv4Address> coreNodes);
    static void setBaseSketch(cModule* module);
    static void setBaseSketch(NetworkSketch* sketch);
    static NetworkSketch* getBaseSketch();
protected:
    void updateSketch(NetworkSketch* sketch, IPv4Datagram* pkt);
    static NetworkSketch* baseSketch;
    NetworkSketch* from;
    NetworkSketch* to;
    SketchHash src;
    SketchHash dst;
};

#endif /* SKETCHSUMMARY_H_ */
