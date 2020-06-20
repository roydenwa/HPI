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

package ub04_hpi09;

import ub04_hpi09.Buffer;


simple Inport
{
    gates:
        input in;
        output out;
        inout arbiterCtrl;
}


simple Arbiter
{
    gates:
    	inout in_out;
}


simple Multiplexer
{
    gates:
        input in[];
        output out;
}


module Outport
{
    gates:
        inout line;
 
    submodules:
        arbiter: Arbiter;
        mux: Multiplexer;
        outbuf: Buffer;
        
    connections:
        line --> mux.in;
        mux.out --> outbuf.in;  
        outbuf.out --> line; 
        line <--> arbiter.in_out;
}


module Crossbar
{     
    parameters:
        // will be set by parent network
        int num_inports = default(0);
        double data_rate @unit(Gbps) = default(1Gbps);
        
    gates:
        inout line;

	submodules:
	    inport[num_inports]: Inport;
	    outport[num_inports]: Outport;
	    
	connections:
	    for i=0..num_inports-1, for j=0..num_inports-1
	    {
	    	inport[i].out --> outport[j].line;     
	    } 
        
}
