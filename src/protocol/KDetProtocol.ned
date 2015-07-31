///***************************************************************************
// *   Copyright (C) 2015 by Ester Lopez                                     *
// *                                                                         *
// *   This program is free software; you can redistribute it and/or modify  *
// *   it under the terms of the GNU General Public License as published by  *
// *   the Free Software Foundation; either version 2 of the License, or     *
// *   (at your option) any later version.                                   *
// *                                                                         *
// *   This program is distributed in the hope that it will be useful,       *
// *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
// *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
// *   GNU General Public License for more details.                          *
// *                                                                         *
// *   You should have received a copy of the GNU General Public License     *
// *   along with this program; if not, write to the                         *
// *   Free Software Foundation, Inc.,                                       *
// *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
// ***************************************************************************/


package kdet.protocol;

import kdet.protocol.detectors.IDetector;
import kdet.protocol.monitors.IMonitor;

module KDetProtocol like IKDet
{
    parameters:
        string monitorType = default("CoreMonitor");
        string detectorType = default("CoreDetector");
        double interval @unit(s) = default(5s);
        double waitTime @unit(s) = default(60s);
        bool faulty = default(false);
        monitor.faulty = faulty;
        detector.faulty = faulty;
    gates:
        output IPout;
        input IPin;
        input graphServerIn;
        output detectorOut;
        input clock[2];
    submodules:
        monitor: <monitorType> like IMonitor {
            @display("p=170,84");
        }
        detector: <detectorType> like IDetector {
            @display("p=51,205");
        }
        flooding: RobustFlooding {
            @display("p=170,205");
        }
    connections allowunconnected:
        monitor.reports --> flooding.in;
        flooding.othersOut --> IPout;
        flooding.othersIn <-- IPin;
        flooding.out --> detector.reportsIn;
        detector.graphServerIn <-- graphServerIn;
        detector.detectionOut --> detectorOut;
        clock[0] --> monitor.clock;
        clock[1] --> detector.clock;
}
