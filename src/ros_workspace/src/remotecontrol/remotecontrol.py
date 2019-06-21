#!/usr/bin/env python
import rospy
from std_msgs.msg import String
from pymodbus.client.sync import ModbusTcpClient
import json


import serial

velsetpoint = 0
breaksetpoint = 0

ser = None
import time

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

        con_ok = False

        while not con_ok:
            try:
                serial.Serial(param_port, int(param_baud))
                con_ok = True
            except:
                con_ok = False
        time.sleep(10)



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
                hupe = line[7]

                if(hupe == "1"):
                    hupe = True
                else:
                    hupe = False
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
                        "hupe": hupe
                    }))

            except:
                rospy.loginfo("serial_error")
            rate.sleep()

    except rospy.ROSInterruptException:
        pass