#!/usr/bin/env python
import rospy
from std_msgs.msg import String
from pymodbus.client.sync import ModbusTcpClient
import json




client =  ModbusTcpClient('192.168.1.17', port=5020)

def callback(data):
    print('.')
    #rospy.loginfo(rospy.get_caller_id() + "I heard %s", data.data)

def callbackui(data):
    #rospy.loginfo(rospy.get_caller_id() + "I heard %s", data.data)
    tmp = json.loads(data.data)
    print(tmp['event'])
    if(tmp['event'] == 'dirch'):
        rr = client.read_coils(1,1,unit=1)
        if( rr.bits[0]):
            client.write_coil(1,False, unit=1)
        else:
            client.write_coil(1,True, unit=1)
    
    if(tmp['event'] == 'ctlmode'):
        rr = client.read_coils(1,1,unit=1)
        if( rr.bits[0]):
            client.write_coil(1,False, unit=1)
        else:
            client.write_coil(1,True, unit=1)

def callbackfb(data):
    rospy.loginfo(rospy.get_caller_id() + "I heard %s", data.data)
    tmp = json.loads(data.data)
    #print(tmp['breaklevel'])

    brklvl = int(tmp['breaklevel'])
    was_error = True
    if(brklvl >80):
        brklvl = 4
    elif(brklvl > 50):
        brklvl = 3
    elif(brklvl > 30):
        brklvl = 2
    elif(brklvl > 15):
        brklvl = 1
    elif(brklvl <= 15):
        brklvl = 0
    else:
        was_error = False
        brklvl = 4
        #%ODO SEND ERROR
    print(brklvl)
    if was_error:
        client.write_register(0,brklvl, unit = 1)


   
        


if __name__ == '__main__':
    try:
        rospy.init_node('modbus', anonymous=True)
        rospy.Subscriber("chatter", String, callback)
        rospy.Subscriber("uimsg", String, callbackui)
        rospy.Subscriber("fromfb", String, callbackfb)

        rate = rospy.Rate(10) # 10hz

       # client = ModbusTcpClient('192.168.1.17', port=5020)

        pub = rospy.Publisher('state', String, queue_size=10)
        pub_fb = rospy.Publisher('fbstate', String, queue_size=10)

        while not rospy.is_shutdown():
            try:
            #print(".")
                response = client.read_input_registers(0,20,unit=1)

                #print(response.registers)

                pub.publish(json.dumps({
                    "kompressordruck": response.registers[4] / 100.0,
                    "kmh": response.registers[3] / 100.0,
                    "kn":response.registers[15],
                    "breaklevel":100-response.registers[5],
                    "direction":response.registers[8],
                    "state_v0":response.registers[11],
                    "state_v1":response.registers[12],
                    "state_v2":response.registers[13],
                    "state_v3":response.registers[14],
                    "ctlmode":response.registers[10] # 1= current 0=rpm

                }))

#input register 0 schreibe 0-4
                pub_fb.publish(json.dumps({
                    "breaklevel":100-response.registers[5],
                }))
            except:
                 rospy.loginfo("modbus_error")
            rate.sleep()

    except rospy.ROSInterruptException:
        client.close()