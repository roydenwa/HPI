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
    parameters:
        // will be set by parent module Outport
        int num_inports = default(0);

    gates:
        input in;
        output out[num_inports];
        inout arbiterCtrl;
}


simple Arbiter
{
    gates:
    	inout in_out;
}


simple Multiplexer
{
    parameters:
        // will be set by parent module Outport
        int num_inports = default(0);
   
    gates:
        input in[num_inports];
        output out;
}


module Outport
{
    parameters:
        // will be set by parent module Crossbar
        int num_inports = default(0);
        
        // pass param to child module
        mux.num_inports = num_inports;
    
    gates:
        inout line;
        input in[num_inports];
        output out;
 
    submodules:
        arbiter: Arbiter;
        mux: Multiplexer;
        outbuf: Buffer;
        
    connections:
        // outport.in-s to mux.in-s
        for i=0..num_inports-1
        {
            in[i] --> mux.in[i];
        }
        
        mux.out --> outbuf.in;  
        outbuf.out --> out; 
        line <--> arbiter.in_out;
}


module Crossbar
{     
    parameters:
        // will be set by parent network module
        int num_inports = default(0);
        double data_rate @unit(Gbps) = default(1Gbps);
        
        // pass param to child modules
        outport[*].num_inports = num_inports;
        inport[*].num_inports = num_inports;
        
    gates:
        input in[num_inports];
        output out[num_inports];

	submodules:
	    inport[num_inports]: Inport;
	    outport[num_inports]: Outport;
	    
	connections:
	    // input to internal part
	    for i=0..num_inports-1
	    {
	        in[i] --> inport[i].in;
	    }
	    
	    // internal part
	    for i=0..num_inports-1, for j=0..num_inports-1
	    {
	    	inport[i].out[i] --> { datarate=data_rate; } --> outport[j].in[i];     
	    }
	    
	    // inports to arbiters in outports
	    for i=0..num_inports-1, for j=0..num_inports-1
	    {
	        inport[i].arbiterCtrl <--> outport[j].line;
	    }
	    
	    // internal part to output
        for i=0..num_inports-1
        {
            outport[i].out --> out[i];
        }
}
