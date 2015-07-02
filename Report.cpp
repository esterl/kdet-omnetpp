/*
 * Report.cpp
 *
 *  Created on: Jun 17, 2015
 *      Author: ester
 */

#include <Report.h>

Register_Class(Report);

void Report::copy(const Report& other) {
    if (this == &other)
        return;
    // Delete summaries
    for (auto it = summaries_var.begin(); it != summaries_var.end(); it++) {
        delete it->second;
    }
    summaries_var.clear();
    setId(other.getId());
    setTTL(other.getTTL());
    setReporter(other.getReporter());
    setSummaries(other.summaries_var);
}

Report::Report(const Report& other):Report_Base(other){
    setSummaries(other.summaries_var);
}

Report::~Report() {
    // Delete summaries
    for (auto it = summaries_var.begin(); it != summaries_var.end(); it++) {
        delete it->second;
    }
    summaries_var.clear();
}

Report& Report::operator=(const Report& other) {
    if (this == &other)
        return *this;
    cPacket::operator=(other);
    copy(other);
    return *this;
}

Report* Report::dup() const {
    return new Report(*this);
}

void Report::setSummaries(const LinkSummariesHash& summaries) {
    if (&summaries == &summaries_var)
        return;
    // Delete old summaries
    for (auto it = summaries_var.begin(); it != summaries_var.end(); it++) {
        delete it->second;
    }
    summaries_var.clear();
    for (auto it = summaries.begin(); it != summaries.end(); it++) {
        summaries_var[it->first] = it->second->copy();
    }
}

LinkSummariesHash Report::getSummaries(){
    return summaries_var;
}

void Report::optimizeSummaries(std::set<IPv4Address> coreNodes){
    for (auto it = summaries_var.begin(); it != summaries_var.end(); it++){
        it->second->optimizeSummary(coreNodes);
    }
}

double Report::getBytes() {
    double bytes = 0;
    for (auto it = summaries_var.begin(); it != summaries_var.end(); it++) {
        bytes += it->second->getBytes();
    }
    return bytes;
}
