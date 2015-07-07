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
import inet.applications.generic.IIPvXTrafficGenerator;

//
// TODO auto-generated module
//
simple WCNTrafGen like IIPvXTrafficGenerator
{
    parameters:
        double startTime @unit("s") = default(1s); // time of sending the first packet
        int protocol; // value for ~IPv4ControlInfo
        string destAddresses = default(""); // list of destination addresses, separated by spaces
        string filename = default(""); // Name of the file with the messages to send
        @display("i=block/source");
        @signal[sentPk](type=cPacket);
        @signal[rcvdPk](type=cPacket);
    gates:
        input ipIn @labels(IPv4ControlInfo/up);
        input ipv6In @labels(IPv6ControlInfo/up);
        output ipOut @labels(IPv4ControlInfo/down);
        output ipv6Out @labels(IPv6ControlInfo/down);
}

