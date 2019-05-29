#!bin/bash
cd src
cd ros_workspace
cd devel
source ./setup.bash
cd ..
cd ..
cd tablet_ui_server
node ./server.js &
cd ..
cd ros_workspace
cd src
bash ./run_ros_nodes.sh
