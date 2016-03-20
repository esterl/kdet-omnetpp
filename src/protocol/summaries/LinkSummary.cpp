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

#include "LinkSummary.h"

namespace kdet {

LinkSummary::LinkSummary(const LinkSummary& other) {
    reporter = other.reporter;
    neighbor = other.neighbor;
    from = other.from;
    to = other.to;
    for (auto it : other.src) {
        src[it.first] = it.second;
    }
    for (auto it : other.dst) {
        dst[it.first] = it.second;
    }
}

void LinkSummary::updateSummaryPreRouting(inet::INetworkDatagram* pkt) {
    // Update from and src sketches
    try {
        from.update(pkt);
        // Create Sketch if it doesn't exist
        inet::IPv4Address address = pkt->getSourceAddress().toIPv4();
        if (src.count(address.getInt()) == 0) {
            src[address.getInt()] = TrafficSketch();
        }
        src[address.getInt()].update(pkt);
    } catch (cRuntimeError e) {
        // Is not a Serializable packet
        std::cout << "No serializable" << endl;
    }
}

void LinkSummary::updateSummaryPostRouting(inet::INetworkDatagram* pkt) {
   // Update from and src sketches
    try {
        to.update(pkt);
        // Create Sketch if it doesn't exist
        inet::IPv4Address address = pkt->getDestinationAddress().toIPv4();
        if (dst.count(address.getInt()) == 0) {
            dst[address.getInt()] = TrafficSketch();
        }
        dst[address.getInt()].update(pkt);
    } catch (cRuntimeError e) {
        // Is not a Serializable packet
        std::cout << "No serializable" << endl;
    }
}

void LinkSummary::clear() {
    to.clear();
    from.clear();
    for (auto it : src) {
        src[it.first].clear();
    }
    for (auto it : dst) {
        dst[it.first].clear();
        //it.second.clear();
    }
}

Summary* LinkSummary::copy() const {
    LinkSummary* summary = new LinkSummary(*this);
    return summary;
}

void merge(SketchHash& hash1, const SketchHash& hash2) {
    for (auto it : hash2) {
        if (hash1.count(it.first) == 0) {
            hash1[it.first] = it.second;
        } else {
            hash1[it.first] += it.second;
        }
    }
}

void LinkSummary::add(Summary* otherPtr) {
    LinkSummary* other = dynamic_cast<LinkSummary*>(otherPtr);
    if (other != NULL) {
        from += other->from;
        to += other->to;
        merge(src, other->src);
        merge(dst, other->dst);
    }
}

double LinkSummary::estimateDrop(std::set<inet::IPv4Address> core) {
    TrafficSketch sketchIn = to;
    for (auto it : dst) {
        if (core.count(inet::IPv4Address(it.first)) != 0) {
            sketchIn -= it.second;
        }
    }
    TrafficSketch sketchOut = from;
    for (auto it : src) {
        if (core.count(inet::IPv4Address(it.first)) != 0) {
            sketchOut -= it.second;
        }
    }
    sketchIn -= sketchOut;
    double dropped = sketchIn.second_moment();
    return dropped;
}

double LinkSummary::estimateIn(std::set<inet::IPv4Address> core) {
   TrafficSketch sketchIn = to;
    for (auto it : dst) {
        if (core.count(inet::IPv4Address(it.first)) != 0) {
            sketchIn -= it.second;
        }
    }
    double received = sketchIn.second_moment();
    return received;
}

double LinkSummary::estimateOut(std::set<inet::IPv4Address> core) {
   TrafficSketch sketchOut = from;
    for (auto it : src) {
        if (core.count(inet::IPv4Address(it.first)) != 0) {
            sketchOut -= it.second;
        }
    }
    double sent = sketchOut.second_moment();
    return sent;
}

double LinkSummary::getDrop(std::set<inet::IPv4Address> core) {
    TrafficSketch sketchIn = to;
    for (auto it : dst) {
        if (core.count(inet::IPv4Address(it.first)) != 0) {
            sketchIn -= it.second;
        }
    }
    TrafficSketch sketchOut = from;
    for (auto it : src) {
        if (core.count(inet::IPv4Address(it.first)) != 0) {
            sketchOut -= it.second;
        }
    }
    sketchIn -= sketchOut;
    return sketchIn.get_counter();
}

double LinkSummary::getIn(std::set<inet::IPv4Address> core) {
   TrafficSketch sketchIn = to;
    for (auto it : dst) {
        if (core.count(inet::IPv4Address(it.first)) != 0) {
            sketchIn -= it.second;
        }
    }
    return sketchIn.get_counter();
}

double LinkSummary::getOut(std::set<inet::IPv4Address> core) {
   TrafficSketch sketchOut = from;
    for (auto it : src) {
        if (core.count(inet::IPv4Address(it.first)) != 0) {
            sketchOut -= it.second;
        }
    }
    return sketchOut.get_counter();
}

double LinkSummary::getBytes() {
// Sum the bytes required to store each Sketch
    double bytes = 0.;
    bytes += from.get_bytes();
    bytes += to.get_bytes();
    for (auto it : src) {
        bytes += it.second.get_bytes();
    }
    for (auto it : dst) {
        bytes += it.second.get_bytes();
    }
    return bytes;
}

double LinkSummary::getOptimizedBytes() {
    double bits = 0.;
    bits += from.get_optimized_bits();
    bits += to.get_optimized_bits();
    for (auto it : src) {
        bits += it.second.get_optimized_bits();
    }
    for (auto it : dst) {
        bits += it.second.get_optimized_bits();
    }
    return bits / 8;
}

void LinkSummary::print(){
    std::cout << "Summary from: " << from.get_counter() << std::endl;
    std::cout << "Summary to: " << to.get_counter() << std::endl;
    for (auto it:src){
        std::cout << "Src: " << inet::IPv4Address(it.first) << " : " << it.second.get_counter();
    }
    std::cout << std::endl;
    for (auto it:dst){
        std::cout << "Dst: " << inet::IPv4Address(it.first) << " : " << it.second.get_counter();
    }
    std::cout << std::endl;
}

void removeNonCoreSketches(SketchHash& sketches,
        std::set<inet::IPv4Address> coreNodes) {
    std::set<int> nonCoreNodes;
    for (auto it : sketches) {
        if (coreNodes.count(inet::IPv4Address(it.first)) == 0) {
            nonCoreNodes.insert(it.first);
        }
    }
    for (auto it : nonCoreNodes) {
        sketches.erase(it);
    }
}
void LinkSummary::optimize(std::set<inet::IPv4Address> coreNodes) {
   removeNonCoreSketches(src, coreNodes);
    removeNonCoreSketches(dst, coreNodes);
}

std::vector<inet::IPv4Address> LinkSummary::getID() {
    std::vector<inet::IPv4Address> id;
    id.push_back(reporter);
    id.push_back(neighbor);
    return id;
}
std::set<inet::IPv4Address> LinkSummary::getSendTo(inet::IPv4Address localIP,
        std::set<inet::IPv4Address> neighbors) {
    if (localIP == reporter) {
        std::set<inet::IPv4Address> result;
        result.insert(neighbor);
        return result;
    }
    // TODO remove reporter from neighbors?
    return neighbors;
}
}
