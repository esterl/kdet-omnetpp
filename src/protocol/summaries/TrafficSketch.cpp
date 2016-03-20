/*
 * TrafficSketch.cpp
 *
 *  Created on: Mar 20, 2016
 *      Author: ester
 */

#include <TrafficSketch.h>
#include <IPv4Serializer.h>
#include <openssl/md5.h>

namespace kdet {

#define     MAXBUFLENGTH   65536
Sketch<uint32_t>* TrafficSketch::baseSketch = NULL;

TrafficSketch::TrafficSketch() {
    counter = 0;
    sketch = baseSketch->copy();
}

TrafficSketch::TrafficSketch(const TrafficSketch& other) {
    counter = other.counter;
    sketch = other.sketch->copy();
}

TrafficSketch& TrafficSketch::operator=(const TrafficSketch& other) {
    counter = other.counter;
    delete sketch;
    sketch = other.sketch->copy();
    return (*this);
}

TrafficSketch::~TrafficSketch() {
    delete sketch;
}

void TrafficSketch::setBaseSketch(cModule* module) {
    if (baseSketch == NULL) {
        unsigned rows = module->par("sketchNumRows");
        unsigned cols = module->par("sketchNumColumns");
        const char* randGenerator =
                module->par("sketchRandomGenerator").stringValue();
        const char* avgFunc = module->par("sketchAvgFunc").stringValue();
        const char* hashFunc = module->par("sketchHashFunction").stringValue();
        const char* sketchType = module->par("sketchType").stringValue();

        if (strcmp(sketchType, "AGMS") == 0) {
            baseSketch = new AGMS_Sketch<uint32_t>(cols, rows, randGenerator,
                    avgFunc);
        } else if (strcmp(sketchType, "FAGMS") == 0) {
            baseSketch = new FAGMS_Sketch<uint32_t>(cols, rows, randGenerator,
                    hashFunc, avgFunc);
        } else if (strcmp(sketchType, "FastCount") == 0) {
            baseSketch = new FastCount_Sketch<uint32_t>(cols, rows,
                    randGenerator);
        } else {
            throw cRuntimeError(module, "Invalid Sketch Type parameter");
        }
    }
}

void TrafficSketch::setBaseSketch(Sketch<uint32_t>* sketch) {
    if (baseSketch == NULL) {
        baseSketch = sketch;
    } else {
        delete sketch;
    }
}

void TrafficSketch::clearBaseSketch() {
    delete baseSketch;
    baseSketch = NULL;
}

void TrafficSketch::update(inet::INetworkDatagram* pkt) {
    ++counter;
    uint32_t pktHash = getPacketHash(pkt);
    sketch->update(pktHash, 1);
}

double TrafficSketch::second_moment() {
    return sketch->second_moment();
}

int TrafficSketch::get_counter() {
    return counter;
}

uint32_t low_md5(unsigned char* hash) {
    uint32_t result = (uint32_t(hash[3]) << 24) + (uint32_t(hash[2]) << 16)
            + (uint32_t(hash[1]) << 8) + uint32_t(hash[0]);
    return result;
}

uint32_t TrafficSketch::getPacketHash(inet::INetworkDatagram* datagram) {
    // Copy packet without TTL, ToS & Checksum (may change)
    if (dynamic_cast<inet::IPv4Datagram*>(datagram)) {
        inet::IPv4Datagram* pkt = static_cast<inet::IPv4Datagram*>(datagram);
        short int oldTTL = pkt->getTimeToLive();
        unsigned char oldTOS = pkt->getTypeOfService();
        pkt->setTimeToLive(0);
        pkt->setTypeOfService(0);
        uint32_t low_hash = 0;
        try {
            uint8 buf[MAXBUFLENGTH];
            memset((void *) &buf, 0, sizeof(buf));
            inet::serializer::Buffer b(buf, sizeof(buf));
            inet::serializer::Context c;
            c.throwOnSerializerNotFound = false;
            inet::serializer::IPv4Serializer().serializePacket(pkt, b, c);
            unsigned packet_length = b.getPos();
            // Compute MD5
            unsigned char tmp_hash[MD5_DIGEST_LENGTH];
            MD5(buf, packet_length, tmp_hash);
            // Strip to the size of the sketch:
            low_hash = low_md5(tmp_hash);
        } catch (cRuntimeError e) {
            pkt->setTimeToLive(oldTTL);
            pkt->setTypeOfService(oldTOS);
            throw e;
        }
        // Revert changes
        pkt->setTimeToLive(oldTTL);
        pkt->setTypeOfService(oldTOS);
        return low_hash;
    } else {
        return 0;
    }
}

double TrafficSketch::get_bytes() {
    return sketch->get_bytes();
}

double TrafficSketch::get_optimized_bits() {
    return sketch->get_optimized_bits();
}

void TrafficSketch::clear() {
    counter = 0;
    sketch->clear();
}

TrafficSketch& TrafficSketch::operator+=(const TrafficSketch& other) {
    counter += other.counter;
    (*sketch) += *(other.sketch);
    return (*this);
}

TrafficSketch& TrafficSketch::operator-=(const TrafficSketch& other) {
    counter -= other.counter;
    (*sketch) -= *(other.sketch);
    return (*this);
}

} /* namespace kdet */
