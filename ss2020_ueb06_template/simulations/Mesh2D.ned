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

package ss2020_ueb06.simulations;

import ss2020_ueb06.Node.Node;


network Mesh2D
{
	parameters:
	    int rows = default(4);
	    int columns = default(4);
	    int num_nodes = rows * columns;

	   	double bandwidth @unit(bps) = default(1500Mbps);
        double delay @unit(ns) = default(0);
	    
	types:
	    channel C extends ned.DatarateChannel
        {
            datarate = bandwidth;
            delay = delay;
        }
	    
	submodules:
	    node[num_nodes]: Node{
            NODE_ID = index;
	    	PORTS = 4;
	    	NODE_COUNT = "4 4";
	    	@display("p=69,56,m,4");
	    }
	    
	connections allowunconnected:
	    for i=0..rows-1, for j=0..columns-1 
	    {
            node[i*columns+j].data[3] <--> C <--> node[(i+1)*columns+j].data[2] if i!=rows-1;
            node[i*columns+j].data[1] <--> C <--> node[i*columns+j+1].data[0] if j!=columns-1;
            
            node[i*columns+j].flowCtrl[3] <--> C <--> node[(i+1)*columns+j].flowCtrl[2] if i!=rows-1;
            node[i*columns+j].flowCtrl[1] <--> C <--> node[i*columns+j+1].flowCtrl[0] if j!=columns-1;
        }
	    
}