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

package exercise5.simulations;

import exercise5.Node.Node;
import exercise5.Xbar.Xbar;


network NetXbar
{
    parameters:
        int numPorts;
        int bandwidth @unit(bps) = default(1000Mbps)
        @display("bgb=411,463");
    types:
        channel C extends ned.DatarateChannel
        {
            datarate = bandwidth;
        }
    submodules:
        node[numPorts]: Node {
            @display("p=69,56,c,75");
            nodeID = index;
            numNodes = numPorts;
        }
        xbar: Xbar {
            @display("p=296,162;is=vl");
        }
    connections:
        for i=0..numPorts-1 {
            node[i].line$o --> C --> xbar.in[i];
            node[i].line$i <-- C <-- xbar.out[i];
        }
}
