/*   Copyright (C) 2015 by Ester Lopez                                     *
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

#ifndef __KDET_DETECTOR_H_
#define __KDET_DETECTOR_H_

#include <random>
#include <omnetpp.h>
#include <L3Address.h>
#include "Report.h"
#include "CoresUpdate_m.h"
#include "CoreEvaluation_m.h"
#include "TrafficMonitor.h"

namespace kdet{
/**
 * Base class that defines the behavior a Detector should have. Implements the
 * logic of randomized interval evaluation based on the core's last
 * performance.
 */
class Detector: public cSimpleModule {
protected:
    virtual void initialize(int stage);
    virtual int numInitStages() const { return inet::NUM_INIT_STAGES; };
    virtual void finish();
    virtual void handleMessage(cMessage *msg);
    virtual void updateReports(Report* report) = 0;
    virtual void evaluateCores();
    virtual void updateCores(CoresUpdate* update);
    virtual void evaluateCore(unsigned index, CoreEvaluation* msg) = 0;
    virtual void clearReports() = 0;
    virtual unsigned updateThreshold(unsigned previous, CoreEvaluation* msg);
    virtual bool getEvaluateNextIter(unsigned i);
    TrafficMonitor* monitor;
    IPSetList cores;
    IPSetList boundaries;
    std::vector<unsigned> thresholds;
    std::vector<std::mt19937> randGenerators;
    std::vector<bool> evaluateNextIter;
    inet::IPv4Address IP;
    bool faulty;
    unsigned min_threshold;
};
}
#endif
