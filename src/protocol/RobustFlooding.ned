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

package kdet.protocol;

//
// TODO auto-generated module
//
simple RobustFlooding
{
    parameters:
        double interval @unit(s);
        int k = default(1);
        bool multicast = default(true);
    gates:
        input in;
        output out;
        input othersIn;
        output othersOut;
}

