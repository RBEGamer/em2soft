#!bin/bash

# launch all ros nodes
bash ./ros_workspace/src/run_ros_launch.sh &

#als letztes ohne &
node ./tablet_ui_server/server.js
