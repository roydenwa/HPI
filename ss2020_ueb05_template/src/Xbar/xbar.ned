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

package exercise5.Xbar;

import exercise5.Outport.Outport;
import exercise5.Inport.Inport;

module Xbar
{
    parameters:
        int numPorts;
        int bandwidth @unit(bps) = default(1000Mbps)
        @display("bgb=417.41,616.77");

    gates:
        input in[numPorts];
        output out[numPorts];
    types:
        channel C extends ned.DatarateChannel
        {
            datarate = bandwidth;
        }


    submodules:
        inports[numPorts]: Inport {
            @display("p=49.84,62.300003,c,75");
        }
        outports[numPorts]: Outport {
            @display("p=373.80002,62.300003,c,75");
        }


    connections allowunconnected:
        for i=0..numPorts-1, for j=0..numPorts-1 {
			inports[i].arbiterCtrl[j] <--> outports[j].arbiterCtrl[i];
            inports[i].outp[j] --> C --> outports[j].inp[i];
        }
        for i=0..numPorts-1 {
            in[i] --> inports[i].in;
            out[i] <-- outports[i].out;
        }


}
