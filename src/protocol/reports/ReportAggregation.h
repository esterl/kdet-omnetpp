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


#ifndef REPORTAGGREGATION_H_
#define REPORTAGGREGATION_H_

#include "ReportAggregation_m.h"

/**
 * TODO
 */
class ReportAggregation : public ReportAggregation_Base {
public:
    ReportAggregation() : ReportAggregation_Base() {};
    ReportAggregation(const char *name, int kind): ReportAggregation_Base(name,kind) {};
    ReportAggregation(const ReportAggregation& other);
    ReportAggregation& operator=(const ReportAggregation& other);
    virtual ~ReportAggregation();
    virtual ReportAggregation* dup() const;
    virtual const ReportPtr& getReports(unsigned int k) const;
};

Register_Class(ReportAggregation);
#endif /* REPORTAGGREGATION_H_ */
