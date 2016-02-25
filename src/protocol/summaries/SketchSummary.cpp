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

#include "SketchSummary.h"

#include <IPv4Serializer.h>
#include <openssl/md5.h>

namespace kdet{
// TODO look the proper length
#define     MAXBUFLENGTH   65536

NetworkSketch* SketchSummary::baseSketch = NULL;

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

uint32_t SketchSummary::getPacketHash(inet::INetworkDatagram* datagram) {
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
            memset((void *)&buf, 0, sizeof(buf));
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
}
