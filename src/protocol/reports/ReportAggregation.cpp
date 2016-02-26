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

#include "ReportAggregation.h"

namespace kdet{
ReportAggregation::ReportAggregation(const ReportAggregation& other) :
        ReportAggregation_Base(other) {
    for (unsigned int i = 0; i < reports_arraysize; i++)
        this->reports_var[i] = other.reports_var[i]->dup();
}

ReportAggregation& ReportAggregation::operator=(
        const ReportAggregation& other) {
    if (this == &other)
        return *this;
    //Delete Reports:
    for (unsigned int i = 0; i < reports_arraysize; i++)
        delete this->reports_var[i];
    // Call Base copy
    ReportAggregation_Base::operator=(other);
    // Dup reports:
    for (unsigned int i = 0; i < reports_arraysize; i++)
        this->reports_var[i] = other.reports_var[i]->dup();
    return *this;
}

ReportAggregation::~ReportAggregation() {
    //Delete Reports:
    for (unsigned int i = 0; i < reports_arraysize; i++)
        delete this->reports_var[i];
}

ReportAggregation* ReportAggregation::dup() const {
    return new ReportAggregation(*this);
}

const ReportPtr& ReportAggregation::getReports(unsigned int k) const {
    if (k>=reports_arraysize) throw cRuntimeError("Array of size %d indexed by %d", reports_arraysize, k);
      return reports_var[k];
}
}
