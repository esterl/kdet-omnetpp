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
#ifndef COREDETECTOR_H_
#define COREDETECTOR_H_

#include "Detector.h"
#include <vector>
#include <unordered_map>

namespace kdet{
/*
 * Implementation of the first KDet strategy proposed in "KDet: Coordinated
 * Detection of Forwarding Faults in Wireless Community Networks". Messages
 * exchanged between nodes relate to a single core.
 */
class CoreDetector: public Detector {
protected:
    virtual void initialize(int stage);
    virtual void updateReports(Report* report);
    virtual void updateCores(CoresUpdate* update);
    virtual void evaluateCore(unsigned index, CoreEvaluation* msg);
    virtual void clearReports();
    unsigned findCore(std::set<inet::IPv4Address> core);
    std::vector<std::unordered_map<int, Report*>> reports;
};
}
#endif /* COREDETECTOR_H_ */
