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

#include "LinkSketchSummary.h"

LinkSketchSummary::LinkSketchSummary(IPv4Address reporter,
        IPv4Address neighborIP) {
    reporter = reporter;
    neighbor = neighborIP;
    from = SketchSummary::getBaseSketch();
    to = SketchSummary::getBaseSketch();
}

LinkSketchSummary::LinkSketchSummary(const LinkSketchSummary& other) {
    reporter = other.reporter;
    neighbor = other.neighbor;
    from = other.from->copy();
    to = other.to->copy();
    for (auto it = other.src.begin(); it != other.src.end(); it++) {
        src[it->first] = it->second->copy();
    }
    for (auto it = other.dst.begin(); it != other.dst.end(); it++) {
        dst[it->first] = it->second->copy();
    }
}

LinkSketchSummary& LinkSketchSummary::operator=(
        const LinkSketchSummary& other) {
    reporter = other.reporter;
    neighbor = other.neighbor;
    NetworkSketch* tmp = other.from->copy();
    delete from;
    from = tmp;
    tmp = other.to->copy();
    delete to;
    to = tmp;
    SketchHash tmpHash;
    for (auto it = other.src.begin(); it != other.src.end(); it++) {
        tmpHash[it->first] = it->second->copy();
    }
    for (auto it = src.begin(); it != src.end(); it++) {
        delete it->second;
    }
    src = tmpHash;
    for (auto it = other.dst.begin(); it != other.dst.end(); it++) {
        tmpHash[it->first] = it->second->copy();
    }
    for (auto it = dst.begin(); it != dst.end(); it++) {
        delete it->second;
    }
    dst = tmpHash;
    return *this;
}

LinkSketchSummary::~LinkSketchSummary() {
    delete from;
    delete to;
    for (auto it = src.begin(); it != src.end(); it++) {
        delete it->second;
    }
    src.clear();
    for (auto it = dst.begin(); it != dst.end(); it++) {
        delete it->second;
    }
    dst.clear();
}

void LinkSketchSummary::updateSummaryPreRouting(IPv4Datagram* pkt) {
    // Update from and src sketches
    try {
        uint32_t pktHash = getPacketHash(pkt);
        from->update(pktHash, 1);
        // Create Sketch if it doesn't exist
        IPv4Address address = pkt->getSrcAddress();
        if (src.count(address.getInt()) == 0) {
            src[address.getInt()] = SketchSummary::getBaseSketch();
        }
        src[address.getInt()]->update(pktHash, 1);
    } catch (cRuntimeError e) {
        // Is not a Serializable packet
        std::cout << "No serializable" << endl;
    }
}

void LinkSketchSummary::updateSummaryPostRouting(IPv4Datagram* pkt) {
    // Update from and src sketches
    try {
        uint32_t pktHash = getPacketHash(pkt);
        to->update(pktHash, 1);
        // Create Sketch if it doesn't exist
        IPv4Address address = pkt->getDestAddress();
        if (dst.count(address.getInt()) == 0) {
            dst[address.getInt()] = SketchSummary::getBaseSketch();
        }
        dst[address.getInt()]->update(pktHash, 1);
    } catch (cRuntimeError e) {
        // Is not a Serializable packet
        std::cout << "No serializable" << endl;
    }
}

void LinkSketchSummary::clear() {
    to->clear();
    from->clear();
    for (auto it = src.begin(); it != src.end(); it++) {
        it->second->clear();
    }
    for (auto it = dst.begin(); it != dst.end(); it++) {
        it->second->clear();
    }
}

Summary* LinkSketchSummary::copy() const {
    LinkSketchSummary* summary = new LinkSketchSummary(reporter, neighbor);
    delete summary->from;
    summary->from = from->copy();
    delete summary->to;
    summary->to = to->copy();
    for (auto it = src.begin(); it != src.end(); it++) {
        summary->src[it->first] = it->second->copy();
    }
    for (auto it = dst.begin(); it != dst.end(); it++) {
        summary->dst[it->first] = it->second->copy();
    }
    return summary;
}

void merge(SketchHash& hash1, const SketchHash& hash2) {
    for (auto it = hash2.begin(); it != hash2.end(); it++) {
        if (hash1.count(it->first) == 0) {
            hash1[it->first] = it->second->copy();
        } else {
            (*hash1[it->first]) += *(it->second);
        }
    }
}

void LinkSketchSummary::add(Summary* otherPtr) {
    LinkSketchSummary* other = dynamic_cast<LinkSketchSummary*>(otherPtr);
    if (other != NULL) {
        (*from) += *(other->from);
        (*to) += *(other->to);
        merge(src, other->src);
        merge(dst, other->dst);
    }
}

double LinkSketchSummary::estimateDrop(std::set<IPv4Address> core) {
    NetworkSketch* sketchIn = to->copy();
    for (auto it = dst.begin(); it != dst.end(); it++) {
        if (core.count(IPv4Address(it->first)) != 0) {
            (*sketchIn) -= (*it->second);
        }
    }
    NetworkSketch* sketchOut = from->copy();
    for (auto it = src.begin(); it != src.end(); it++) {
        if (core.count(IPv4Address(it->first)) != 0) {
            (*sketchOut) -= (*it->second);
        }
    }
    (*sketchIn) -= (*sketchOut);
    double dropped = sketchIn->second_moment();
    delete sketchIn;
    delete sketchOut;
    return dropped;
}

double LinkSketchSummary::estimateIn(std::set<IPv4Address> core) {
    NetworkSketch* sketchIn = to->copy();
    for (auto it = dst.begin(); it != dst.end(); it++) {
        if (core.count(IPv4Address(it->first)) != 0) {
            (*sketchIn) -= (*it->second);
        }
    }
    double received = sketchIn->second_moment();
    delete sketchIn;
    return received;
}

double LinkSketchSummary::estimateOut(std::set<IPv4Address> core) {
    NetworkSketch* sketchOut = from->copy();
    for (auto it = src.begin(); it != src.end(); it++) {
        if (core.count(IPv4Address(it->first)) != 0) {
            (*sketchOut) -= (*it->second);
        }
    }
    double sent = sketchOut->second_moment();
    delete sketchOut;
    return sent;
}

double LinkSketchSummary::getBytes() {
// Sum the bytes required to store each Sketch
    double bytes = 0.;
    bytes += from->get_bytes();
    bytes += to->get_bytes();
    for (auto it = src.begin(); it != src.end(); it++) {
        bytes += (*it->second).get_bytes();
    }
    for (auto it = dst.begin(); it != dst.end(); it++) {
        bytes += (*it->second).get_bytes();
    }
    return bytes;
}

void removeNonCoreSketches(SketchHash& sketches,
        std::set<IPv4Address> coreNodes) {
    std::set<int> nonCoreNodes;
    for (auto it = sketches.begin(); it != sketches.end(); it++) {
        if (coreNodes.count(IPv4Address(it->first)) == 0) {
            nonCoreNodes.insert(it->first);
        }
    }
    for (auto it = nonCoreNodes.begin(); it != nonCoreNodes.end(); it++) {
        delete sketches[*it];
        sketches.erase(*it);
    }
}
void LinkSketchSummary::optimizeSummary(std::set<IPv4Address> coreNodes) {
    removeNonCoreSketches(src, coreNodes);
    removeNonCoreSketches(dst, coreNodes);
}
