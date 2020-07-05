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

package exercise5.Outport;

import exercise5.Arbiter.Arbiter;
import exercise5.Mux.Mux;
import exercise5.Buffer.Buffer;
//
// TODO auto-generated module
//
module Outport
{
    parameters:
        int numPorts;
    gates:
        inout arbiterCtrl[numPorts];

        input inp[numPorts];
        output out;
    submodules:
        arbiter: Arbiter {
            @display("p=56,69");
        }
        mux: Mux {
            @display("p=56,165");
        }
        outbuf: Buffer {
            @display("p=193,165");
        }
    connections allowunconnected:
        for i=0..numPorts-1 {
            arbiterCtrl[i] <--> arbiter.arbiterCtrl[i];
            inp[i] --> mux.inp[i];
        }
        mux.out --> outbuf.in;
        outbuf.out --> out;


}
