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

package exercise5.Node;

import exercise5.App.App;
import exercise5.Buffer.Buffer;


module Node
{
    parameters:
        int nodeID;
        int numNodes;
    gates:
        inout line;
    submodules:
        app: App {
            @display("p=35,96");
            nodeID = nodeID;
            numNodes = numNodes;
        }
        buffer: Buffer {
            @display("p=175,96");
        }
    connections:
        app.out --> buffer.in;
        buffer.out --> line$o;
        app.in <-- line$i;
}
