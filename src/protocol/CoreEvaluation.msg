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