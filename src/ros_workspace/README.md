# `./ros_workspace` - ROS PACKAGE


# PACKAGES

* `./src/modbus_bridge` - Modbus communication package 


# ENABLE ROS ENVIRONMENT
To enable the ROS environment for this package you have to call `source ./devel/setup.bash` in a bash shell.
After this all ROS commands are avariable like `roscd`, `rosrun`

# BUILDING C++ ROS NODES
Some ROS Nodes are written in C++ so you have to compile them for your system.
Call `bash ./build_ros_nodes.sh` or simply `catkin_make` to build all ROS Nodes written in C++.


# RUN NODES
To run a specific ROS Node in this packages you can call `rosrun <nodename> <instancename>`. So for example if you want to run the modbus_bridge node you can call `rosrun modbus_bridge_node modbus_bridge_node`. To run all nodes used in this Project you can call `bash ./run_ros_launch.sh`. The nodes are specified in the `./launch_nodes.launch` file so you can make changes.
A running instance of the `roscore` is needed to start ros nodes.
### !Please run the nodes by using the launchscript!

# CONFIGURATION

In the `./launch_nodes.launch` file you can finddifferent system settings, like IP-Adresses and other values.

* `<param name="plc_ip" value="192.168.1.18" />` - The IP Adress of the WAGO SPS
