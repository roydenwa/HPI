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

package ub03_hpi09.simulations;

import ub03_hpi09.Node;


network Net2
{
    types: 
        channel C extends ned.DatarateChannel
        {
            datarate = 1Gbps;
            delay = 5us;
        }
    
    submodules:
        node[2]: Node;
        
    connections:
        node[0].line <--> C <--> node[1].line;
}