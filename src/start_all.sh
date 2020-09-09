#!bin/bash

#als letztes ohne &
node ./tablet_ui_server/server.js &

# launch all ros nodes
python ./ros_workspace/src/modbusbridge/modbusbridge.py

