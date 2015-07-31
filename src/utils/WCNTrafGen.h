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


#ifndef __KDET_WCNTRAFGEN_H_
#define __KDET_WCNTRAFGEN_H_

#include <omnetpp.h>
#include <fstream>
#include "IPvXTrafGen.h"
#include "AppMsg_m.h"

/**
 * Generates packets as determined by the file pointed by the parameter
 * "filename" and destined to the node selected by the parameter
 * "destAddresses".
 */
class WCNTrafGen: public cSimpleModule {
public:
    WCNTrafGen(){};
    virtual ~WCNTrafGen();
protected:
    virtual void initialize(int stage);
    virtual void handleMessage(cMessage *msg);
    virtual void newAppMsg(AppMsg* msg);
    virtual void replyAppMsg(AppMsg* msg);
    virtual int numInitStages() const { return 4; }
    int protocol;
    int numSent;
    int numReceived;
    simtime_t startTime;
    static simsignal_t sentPkSignal;
    static simsignal_t rcvdPkSignal;
private:
    bool processFileLine(bool& direction, double& deltaTime, long &bytes);
    std::vector<std::pair<double, long>> readAndScheduleNextIn();
    IPv4Address getIP();
    std::ifstream file;
    std::vector<AppMsg*> pendingMessages;
};

#endif
