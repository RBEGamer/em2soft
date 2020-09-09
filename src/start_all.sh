#!bin/bash

#als letztes ohne &
node ./tablet_ui_server/server.js &

python3 ./ros_workspace/src/ps4control/Gamepad/EmmaGamePadControl.py &
# launch all ros nodes
python ./ros_workspace/src/modbusbridge/modbusbridge.py

