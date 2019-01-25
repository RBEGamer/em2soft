// ros_modbus_bridge node
// @author Marcel Ochsendorf
// @version 1.0


#include "ros/ros.h"
#include "std_msgs/String.h"

#include "./modbus.cpp"
//string array for coils -> topic -> names


int main(int argc, char** argv){

ros::init(argc, argv, "modbus_bridge_node");

ros::NodeHandle n;
ros::Publisher pub_main = n.advertise<std_msgs::String>("modbus_bridge_", 1000);

ros::Rate loop_rate(10);



std::string plc_ip = "";
bool ok = ros::param::get("plc_ip", plc_ip);
if(!ok) { ROS_FATAL_STREAM("parameter get plc_ip err");
exit(1);
}

modbus mb = modbus(plc_ip, 502);
mb.modbus_set_slave_id(1);
mb.modbus_connect();

while (ros::ok()){


ROS_INFO("modbus_bridge_log");
ros::spinOnce();
loop_rate.sleep();
}




mb.modbus_close();
delete(&mb);


return 0;
}
