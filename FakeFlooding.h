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

#ifndef __KDET_FAKEFLOODING_H_
#define __KDET_FAKEFLOODING_H_

#include <omnetpp.h>
#include <map>
#include <vector>
#include <utility>
#include "IPv4Datagram.h"
#include "IPv4Address.h"
#include "ReportIdMsg_m.h"
#include "Report.h"
#include "ReportAck_m.h"
#include "GraphServer.h"

/**
 * TODO - Generated class
 */
class FakeFlooding : public cSimpleModule
{
protected:
    virtual void initialize();
    virtual void finish();
    virtual void handleMessage(cMessage *msg);
    void reliablyFlood(Report *report);
    void newReport(Report* report);
    void processReport(Report *report);
    bool isNew(Report* report);
    void timeoutExpired(ReportIdMsg* idMsg);
    void sendAck(Report* report);
    void processAck(ReportAck* ack);
    IPv4Address getIP();
    IPv4Datagram* encapsulate(Report* report);
    cPacket* decapsulate(IPv4Datagram* ipPkt);
    void scheduleTimeout(int index,IPv4Address addr);
private:
    std::map<int, int> IPToIndex;
    std::vector<IPv4Datagram*> messages;
    std::vector<ReportIdMsg*> timeouts;
    std::vector<std::set<IPv4Address>> sendTo;
    long id = 0;
    unsigned TTL;
    simtime_t timeout;
    GraphServer* graphServer;
    IPv4Address IP;
};

#endif
