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

#ifndef __KDET_ROBUSTFLOODING_H_
#define __KDET_ROBUSTFLOODING_H_

#include <omnetpp.h>
#include <vector>
#include <set>
#include "Report.h"
#include "ReportIdMsg_m.h"
#include "ReportAck_m.h"
#include "IPv4Address.h"
#include "GraphServer.h"

/**
 * TODO - Generated class
 */
class RobustFlooding : public cSimpleModule
{
  protected:
    virtual void initialize();
    virtual void finish();
    virtual void handleMessage(cMessage *msg);
    void newReport(Report* report);
    void processReport(Report *report);
    void processAck(ReportAck* ack);
    void reliablyFlood(Report *report);
    bool isNew(Report* report);
    void setControlInfo(Report *report, IPv4Address dst);
    void sendAck(Report* report);
    void scheduleTimeout(int index,IPv4Address addr);
    void timeoutExpired(ReportIdMsg* idMsg);
    void sendMulticast(ReportIdMsg* idMsg);
    void sendUnicast(ReportIdMsg* idMsg);
    IPv4Address getIP();
    simtime_t jitter();
  private:
      std::map<int, int> IPToIndex;
      std::vector<Report*> messages;
      std::vector<ReportIdMsg*> timeouts;
      std::vector<std::set<IPv4Address>> sendTo;
      long id = 0;
      unsigned TTL;
      simtime_t timeout;
      GraphServer* graphServer;
      IPv4Address IP;
      cOutVector overhead;
      std::vector<std::string> sendToStr;
};

#endif
