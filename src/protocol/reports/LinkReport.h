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


#ifndef LINKREPORT_H_
#define LINKREPORT_H_

#include "LinkReport_m.h"
#include "kdet_defs.h"
#include <unordered_map>
#include "LinkSummary.h"

typedef std::unordered_map<int, LinkSummary*> LinkSummariesHash;

/**
 * Message that contains a list of all the summaries from a node, one for each
 * link with its neighbors, without any post-processing.
 */
class LinkReport : public LinkReport_Base{
private:
    void copy(const LinkReport& other);
    LinkSummariesHash summaries_var;
public:
    LinkReport(const char *name = NULL, int kind = KDET_REPORT_MSG) : LinkReport_Base(name, kind){};
    LinkReport(const LinkReport& other);
    virtual ~LinkReport();
    LinkReport& operator=(const LinkReport& other);
    virtual LinkReport *dup() const;
    virtual void setSummaries(const LinkSummariesHash& summaries);
    LinkSummariesHash getSummaries();
    virtual void updateBytes();
    virtual bool equals_to(Report* other);
    virtual size_t Hash() const;
    virtual std::set<IPv4Address> sendTo(IPv4Address local, std::set<IPv4Address> neighbors);
    virtual std::string getName();
    virtual void optimizeSummaries(std::set<IPv4Address> kNeighbors);
};

#endif /* LINKREPORT_H_ */
