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

#include "CoreSummary.h"

namespace kdet {
CoreSummary::CoreSummary(const inet::IPv4Address reporter_var,
        const std::set<inet::IPv4Address>& core_var) {
    reporter = reporter_var;
    core = core_var;
    boundaryReportsIncluded.insert(reporter);
}

CoreSummary::CoreSummary(const CoreSummary& other) {
    sketchIn = other.sketchIn;
    sketchOut = other.sketchOut;
    reporter = other.reporter;
    core = other.core;
    boundaryReportsIncluded = other.boundaryReportsIncluded;
}

CoreSummary& CoreSummary::operator=(const CoreSummary& other) {
    sketchIn = other.sketchIn;
    sketchOut = other.sketchOut;
    reporter = other.reporter;
    core = other.core;
    boundaryReportsIncluded = other.boundaryReportsIncluded;
    return *this;
}

void CoreSummary::updateSummaryPreRouting(inet::INetworkDatagram* pkt) {
    if (core.count(pkt->getSourceAddress().toIPv4()) == 0
            && core.count(pkt->getDestinationAddress().toIPv4()) == 0) {
        try {
            sketchOut.update(pkt);
        } catch (cRuntimeError e) {
            // Is not a Serializable packet
            std::cout << "No serializable" << endl;
        }
    }
}

void CoreSummary::updateSummaryPostRouting(inet::INetworkDatagram* pkt) {
    if (core.count(pkt->getSourceAddress().toIPv4()) == 0
            && core.count(pkt->getDestinationAddress().toIPv4()) == 0) {
        try {
            sketchIn.update(pkt);
        } catch (cRuntimeError e) {
            // Is not a Serializable packet
            std::cout << "No serializable" << endl;
        }
    }
}

void CoreSummary::clear() {
    sketchIn.clear();
    sketchOut.clear();
}

Summary* CoreSummary::copy() const {
    CoreSummary* summary = new CoreSummary(*this);
    return summary;
}

void CoreSummary::add(Summary* otherPtr) {
    CoreSummary* other = dynamic_cast<CoreSummary*>(otherPtr);
    if (other != NULL) {
        sketchIn += other->sketchIn;
        sketchOut += other->sketchOut;
        boundaryReportsIncluded.insert(other->getReporter());
    }
}

double CoreSummary::estimateDrop(std::set<inet::IPv4Address> core) {
    TrafficSketch sketch = sketchIn;
    sketch -= sketchOut;
    double dropped = sketch.second_moment();
    return dropped;
}

double CoreSummary::estimateIn(std::set<inet::IPv4Address> core) {
    return sketchIn.second_moment();
}

double CoreSummary::estimateOut(std::set<inet::IPv4Address> core) {
    return sketchOut.second_moment();
}

double CoreSummary::getDrop(std::set<inet::IPv4Address> core) {
    TrafficSketch sketch = sketchIn;
    sketch -= sketchOut;
    return sketch.get_counter();
}

double CoreSummary::getIn(std::set<inet::IPv4Address> core) {
    return sketchIn.get_counter();
}

double CoreSummary::getOut(std::set<inet::IPv4Address> core) {
    return sketchOut.get_counter();
}

double CoreSummary::getBytes() {
    return sketchIn.get_bytes() + sketchOut.get_bytes();
}

double CoreSummary::getOptimizedBytes() {
    return sketchIn.get_optimized_bits() / 8
            + sketchOut.get_optimized_bits() / 8;
}

std::vector<inet::IPv4Address> CoreSummary::getID() {
    std::vector<inet::IPv4Address> id;
    id.push_back(reporter);
    for (auto it = core.begin(); it != core.end(); it++)
        id.push_back(*it);
    return id;
}
std::set<inet::IPv4Address> CoreSummary::getSendTo(inet::IPv4Address localIP,
        std::set<inet::IPv4Address> neighbors) {
    if (localIP == reporter) {
        std::set<inet::IPv4Address> result;
        for (auto it = core.begin(); it != core.end(); it++) {
            if (neighbors.count(*it) > 0) {
                result.insert(*it);
            }
        }
        return result;
    } else if (core.count(localIP) > 0) {
        return neighbors;
    }
    return std::set<inet::IPv4Address>();
}
}
