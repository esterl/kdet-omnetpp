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


#include "WCNTrafGen.h"
#include <string>
#include "IPSocket.h"
#include "IPv4ControlInfo.h"
#include "IPvXAddressResolver.h"
#include <algorithm>

#define IN true
#define OUT false

Define_Module(WCNTrafGen);
simsignal_t WCNTrafGen::rcvdPkSignal = registerSignal("rcvdPk");
simsignal_t WCNTrafGen::sentPkSignal = registerSignal("sentPk");

bool WCNTrafGen::processFileLine(bool& direction, double& deltaTime,
        long &bytes) {
    if (file.is_open() & file.good()) {
        std::string line, field;
        std::getline(file, line);
        if (!line.empty()) {
            std::istringstream buffer(line);
            // Direction
            std::getline(buffer, field, ',');
            direction = std::stoi(field);
            // DeltaTime
            std::getline(buffer, field, ',');
            deltaTime = std::stod(field);
            // Bytes
            std::getline(buffer, field, ',');
            bytes = std::stol(field);
            if (bytes > 2280)
                bytes = 2280;
            return true;
        }
    }
    return false;
}

std::vector<std::pair<double, long>> WCNTrafGen::readAndScheduleNextIn() {
    std::vector<std::pair<double, long>> result;
    if (file.is_open()) {
        while (file.good()) {
            bool direction;
            double deltaTime;
            long bytes;
            if (processFileLine(direction, deltaTime, bytes)) {
                // Look for the first "IN" message:
                simtime_t time = deltaTime + startTime;
                if (direction == IN) {
                    AppMsg* msg = new AppMsg();
                    msg->setByteLength(bytes);
                    msg->setIPAddress(getIP());
                    if (time < simTime()) {
                        scheduleAt(simTime(), msg);
                    } else {
                        scheduleAt(time, msg);
                    }
                    pendingMessages.push_back(msg);
                    break;
                } else {
                    result.push_back(std::make_pair(deltaTime, bytes));
                }
            }
        }
    }
    return result;
}

void WCNTrafGen::initialize(int stage) {
    cSimpleModule::initialize(stage);

    if (stage == 0) {
        protocol = par("protocol");
        startTime = par("startTime");
        numSent = 0;
        numReceived = 0;
        WATCH(numSent);
        WATCH(numReceived);
    } else if (stage == 3) {
        IPSocket ipSocket(gate("ipOut"));
        ipSocket.registerProtocol(protocol);
        if (par("filename") != "") {
            file.open(par("filename").stringValue());
            // Schedule first message
            readAndScheduleNextIn();
        }

    }
}

WCNTrafGen::~WCNTrafGen() {
    // Delete pending messages
    for (auto it = pendingMessages.begin(); it != pendingMessages.end(); it++) {
        cancelAndDelete(*it);
    }
    pendingMessages.clear();
    // Close file
    if (file.is_open())
        file.close();
}

void WCNTrafGen::handleMessage(cMessage *msg) {
    if (dynamic_cast<AppMsg*>(msg)) {
        if (msg->isSelfMessage()) {
            newAppMsg(check_and_cast<AppMsg*>(msg));
        } else {
            replyAppMsg(check_and_cast<AppMsg*>(msg));
        }
    }
}

void WCNTrafGen::newAppMsg(AppMsg* msg) {
    char msgName[32];
    sprintf(msgName, "appData-%d", numSent);
    msg->setName(msgName);

    // Set required responses:
    msg->setResponses(readAndScheduleNextIn());

    // Create IPv4 Packet & send
    IPv4ControlInfo *controlInfo = new IPv4ControlInfo();
    controlInfo->setDestAddr(msg->getIPAddress());
    controlInfo->setProtocol(protocol);
    msg->setControlInfo(controlInfo);
    EV << "Sending packet: ";
    emit(sentPkSignal, msg);
    send(msg, "ipOut");
    numSent++;

    // Delete msg from pendingMessages:
    pendingMessages.erase(
            std::remove(pendingMessages.begin(), pendingMessages.end(), msg),
            pendingMessages.end());
}

void WCNTrafGen::replyAppMsg(AppMsg* msg) {
    emit(rcvdPkSignal, msg);
    numReceived++;
    // Schedule a message for each required response
    std::vector<std::pair<double, long>> responses = msg->getResponses();
    simtime_t schedTime;
    IPv4ControlInfo *ctrl =
            dynamic_cast<IPv4ControlInfo *>(msg->getControlInfo());
    IPv4Address ip = ctrl->getSrcAddr();
    for (auto it = responses.begin(); it != responses.end(); it++) {
        AppMsg* newMsg = new AppMsg();
        schedTime = it->first + startTime;
        newMsg->setByteLength(it->second);
        newMsg->setIPAddress(ip);
        pendingMessages.push_back(newMsg);
        if (schedTime < simTime()) {
            scheduleAt(simTime(), newMsg);
        } else {
            scheduleAt(schedTime, newMsg);
        }
    }
    delete msg;
}

IPv4Address WCNTrafGen::getIP() {
    // Look up destination
    IPvXAddress result;
    IPvXAddressResolver().tryResolve(par("destAddresses"), result);
    return result.get4();
}
