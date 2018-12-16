# `/src` FOLDER STRUCTURE


This folder contain all sourcefiles for the project.

# FOLDERS
* `ros_workspace` - The ros packages for the main pc, building the heart of the system
* `tablet_ui_server` - A NodeJS webserver for the information/control interface
* `wago_sps_software` - The Wago PLC software to control/read all actors and sensors and sending it to the ros system



# SETUP THE SYSTEM
If you have all components together and connected you can setup the TX2-Board.
* Install `Ubuntu 16.04` on a VM/PC and download the `Jetson` Installer for your TX2 board, from the `NVIDIA developer site`.
* Flash the latest Jetson Image `JetPack 3.3` to the board.

Install on your development PC and the TX2 Board the following packages:

* Install `ROS Kinectic`
* Install `NodeJS >= V. 8`
* Install `Python-Pip`


For easy installation on a clean PC `Ubuntu 16.04` you can use the `./ubuntu_install_req.sh` script.
For installation on the TX2 Board please use the `./tx2_install_req.sh`

* Clone this repository `git clone https://github.com/RBEGamer/em2soft.git`




# RUN ALL TOGETHER

* Flash the PLC Software to your WagoSPS
* Check the IP Adress of the PLC and the TX2-Board, and check them with the ROS configuration (ros property server or `./ros_packages/src/launch.launch`)
* Run the start script `bash ./start_all.sh` to start `roscore`, `ros nodes`, `tablet_ui_server`



# DEVELOPING AND DEPLOYMENT
For easy development, developing the ros nodes on the Ubuntu VM, i have created a autodepoly script that transfers the ros package `./ros_workspace` to the TX2 board and run the nodes there. The Python script `./deploy_to_tx2.py` does that.
The script needs the ip, username and password from the TX2 Board to open a connecteion via ssh to transfer commands and copy files.
EXAMPLE CALL: `./deploy_to_tx2.py <tx2_ip> <username> <password>` for easy use you can edit the `./deploy_to_tx2.sh` with fixed user credentials.
The files are only copied to the `/tmp` folder of the system, after reboot or closing the connection you need to run the deploy script again.

# SETUP AUTOSART SCRIPTS
To startup the whole system at startup, you have to edit the startupfile.
`./start_all.sh
