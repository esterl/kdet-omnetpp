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
cplusplus {{
#include "IPv4Address.h"
#include "LinkSummary.h"

typedef std::map<int, bool> BoolMap;
typedef std::vector<IPv4Address> IPList;
}}

class noncobject IPv4Address;
class noncobject BoolMap;
class noncobject IPList;

message CoreEvaluation{
    IPv4Address reporter;
	BoolMap receivedReports;
	IPList core;
	double dropEstimation;
	double inEstimation;
	double outEstimation;
	bool bogus;
}