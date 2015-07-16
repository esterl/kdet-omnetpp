//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public License
// along with this program.  If not, see http://www.gnu.org/licenses/.
// 

package kdet;

module KDetProtocol
{
    parameters:
        string monitorType = default("SketchMonitor");
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
        detector: Detector {
            @display("p=51,205");
        }
        flooding: RobustFlooding {
        //flooding: FakeFlooding {
            @display("p=170,205");
        }
    connections allowunconnected:
        monitor.reports --> flooding.in;
        //flooding.IPin <-- IPin;
        //flooding.IPout --> IPout;
        // Fake flooding
        flooding.othersOut --> IPout;
        flooding.othersIn <-- IPin;
        flooding.out --> detector.reportsIn;
        detector.graphServerIn <-- graphServerIn;
        detector.detectionOut --> detectorOut;
        clock[0] --> monitor.clock;
        clock[1] --> detector.clock;
}
