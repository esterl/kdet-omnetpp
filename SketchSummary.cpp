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

#include <SketchSummary.h>
#include <IPv4Serializer.h>
#include <openssl/md5.h>

// TODO look the proper length
#define     MAXBUFLENGTH   65536

NetworkSketch* SketchSummary::baseSketch = NULL;

SketchSummary::SketchSummary(IPv4Address hostIP, IPv4Address neighborIP) {
    host = hostIP;
    neighbor = neighborIP;
    from = SketchSummary::getBaseSketch();
    to = SketchSummary::getBaseSketch();
}

SketchSummary::~SketchSummary() {
    delete from;
    delete to;
    for (auto it = src.begin(); it != src.end(); it++) {
        delete it->second;
    }
    for (auto it = dst.begin(); it != dst.end(); it++) {
        delete it->second;
    }
}

void SketchSummary::updateSummaryPreRouting(IPv4Datagram* pkt) {
    // Update from and src sketches
    updateSketch(from, pkt);
    // Create Sketch if it doesn't exist
    IPv4Address address = pkt->getSrcAddress();
    if (src.count(address.getInt()) == 0) {
        src[address.getInt()] = SketchSummary::getBaseSketch();
    }
    updateSketch(src[address.getInt()], pkt);
}

void SketchSummary::updateSummaryPostRouting(IPv4Datagram* pkt) {
    // Update from and src sketches
    updateSketch(to, pkt);
    // Create Sketch if it doesn't exist
    IPv4Address address = pkt->getDestAddress();
    if (dst.count(address.getInt()) == 0) {
        dst[address.getInt()] = SketchSummary::getBaseSketch();
    }
    updateSketch(dst[address.getInt()], pkt);
}

void SketchSummary::clear() {
    to->clear();
    from->clear();
    for (auto it = src.begin(); it != src.end(); it++) {
        it->second->clear();
    }
    for (auto it = dst.begin(); it != dst.end(); it++) {
        it->second->clear();
    }
}

LinkSummary* SketchSummary::copy() const{
    SketchSummary* summary = new SketchSummary(host, neighbor);
    summary->from = from->copy();
    summary->to = to->copy();
    for (auto it = src.begin(); it != src.end(); it++) {
        summary->src[it->first] = it->second->copy();
    }
    for (auto it = dst.begin(); it != dst.end(); it++) {
        summary->dst[it->first] = it->second->copy();
    }
    return summary;
}

void merge(const SketchHash& hash1, const SketchHash& hash2,
        SketchHash& result) {
    for (auto it = hash1.begin(); it != hash1.end(); it++) {
        result[it->first] = it->second->copy();
    }
    for (auto it = hash2.begin(); it != hash2.end(); it++) {
        if (result.count(it->first) == 0) {
            result[it->first] = it->second->copy();
        } else {
            (*result[it->first]) += *(it->second);
        }
    }
}

LinkSummary* SketchSummary::operator+(LinkSummary* otherPtr) const {
    SketchSummary* other = dynamic_cast<SketchSummary*>(otherPtr);
    SketchSummary* result = new SketchSummary();
    // Sum from and to Sketches
    result->from = from->copy();
    *(result->from) += *(other->from);
    result->to = to->copy();
    *(result->to) += *(other->to);
    merge(src, other->src, result->src);
    merge(dst, other->dst, result->dst);
    return result;
}

double SketchSummary::estimateDrop(std::set<IPv4Address> core) {

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
    double sent = sketchIn->second_moment();
    (*sketchIn) -= (*sketchOut);
    double dropped = sketchIn->second_moment();
    delete sketchIn;
    delete sketchOut;
    if (sent == 0)
        return 0.0;
    return dropped / sent;
}

void SketchSummary::setBaseSketch(cModule* module) {
    if (baseSketch == NULL) {
        unsigned rows = module->par("sketchNumRows");
        unsigned cols = module->par("sketchNumColumns");
        const char* randGenerator =
                module->par("sketchRandomGenerator").stringValue();
        const char* avgFunc = module->par("sketchAvgFunc").stringValue();
        const char* hashFunc = module->par("sketchHashFunction").stringValue();
        const char* sketchType = module->par("sketchType").stringValue();

        if (strcmp(sketchType, "AGMS") == 0) {
            baseSketch = new AGMS_Sketch<uint32_t>(rows, cols, randGenerator,
                    avgFunc);
        } else if (strcmp(sketchType, "FAGMS") == 0) {
            baseSketch = new FAGMS_Sketch<uint32_t>(rows, cols, randGenerator,
                    hashFunc, avgFunc);
        } else if (strcmp(sketchType, "FastCount") == 0) {
            baseSketch = new FastCount_Sketch<uint32_t>(rows, cols,
                    randGenerator);
        } else {
            throw cRuntimeError(module, "Invalid Sketch Type parameter");
        }
    }
}

void SketchSummary::setBaseSketch(NetworkSketch* sketch) {
    if (baseSketch == NULL) {
        baseSketch = sketch;
    } else {
        delete sketch;
    }
}

NetworkSketch* SketchSummary::getBaseSketch() {
    if (baseSketch != NULL) {
        return baseSketch->copy();
    } else {
        return NULL;
    }
}

uint32_t low_md5(unsigned char* hash) {
    uint32_t result = (uint32_t(hash[3]) << 24) + (uint32_t(hash[2]) << 16)
            + (uint32_t(hash[1]) << 8) + uint32_t(hash[0]);
    return result;
}

void SketchSummary::updateSketch(NetworkSketch* sketch, IPv4Datagram* pkt) {
    // Copy packet without TTL, ToS & Checksum (may change)
    short int oldTTL = pkt->getTimeToLive();
    unsigned char oldTOS = pkt->getTypeOfService();
    pkt->setTimeToLive(0);
    pkt->setTypeOfService(0);
    unsigned char buf[MAXBUFLENGTH];
    try {
        unsigned packet_length = IPv4Serializer().serialize(pkt, buf,
                sizeof(buf));

        // Compute MD5
        unsigned char tmp_hash[MD5_DIGEST_LENGTH];
        MD5(buf, packet_length, tmp_hash);
        // Strip to the size of the sketch:
        uint32_t low_hash = low_md5(tmp_hash);
        // Update sketch
        sketch->update(low_hash, 1.);
    } catch (cRuntimeError e) {
        // Is not a Serializable packet
        std::cout << "No serializable" << endl;
    }
    // Revert changes
    pkt->setTimeToLive(oldTTL);
    pkt->setTypeOfService(oldTOS);
}

