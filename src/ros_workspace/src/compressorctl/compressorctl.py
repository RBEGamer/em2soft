#! /usr/bin/env python
import rospy
form std_msgs.msg import String
import json






if __name__ == '__main__':
	rospy.init_node("compressorctl", anonymous=True)

	param_rate = rospy.get_param('refresh_rate','10')
	rate = rospy.Rate(int(param_rate))
	while not rospy.is_shutdown():
		rate.sleep()
