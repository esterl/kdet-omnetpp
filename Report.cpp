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
    setId(other.getId());
    setTTL(other.getTTL());
    setReporter(other.getReporter());
    setSummaries(other.getSummaries());
}

Report::Report(const Report& other) :
        Report_Base(other) {
    setId(other.getId());
    setTTL(other.getTTL());
    setReporter(other.getReporter());
    setSummaries(other.getSummaries());
}

Report& Report::operator=(const Report& other) {
    if (this == &other)
        return *this;
    Report_Base::operator=(other);
    copy(other);
    return *this;
}

Report* Report::dup() const {
    return new Report(*this);
}

void Report::setSummaries(const LinkSummariesHash& summaries) {
    if (summaries_var == summaries) return;
    // Delete old summaries
    for (auto it = summaries_var.begin(); it != summaries_var.end(); it++) {
        delete it->second;
    }
    summaries_var.clear();
    for (auto it = summaries.begin(); it != summaries.end(); it++) {
        summaries_var[it->first] = it->second->copy();
    }
}
