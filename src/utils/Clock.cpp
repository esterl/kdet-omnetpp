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


#include "Clock.h"

namespace kdet{
Define_Module(Clock);

void Clock::initialize()
{
    interval = par("interval");
    timer = new cMessage();
    scheduleAt(par("waitTime"), timer);
}

void Clock::finish(){
    cancelAndDelete(timer);
    recordScalar("Interval", interval);
    recordScalar("StartTime", par("waitTime").doubleValue());
    recordScalar("EndTime", simTime());
}
void Clock::handleMessage(cMessage *msg)
{
    for (int i = 0; i < gateSize("out"); i++)
        send(msg->dup(), "out",i);
    scheduleAt(simTime()+ interval, msg);
}
}
