/***************************************************************************
 *   Copyright (C) 2016 by Ester Lopez                                     *
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

#ifndef TRAFFICSKETCH_H_
#define TRAFFICSKETCH_H_

#include <sketches.h>
#include <cmodule.h>
#include <INetworkDatagram.h>
namespace kdet {

/**
 * Encapsulates the Sketch class to use for Traffic Validation: update uses
 * a packet and it keeps an additional counter with the number of packets to
 * provide better estimations.
 */
class TrafficSketch {
public:
    TrafficSketch();
    TrafficSketch(const TrafficSketch& other);
    TrafficSketch& operator=(const TrafficSketch& other);
    virtual ~TrafficSketch();
    virtual void update(inet::INetworkDatagram* pkt);
    virtual double second_moment();
    virtual int get_counter();
    virtual double get_bytes();
    virtual double get_optimized_bits();
    virtual void clear();
    TrafficSketch& operator+=(const TrafficSketch& other);
    TrafficSketch& operator-=(const TrafficSketch& other);
    static void setBaseSketch(cModule* module);
    static void setBaseSketch(Sketch<uint32_t>* sketch);
    static void clearBaseSketch();
protected:
    uint32_t getPacketHash(inet::INetworkDatagram* pkt);
    static Sketch<uint32_t>* baseSketch;
    Sketch<uint32_t>* sketch;
    int counter;
};

} /* namespace kdet */

#endif /* TRAFFICSKETCH_H_ */
