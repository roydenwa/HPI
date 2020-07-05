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

package exercise5.App;

//
// TODO auto-generated module
//
simple App
{
    parameters:
        int nodeID;
        int numNodes;
        bool cutThrough = default(false);
        volatile double sendInterval @unit(s) = default(uniform(1us,10us));
        volatile int pktSize = default(64 * intuniform(1,64));
        
        @signal[sent](type = Packet);
        @signal[rcvd](type = Packet);
        @signal[eed](type = simtime_t);
        @statistic[sentCnt](source=count(sent); record=last,vector?);
        @statistic[rcvdCnt](source=count(rcvd); record=last,vector?);
        @statistic[packetSize](source=packetBits(sent); record=min,max,mean,histogram,stats,vector?);
        @statistic[rcvdBits](source=sumPerDuration(packetBits(rcvd))/1000000; record=last);
        @statistic[endToEndDelay](source=eed; record=min,max,vector,timeavg);
        
        
    gates:
        input in;
        output out;
}
