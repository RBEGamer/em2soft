#!/usr/bin/env python
import rospy
from std_msgs.msg import String
from pymodbus.client.sync import ModbusTcpClient
import json


import serial

velsetpoint = 0
breaksetpoint = 0

ser = None


def callback(data):
    #print('.')
    pass
    #rospy.loginfo(rospy.get_caller_id() + "I heard %s", data.data)


if __name__ == '__main__':
    try:
        rospy.init_node('remotecontrol', anonymous=True)

        rospy.Subscriber("fbstate", String, callback)

        param_rate = rospy.get_param('refresh_rate', '10')
        param_port = rospy.get_param('serialport', '/dev/ttyUSB0')
        param_baud = rospy.get_param('serialbaud', '9600')

        rate = rospy.Rate(int(param_rate)) # 10hz

        serial.Serial(param_port, int(param_baud)) # Establish the connection on a specific port


        pub = rospy.Publisher('fromfb', String, queue_size=10)


        while not rospy.is_shutdown():
            try:
                #print(".")
                line = ser.readline()


                line = line.split("_")
                print(line)
                if(line[2] == "cpkg"):
                    print("cpkg")
                    pass
                velsetpoint = line[2]
                breaksetpoint = line[3]


                #CALC BREAKLEVEL
                was_error = False
                if(breaksetpoint >80):
                    breaksetpoint = 4
                elif(breaksetpoint > 50):
                    breaksetpoint = 3
                elif(breaksetpoint > 30):
                    breaksetpoint = 2
                elif(breaksetpoint > 15):
                    breaksetpoint = 1
                elif(breaksetpoint <= 15):
                    breaksetpoint = 0
                else:
                    was_error = True
                    breaksetpoint = 4


                if(was_error):
                    rospy.logerror("INVALID breaksetpoint")
                    print(breaksetpoint)



                pub.publish(
                    json.dumps({
                        "breaklevel": breaksetpoint,
                        "velocity": velsetpoint,
                    }))

            except:
                rospy.loginfo("modbus_error")
            rate.sleep()

    except rospy.ROSInterruptException:
        pass