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

simple AppLineDriver {
    parameters:
        int FLIT_BYTES @unit(byte) = default(16byte);
        int HL_BUFFER_CAPACITY = default(10);
        double DL_CHECK_INTERVAL @unit(s)  = default(100us);
        @signal[sigAldPacketStuckTime](type=simtime_t);
        @statistic[aldPacketStuckTime](title="Duration of stuck packets in the TX-Queue"; source=sigAldPacketStuckTime; record=vector);
    gates:
        inout flitData;
        inout flitFlowCtrl;
        inout hlData;
}
