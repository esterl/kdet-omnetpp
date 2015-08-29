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

package kdet.protocol.monitors;

moduleinterface IMonitor
{
    gates:
        input clock;
        output reports;
}


simple LinkMonitor like IMonitor
{
    parameters:
        int sketchNumRows = default(32);
        int sketchNumColumns = default(32);
        string sketchRandomGenerator @enum("eh3", "bch3", "bch5", "cw2", "cw4", "tab") = default("cw4");
        string sketchAvgFunc @enum("median", "mean", "trimmean") = default("mean");
        string sketchHashFunction @enum("cw2", "cw4", "tab") = default("cw2");
        string sketchType @enum("AGMS", "FAGMS", "FastCount") = default("FastCount");
        bool faulty = default(false);
        bool randomized = default(false);
    gates:
        input clock;
        output reports;
}


simple CoreMonitor extends LinkMonitor
{
    parameters:
        @class(CoreMonitor);
}