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

#ifndef LINKSKETCHSUMMARY_H_
#define LINKSKETCHSUMMARY_H_

#include "LinkSummary.h"
#include "SketchSummary.h"
#include "sketches/sketches.h"
typedef Sketch<uint32_t> NetworkSketch;
typedef std::map<uint32_t, NetworkSketch*> SketchHash;

/**
 * Traffic summary based on sketches and links.
 */
class LinkSketchSummary: public LinkSummary, public SketchSummary {
public:
    LinkSketchSummary(IPv4Address reporter, IPv4Address link =
            IPv4Address::UNSPECIFIED_ADDRESS);
    LinkSketchSummary(const LinkSketchSummary& other);
    LinkSketchSummary& operator=(const LinkSketchSummary& other);
    virtual ~LinkSketchSummary();
    virtual void updateSummaryPreRouting(IPv4Datagram* pkt);
    virtual void updateSummaryPostRouting(IPv4Datagram* pkt);
    virtual void clear();
    virtual Summary* copy() const;
    virtual void add(Summary* otherPtr);
    virtual double estimateDrop(std::set<IPv4Address> core);
    virtual double estimateIn(std::set<IPv4Address> core);
    virtual double estimateOut(std::set<IPv4Address> core);
    virtual double getBytes();
    virtual double getOptimizedBytes();
    virtual void optimize(std::set<IPv4Address> kHopNodes);
    static void setBaseSketch(cModule* module);
    static void setBaseSketch(NetworkSketch* sketch);
    static NetworkSketch* getBaseSketch();
protected:
    void updateSketch(NetworkSketch* sketch, IPv4Datagram* pkt);
    static NetworkSketch* baseSketch;
    IPv4Address neighbor;
    NetworkSketch* from;
    NetworkSketch* to;
    SketchHash src;
    SketchHash dst;
};

#endif /* LINKSKETCHSUMMARY_H_ */
