/********************************************************
 * PID Basic Example
 * Reading analog input 0 to control analog PWM output 3
 ********************************************************/

#include<SPI.h>
#include<nRF24L01.h>
#include<RF24.h>


const uint64_t pipe[1]= {0xF0F0F0F0E1LL};
RF24 radio(7,8);

const byte send_to_ctl_len = 8;
byte send_to_ctl[send_to_ctl_len] = {0,50,0,0,0,0,0,0};

const byte rec_from_ctl_len = 8;
byte rec_from_ctl[rec_from_ctl_len] = {0,0,0,0,0,0,0,0};



union crc_data
{
   unsigned short value;
   byte bytes[2];
};

unsigned short crc16(const unsigned char* data_p, unsigned char length){
    unsigned char x;
    unsigned short crc = 0xFFFF;
    while (length--){
        x = crc >> 8 ^ *data_p++;
        x ^= x>>4;
        crc = (crc << 8) ^ ((unsigned short)(x << 12)) ^ ((unsigned short)(x <<5)) ^ ((unsigned short)x);
    }
    return crc;
}






#include <PID_v1.h>

#define PIN_INPUT 1
#define PIN_OUTPUT_B 3
#define PIN_OUTPUT_A 6
//Define Variables we'll be connecting to
double Setpoint_VEL,Setpoint_BRK, Input_VEL,Input2_VEL, Output;

//Specify the links and initial tuning parameters
double Kp=1, Ki=10, Kd=5;
PID myPID(&Input_VEL, &Output, &Setpoint_VEL, Kp, Ki, Kd, DIRECT);
PID myPID2(&Input2_VEL, &Output, &Setpoint_VEL, Kp, Ki, Kd, DIRECT);




int hapitc_feedback_strengh = 100;
int current_haptic_feedback = 100;

void hf_off(){
  current_haptic_feedback = 1;
  myPID.SetOutputLimits(0, current_haptic_feedback);
myPID2.SetOutputLimits(0, current_haptic_feedback);
  }

  void hf_max(){
  current_haptic_feedback = 255;
  myPID.SetOutputLimits(0, current_haptic_feedback);
myPID2.SetOutputLimits(0, current_haptic_feedback);
  }

  void hf_normal(){
  current_haptic_feedback = hapitc_feedback_strengh;
  myPID.SetOutputLimits(0, current_haptic_feedback);
myPID2.SetOutputLimits(0, current_haptic_feedback);
  }


int hf_mode = 0;

unsigned long startMillis;

void setup()
{

 // Wire.begin(9);
//   Wire.onReceive(receiveEvent);
  Serial.begin(9600);

  radio.begin();
  delay(500);
  radio.setAutoAck(true);
  radio.enableAckPayload();
  radio.enableDynamicPayloads();
  radio.openReadingPipe(1,pipe[0]);
  radio.stopListening();
  radio.setRetries(15,15);

  
  //initialize the variables we're linked to
  Input_VEL = map(analogRead(A1), 0, 1024, 0, 100);
  Setpoint_VEL = 50;

hf_max();
  //turn the PID on
  myPID.SetMode(AUTOMATIC);
    myPID2.SetMode(AUTOMATIC);

    startMillis = millis();
}


bool wait_for_reset = false;
void loop()
{












 
Input_VEL = map(analogRead(A1), 0, 1024, 0, 100);
Input2_VEL = 100-Input_VEL;



if(wait_for_reset){

  if(Input_VEL == 50){
  send_to_ctl[4] = 1;    
    }



 
  }


send_to_ctl[0] = Input_VEL;
send_to_ctl[1] = Input2_VEL;



if (millis() - startMillis >= 50)  //test whether the period has elapsed
  {
    startMillis = millis();
     crc_data crcfbsend;
    crcfbsend.value = crc16(send_to_ctl,sizeof(send_to_ctl)-2);
    send_to_ctl[send_to_ctl_len-2] = crcfbsend.bytes[0];
    send_to_ctl[send_to_ctl_len-1] = crcfbsend.bytes[1]; 
    if(radio.write( send_to_ctl, sizeof(send_to_ctl) )){
        radio.read( &rec_from_ctl,sizeof(rec_from_ctl) );
        unsigned short crc_from_ctl = crc16(rec_from_ctl,sizeof(rec_from_ctl)-2);
        crc_data crcctlrec;
        crcctlrec.bytes[0] = rec_from_ctl[sizeof(rec_from_ctl)-2];
        crcctlrec.bytes[1] = rec_from_ctl[sizeof(rec_from_ctl)-1];

        if(crc_from_ctl == crcctlrec.value){
          Setpoint_VEL = rec_from_ctl[1];
          Setpoint_BRK = rec_from_ctl[3];
          
          
           
          if(rec_from_ctl[0] == 0){
          analogWrite(PIN_OUTPUT_A, 0);
          analogWrite(PIN_OUTPUT_B, 0);
          }

          if(rec_from_ctl[5] == 1){
           wait_for_reset  = true;
            send_to_ctl[4] = 0;
            Setpoint_VEL = 50;
           }

          
          }
   
      }
      
  }

  

   
    
    





//PID CTL FOR VELOCITY SLIDER
if(rec_from_ctl[5] == 1|| rec_from_ctl[0] > 0){
if(abs(Setpoint_VEL-Input_VEL)< 5){
   analogWrite(PIN_OUTPUT_A, 0);
   analogWrite(PIN_OUTPUT_B, 0);
    return;
  } 
if((Setpoint_VEL-Input_VEL)> 0){ 
    myPID.Compute();
   analogWrite(PIN_OUTPUT_A, 0);
   analogWrite(PIN_OUTPUT_B, abs(Output));
}
  if((Setpoint_VEL-Input_VEL)< 0){ 
    myPID2.Compute();
   analogWrite(PIN_OUTPUT_B, 0);
   analogWrite(PIN_OUTPUT_A, abs(Output));
}  
  }else{
    
    }



}
