/*
 * Report.h
 *
 *  Created on: Jun 17, 2015
 *      Author: ester
 */

#ifndef REPORT_H_
#define REPORT_H_

#include "Report_m.h"
#include "kdet_defs.h"

class Report: public Report_Base {
private:
    void copy(const Report& other);
    LinkSummariesHash summaries_var;
public:
    Report(const char *name = NULL, int kind = KDET_REPORT_MSG) : Report_Base(name, kind){};
    Report(const Report& other);
    virtual ~Report();
    Report& operator=(const Report& other);
    virtual Report *dup() const;
    virtual void setSummaries(const LinkSummariesHash& summaries);
    LinkSummariesHash getSummaries();
    virtual void optimizeSummaries(std::set<IPv4Address> address);
    virtual double getBytes();
};

#endif /* REPORT_H_ */
