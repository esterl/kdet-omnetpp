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

#ifndef CORESKETCHSUMMARY_H_
#define CORESKETCHSUMMARY_H_

#include "CoreSummary.h"
#include "SketchSummary.h"

/**
 * Traffic summary based on cores and sketches.
 */
class CoreSketchSummary: public CoreSummary, public SketchSummary {
public:
    CoreSketchSummary(const IPv4Address reporter, const std::set<IPv4Address>& core);
    CoreSketchSummary(const CoreSketchSummary& other);
    CoreSketchSummary& operator=(const CoreSketchSummary& other);
    virtual ~CoreSketchSummary();
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

protected:
    std::set<IPv4Address> boundaryReportsIncluded;
    NetworkSketch *sketchIn, *sketchOut;

};

#endif /* CORESKETCHSUMMARY_H_ */
