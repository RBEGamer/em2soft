#!/usr/bin/env python
import rospy
from std_msgs.msg import String
from pymodbus.client.sync import ModbusTcpClient
import json
import time



client = None

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
        client.write_coil(0,True, unit=1)

    if(tmp['event'] == 'ctlmode'):
        rr = client.read_coils(2,1,unit=1)
        if( rr.bits[0]):
            client.write_coil(3,False, unit=1)
        else:
            client.write_coil(3,True, unit=1)
        #client.write_coil(4,True, unit=1)

    if(tmp['event'] == 'ascch'):
        print('ascch event')
        # TODO
        #rr = client.read_coils(1,1,unit=1)
        #if( rr.bits[0]):
        #    client.write_coil(1,False, unit=1)
        #else:
        #    client.write_coil(1,True, unit=1)
    if (tmp['event'] == 'kompmode'):
        print('kompmode event')
        #response.registers[22] 1/0/2 schreiben

    if (tmp['event'] == 'embreak'):
        print('embreak event')
        client.write_register(22,1, unit = 1)
    if (tmp['event'] == 'lightmode'):
        print('lightmode event')
        client.write_register(23,1, unit = 1)


    if (tmp['event'] == 'brkstep0'):
        print('brkstep0 event')
        client.write_register(0,0, unit = 1)
    if (tmp['event'] == 'brkstep1'):
        print('brkstep0 event')
        client.write_register(0,1, unit = 1)
    if (tmp['event'] == 'brkstep2'):
        print('brkstep0 event')
        client.write_register(0,2, unit = 1)
    if (tmp['event'] == 'brkstep3'):
        print('brkstep0 event')
        client.write_register(0,3, unit = 1)
    if (tmp['event'] == 'brkstep4'):
        print('brkstep0 event')
        client.write_register(0,4, unit = 1)


    if (tmp['event'] == 'brkloes'):
        print('brkloes event')
        client.write_coil(6,True, unit=1)
        time.sleep(2)
        client.write_coil(6,False, unit=1)

    if(tmp['event'] == 'startup'):
        print('startup event')
        client.write_coil(6,False, unit=1)
        client.write_coil(7,False, unit=1)

    if (tmp['event'] == 'hupe'):
        #print( 'hupe event')
        print(tmp)

        if(tmp['state'] == 1):
            client.write_coil(7,True, unit=1)
            print("h1")
        else:
            client.write_coil(7,False, unit=1)
            print("h0")
        
        







def callbackfb(data):
    rospy.loginfo(rospy.get_caller_id() + "I heard %s", data.data)
    tmp = json.loads(data.data)
    #print(tmp['breaklevel'])
    client.write_register(0,int(tmp['breaklevel']), unit = 1) #WRITE BREAKLEVEL TO REISTER 0^






if __name__ == '__main__':
    try:
        rospy.init_node('modbusbridge', anonymous=True)
        rospy.Subscriber("chatter", String, callback)
        rospy.Subscriber("uimsg", String, callbackui)
        rospy.Subscriber("fromfb", String, callbackfb)

        param_rate = rospy.get_param('refresh_rate', '10')
        param_port = rospy.get_param('modbus_port', '5020')
        param_ip = rospy.get_param('modbus_ip', "192.168.178.95")

        rate = rospy.Rate(int(param_rate)) # 10hz

        client = ModbusTcpClient(param_ip, port=int(param_port))


        pub = rospy.Publisher('state', String, queue_size=10)
        pub_fb = rospy.Publisher('fbstate', String, queue_size=10)

        client.write_coil(6,False, unit=1)
        client.write_coil(7,False, unit=1)
        
        while not rospy.is_shutdown():
            try:
                #print(".")
                response = client.read_input_registers(0,30,unit=1)
              
                #print(response.registers[23])

                pub.publish(json.dumps({
                    "kompressordruck": response.registers[4] / 100.0,
                    "kmh": response.registers[3] / 100.0,
                    "kn":(response.registers[15]-1000) / 100.0, # anzeige ist in N
                    "breaklevel":100-response.registers[5], # 0-4
                    "direction":response.registers[8],# 0-1
                    "state_v0":response.registers[11],
                    "state_v1":response.registers[12],
                    "state_v2":response.registers[13],
                    "state_v3":response.registers[14],
                    "ctlmode":response.registers[10], # 1= current 0=rpm
                    "fire_detcted": response.registers[20],#true fals
                    "temperature": ((response.registers[18]/100.0)-1.0)/10.0,#grad c
                    "batt_charge": (response.registers[22]-1000) / 100.0,#A
                    "asc_state":response.registers[20], #0 off 1 active 2 triggered
                    "asc_rest_dist": response.registers[19]/100.0,
                    "emergencybrake":response.registers[23], # 0 nicht 1 ausgeloesst
                    "emergencybrakereset":0,# 1 warte auf release
                    "kompressorstate":response.registers[17], # 0 = off  1= on 2= auto
                    "lightstate": response.registers[21],
                    "kompressor_power_state":response.registers[16],#0=off 1=on
                    "hupe_state":1

                }))

                #input register 0 schreibe 0-4
                pub_fb.publish(json.dumps({
                    "breaklevel":100-response.registers[5]
                }))
            except:
                rospy.loginfo("modbus_error")
            rate.sleep()

    except rospy.ROSInterruptException:
        client.close()