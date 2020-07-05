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

package exercise5.Buffer;

//
// TODO auto-generated module
//
simple Buffer
{
    parameters:
        int queue_size = default(0);
        bool cutThrough = default(false);
        
        @signal[qlen](type = long);
        @signal[qtime](type = simtime_t);
        @signal[busy](type = bool);
        @signal[drop](type = long);
        @statistic[qlength](source=qlen; record=vector,max,timeavg; interpolationmode=sample-hold);
        @statistic[qutime](source=qtime; record=vector,max,timeavg; interpolationmode=sample-hold);
        @statistic[cBusy](source=timeavg(busy); record=vector,timeavg; interpolationmode=linear);
        @statistic[dropped](source=count(drop); record=last; interpolationmode=sample-hold);
    gates:
        input   in;  //from higher layer
        output out;  //to network layer
}
