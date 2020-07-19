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

module Node
{
    parameters:
        volatile double SEND_DELAY @unit(s) = default(uniform(1.0us, 25.0us));
        volatile int PKT_SIZE @unit(byte) = default(intuniform(32byte, 1504byte));
        double DL_CHECK_INTERVAL @unit(s) = default(100us);
        int NODE_ID = default(0);
        string NODE_COUNT = default("1");
        int PORTS = default(2);
        int FLIT_BYTES @unit(byte) = default(16byte);
        int HL_BUFFER_CAPACITY = default(10);
        int LINE_DRIVER_RX_BUFFER_SIZE = default(16);
        int VIRTUAL_CHANNELS = default(2);
        int XON_THRES = default(1);
        int XOFF_THRES = default(1);
        bool TOPO_CUBE = default(false); // Either Mesh or Cube.
        bool ROUTE_TEST = default(false);
        @display("bgb=417,450");
    gates:
        inout data[PORTS];
        inout flowCtrl[PORTS];
    submodules:
        LineDrivers[PORTS]: LineDriver {
            BUFFER_SIZE = LINE_DRIVER_RX_BUFFER_SIZE;
            VIRTUAL_CHANNELS = VIRTUAL_CHANNELS;
            XON_THRES = XON_THRES;
            XOFF_THRES = XOFF_THRES;
            DL_CHECK_INTERVAL = DL_CHECK_INTERVAL;
            @display("p=156,304");
        }

        Xbar: Xbar {
            VIRTUAL_CHANNELS = VIRTUAL_CHANNELS;
            PORTS = PORTS;
            NODE_ID = NODE_ID;
            NODE_COUNT = NODE_COUNT;
            TOPO_CUBE = TOPO_CUBE;
            @display("p=164,198");
        }

        AppLineDriver: AppLineDriver {
            FLIT_BYTES = FLIT_BYTES;
            HL_BUFFER_CAPACITY = HL_BUFFER_CAPACITY;
            DL_CHECK_INTERVAL = DL_CHECK_INTERVAL;
            @display("p=164,114");
        }

        Application: Application {
            FLIT_BYTES = FLIT_BYTES;
            SEND_DELAY = SEND_DELAY;
            PKT_SIZE = PKT_SIZE;
            NODE_ID = NODE_ID;
            NODE_COUNT = NODE_COUNT;
            ROUTE_TEST = ROUTE_TEST;
            @display("p=170,29");
        }

    connections:
        for i=0..(PORTS - 1) {
            data[i] <--> LineDrivers[i].extData;
            flowCtrl[i] <--> LineDrivers[i].extFlowCtrl;
            LineDrivers[i].intData <--> Xbar.data[i];
            LineDrivers[i].intFlowCtrl <--> Xbar.flowCtrl[i];
        }

        Application.data <--> AppLineDriver.hlData;
        AppLineDriver.flitData <--> Xbar.data[PORTS];
        AppLineDriver.flitFlowCtrl <--> Xbar.flowCtrl[PORTS];

}
