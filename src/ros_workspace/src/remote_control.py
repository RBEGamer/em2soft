#!/usr/bin/env python
import rospy
from std_msgs.msg import String
from pymodbus.client.sync import ModbusTcpClient
import json


import serial


ser = serial.Serial('/dev/ttyUSB0', 9600) # Establish the connection on a specific port


def callback(data):
    #print('.')
    pass
    #rospy.loginfo(rospy.get_caller_id() + "I heard %s", data.data)


if __name__ == '__main__':
    try:
        rospy.init_node('remotecontrol', anonymous=True)
        rospy.Subscriber("fbstate", String, callback)
        #rospy.Subscriber("uimsg", String, callbackui)
        rate = rospy.Rate(10) # 10hz

       # client = ModbusTcpClient('192.168.1.17', port=5020)

        pub = rospy.Publisher('fromfb', String, queue_size=10)
        #pub_fb = rospy.Publisher('fbstate', String, queue_size=10)

        while not rospy.is_shutdown():
            try:
            #print(".")
                line = ser.readline()
                
                
                line = line.split("_")
                print(line)
                if(line[2] == "cpkg"):
                    print("cpkg")
                    pass

                pub.publish(json.dumps({
                    "breaklevel":line[3],
                     "velocity":line[2],
                }))

            except:
                 rospy.loginfo("modbus_error")
            rate.sleep()

    except rospy.ROSInterruptException:
        pass