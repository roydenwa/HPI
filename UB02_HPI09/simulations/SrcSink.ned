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

package ub02_hpi09.simulations;

import ub02_hpi09.App;


network SrcSink
{
    parameters:
        int num_nodes = default(3);

    submodules:
        node[num_nodes]: App;
        
	connections:
	    // circle:
	    for i=0..num_nodes-2
	    {
	        node[i].out --> node[i+1].in; 
	    }
	    
	    node[num_nodes-1].out --> node[0].in; 
        
}
