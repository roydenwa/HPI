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

import ub04_hpi09.App;
import ub04_hpi09.Buffer;


module Node
{
    @display("bgb=263,209");
    gates:
        inout line;

    submodules:
        app: App {
            @display("p=35,135");
        }
        buffer: Buffer {

            @display("p=124,40");
        }
    connections:
        app.in <-- buffer.out;
        app.out --> buffer.in;
        buffer.line <--> line;
}
