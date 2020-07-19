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

package ss2020_ueb06.Node;

simple Xbar {
    parameters:
        int VIRTUAL_CHANNELS = default(1);
        int PORTS = default(2);
        int NODE_ID = default(0);
        string NODE_COUNT = default("1");
        bool TOPO_CUBE = default(false); // Either Mesh or Cube.
       
	gates:
	    inout flowCtrl[PORTS + 1];
	    inout data[PORTS + 1];
}
