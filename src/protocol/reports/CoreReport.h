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

#ifndef COREREPORT_H_
#define COREREPORT_H_

#include "CoreReport_m.h"

/**
 * Message that contains a core report, that is a single report that already
 * has pre-computed the difference between the traffic sent and destined to
 * a core; as well as between the traffic received and originated from it.
 */
class CoreReport: public CoreReport_Base {
public:
    CoreReport(const char *name = NULL, int kind = KDET_REPORT_MSG) : CoreReport_Base(name, kind){ summary_var = NULL;};
    CoreReport(const CoreReport& other);
    virtual ~CoreReport();
    virtual CoreReport *dup() const;
    virtual bool equals_to(Report* other);
    virtual size_t Hash() const;
    virtual std::set<IPv4Address> sendTo(IPv4Address local,
            std::set<IPv4Address> neighbors);
    virtual std::string getName();
    virtual CoreSketchSummary* getSummary();
    virtual void setSummary(CoreSketchSummary* summary);
protected:
    CoreSketchSummary *summary_var;
    // For the hash function
    static Hash_CW2<uint8_t> hash; // Results will be within 2^13
    static uint32_t prime; //Mersenne prime 17
    static uint64_t seedPows[5]; // Random seed and its powers % p
};

#endif /* COREREPORT_H_ */
