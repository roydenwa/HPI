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

simple LineDriver {
    parameters:
        int BUFFER_SIZE = default(10);
        int VIRTUAL_CHANNELS = default(1);
        int XON_THRES = default(1);
        int XOFF_THRES = default(1);
        double DL_CHECK_INTERVAL @unit(s)  = default(100us);
        @signal[sigPacketStuckTime-vid*](type=simtime_t);
    gates:
         inout extData;
         inout extFlowCtrl;
         inout intData;
         inout intFlowCtrl;
}
