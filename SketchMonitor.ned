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
simple SketchMonitor like IMonitor
{
    parameters:
        int sketchNumRows = default(32);
        int sketchNumColumns = default(32);
        string sketchRandomGenerator @enum("eh3", "bch3", "bch5", "cw2", "cw4", "tab") = default("cw4");
        string sketchAvgFunc @enum("median", "mean", "trimmean") = default("mean");
        string sketchHashFunction @enum("cw2", "cw4", "tab") = default("cw2");
        string sketchType @enum("AGMS", "FAGMS", "FastCount") = default("FastCount");
    gates:
        output reports;
}
