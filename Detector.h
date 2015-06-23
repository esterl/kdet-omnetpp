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

#ifndef __KDET_DETECTOR_H_
#define __KDET_DETECTOR_H_

#include <omnetpp.h>
#include <utility>
#include <unordered_map>
#include <map>
#include <IPv4Address.h>
#include "Report.h"
#include "CoresUpdate_m.h"

/**
 * TODO - Generated class
 */
class Detector: public cSimpleModule {
protected:
    virtual void initialize();
    virtual void finish();
    virtual void handleMessage(cMessage *msg);
    void updateReports(Report* report);
    virtual void evaluateCores();
    void updateCores(CoresUpdate* update);
    std::pair<std::map<int, bool>, double> evaluateCore(
            std::set<IPv4Address> core, std::set<IPv4Address> boundary);
    void clearReports();
    IPv4Address getIP();
    std::unordered_map<int, Report*> reports;
    IPSetList cores;
    IPSetList boundaries;
    IPv4Address IP;
    cMessage* timeout;
    simtime_t interval;
};

#endif
