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

package ub01_hpi09.simulations;

import ub01_hpi09.Simple2;


network Multigate
{
    // mesh network topology
    submodules:
        tic: Simple2 {
            @display("p=232,300");
        }
        toc0: Simple2 {
            @display("p=107,216");
        }
        toc1: Simple2 {
            @display("p=369,216");
        }
        toc2: Simple2 {
            @display("p=168,99");
        }
        toc3: Simple2 {
            @display("p=308,99");
        }

    connections:
        tic.out1 --> { delay = 100ms; } --> toc0.in1;
        tic.out2 --> { delay = 100ms; } --> toc1.in1;
        toc0.out1 --> { delay = 100ms; } --> toc2.in1;
        toc1.out1 --> { delay = 100ms; } --> toc3.in1;
        toc2.out1 --> { delay = 100ms; } --> toc3.in2;
        toc2.out2 --> { delay = 100ms; } --> tic.in1;
        toc1.out2 --> { delay = 100ms; } --> toc2.in2;
        toc3.out1 --> { delay = 100ms; } --> tic.in2;
        toc3.out2 --> { delay = 100ms; } --> toc0.in2;
        toc0.out2 --> { delay = 100ms; } --> toc1.in2;
}
