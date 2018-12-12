# `/src` FOLDER STRUCTURE


This folder contain all sourcefiles for the project.

# FOLDERS
* `ros_workspace` - The ros packages for the main pc, building the heart of the system
* `tablet_ui_server` - A NodeJS webserver for the information/control interface
* `wago_sps_software` - The Wago PLC software to control/read all actors and sensors and sending it to the ros system



# SETUP THE SYSTEM
If you have all components together and connected you can setup the TX2-Board.
* Install `Ubuntu 16.04` on a VM/PC and download the `Jetson` Installer for your TX2 board, from the `NVIDIA developer site`.
* Flash the latest Jetson Image to board.


* Install `ROS Kinectic`
* Install `NodeJS >= V. 8`
* Install `Python-Pip`


* Clone this repository `git clone https://github.com/RBEGamer/em2soft.git`




# RUN ALL TOGETHER

* Flash the PLC Software to your WagoSPS
* Check the IP Adress of the PLC and the TX2-Board, and check them with the ROS configuration (`./ros_packages/src/modbus_bridge/src/modbus_bridge_node.cpp`)
* Run the start script `bash ./start_all.sh` to start `roscore`, `ros nodes`, `tablet_ui_server`

