// ros_modbus_bridge node
// @author Marcel Ochsendorf
// @version 1.0


#include "ros/ros.h"
#include "std_msgs/String.h"

#include "../include/modbus_bridge/modbus.h"
//string array for coils -> topic -> names


int main(int argc, char** argv){

ros::init(argc, argv, "modbus_bridge_node");


ros::Rate loop_rate(10);

while (ros::ok()){


ROS_INFO("modbus_bridge_log");
ros::spinOnce();
loop_rate.sleep();
}


return 0;
}
