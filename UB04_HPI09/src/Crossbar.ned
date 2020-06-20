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


simple Inport
{
    parameters:
        // will be set by parent module Outport
        int num_inports = default(0);

    gates:
        input in;
        output out[num_inports];
        inout arbiterCtrl[num_inports];
}


simple Arbiter
{
    parameters:
        // will be set by parent module Outport
        int num_inports = default(0);

    gates:
    	inout in_out[num_inports];
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


simple Outbuffer
{
    gates:
        input in;
        output out;
}


module Outport
{
    parameters:
        // will be set by parent module Crossbar
        int num_inports = default(0);
        
        // pass param to child module
        arbiter.num_inports = num_inports;
        mux.num_inports = num_inports;
    
    gates:
        inout line[num_inports];
        input in[num_inports];
        output out;
 
    submodules:
        arbiter: Arbiter;
        mux: Multiplexer;
        outbuf: Outbuffer;
        
    connections:
        // outport.in-s to mux.in-s
        for i=0..num_inports-1
        {
            in[i] --> mux.in[i];
        }
        
        mux.out --> outbuf.in;  
        outbuf.out --> out;
        
        for i=0..num_inports-1
        {
            line[i] <--> arbiter.in_out[i];
        }
}


module Crossbar
{     
    parameters:
        // will be set by parent network module
        int num_inports = default(4);
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
	    	inport[i].out[j] --> { datarate=data_rate; } --> outport[j].in[i];      
	    }
	    
	    // inports to arbiters in outports
	    for i=0..num_inports-1, for j=0..num_inports-1
	    {
	        inport[i].arbiterCtrl[j] <--> outport[j].line[i];
	    }
	    
	    // internal part to output
        for i=0..num_inports-1
        {
            outport[i].out --> out[i];
        }
}
