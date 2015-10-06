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


#ifndef __KDET_ROBUSTFLOODING_H_
#define __KDET_ROBUSTFLOODING_H_

#include <omnetpp.h>
#include <vector>
#include <set>
#include <unordered_map>
#include "Report.h"
#include "ReportAggregation.h"
#include "ReportIdMsg_m.h"
#include "ReportAck_m.h"
#include "IPv4Address.h"
#include "GraphServer.h"
#include <hash.h>
#include <mersenne.h>

/**
 * RobustFlooding floods reliably every report received through gate "in" to
 * every neighbor of the node until the TTL of the report is expired. Received
 * reports are delivered through gate "out".
 */

class RobustFlooding: public cSimpleModule {
protected:
    virtual void initialize(int stage);
    virtual int numInitStages() const { return 4;};
    virtual void finish();
    virtual void handleMessage(cMessage *msg);
    void newReport(Report* report);
    void processReport(ReportAggregation *report);
    void processAck(ReportAck* ack);
    void reliablyFlood(Report *report);
    bool isNew(Report* report);
    void setControlInfo(ReportAggregation *report, IPv4Address dst);
    void sendAck(ReportAggregation* report);
    void scheduleTimeout(int);
    void timeoutExpired(ReportIdMsg* idMsg);
    void sendMulticast(ReportIdMsg* idMsg);
    void sendUnicast(ReportIdMsg* idMsg);
    void queueReport(int reportIndex, int addr);
    void flushQueues();
    void setParameters(Report *report);
    IPv4Address getIP();
    simtime_t jitter();
private:
    std::unordered_map<Report*, int, ReportHash, ReportEqual> reportToIndex;
    std::unordered_map<Report*, long, ReportHash, ReportEqual> localReportVersions;
    std::unordered_map<int, std::vector<int>> reportQueues;
    std::vector<Report*> messages;
    std::vector<ReportIdMsg*> timeouts;
    cMessage* sendTimeout;
    std::vector<std::set<IPv4Address>> sendTo;
    long version = 0;
    unsigned TTL;
    simtime_t timeout;
    GraphServer* graphServer;
    IPv4Address IP;
    cOutVector overhead;
    std::vector<std::string> sendToStr;
};

#endif
