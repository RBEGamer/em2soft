# THE SWITCH LIKE CONTROLLER

To control the locomotive, we are designing a controller with 2 sliders to control velocity, e-breake, system break.
The sliders are synced with the current velocity of the locomotive. The motors within the sliders can move the slider handle for example to the stop position.
The current velocity is shown by the left slider, so if thespped is 7km/h the slider handle is positioned at 70% by the motor. if you want to increase the speed you simply can push the slider to.



# FEATURES
* simple protocol with CRC32
* heartbeat for packetloss, out of range
* relais to directly enable the emergency brake if heartbet not present
* can bus protocol to send data to main control unit
*



# HARDWARE
* 1x Arduino Nano
* 1x Arduino Due
* 1x MCP2515 Can module
* 4x 1N diodes
* 3x 5V relais
* 3x BC337
* 3x 2k resistor
* 2x Alps motor potentiometer 150mm linear 10k
* lx d293d h bridge
* 3x push switch
* 2x 4pin connectors
* 2x Red/Green Signal lights 12V
* 2x NRF24 modules



# SOFTWARE
* `receieve_station_fw` -> Firmware for the ground station
* `remote_controller_fw` -> Firmware for the 
