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

#define PIN_INPUT_VEL A1 //analog input for velocity slider
#define PIN_INPUT_BRK A2 //analog input for break slider

#define PIN_OUTPUT_B 3
#define PIN_OUTPUT_A 6
//Define Variables we'll be connecting to
double Setpoint_VEL,Setpoint_BRK, Input_VEL,Input2_VEL,Input_BRK,Input2_BRK, Output_VEL,Output_BRK;

//Specify the links and initial tuning parameters
double Kp=100, Ki=1.1, Kd=0.1;
PID myPID(&Input_VEL, &Output_VEL, &Setpoint_VEL, Kp, Ki, Kd, DIRECT);
PID myPID2(&Input2_VEL, &Output_VEL, &Setpoint_VEL, Kp, Ki, Kd, DIRECT);


#define PIN_BUTTON_HUPE A3
#define PIN_LED_NOTAUS A4


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
unsigned long led_blink_milis = 0;
bool led_blink_state = false;
void setup()
{

 // Wire.begin(9);
//   Wire.onReceive(receiveEvent);
  Serial.begin(9600);
  pinMode(PIN_BUTTON_HUPE,INPUT_PULLUP);
  pinMode(PIN_LED_NOTAUS, OUTPUT);
  digitalWrite(PIN_LED_NOTAUS, LOW);
  radio.begin();
  delay(500);
  radio.setAutoAck(true);
  radio.enableAckPayload();
  radio.enableDynamicPayloads();
  radio.openReadingPipe(1,pipe[0]);
  radio.stopListening();
  radio.setRetries(15,15);

  
  //initialize the variables we're linked to
  Input_VEL = map(analogRead(PIN_INPUT_VEL), 0, 1024, 0, 100);
   Input_BRK = map(analogRead(PIN_INPUT_BRK), 0, 1024, 0, 100);
  Setpoint_VEL = 50;
  Setpoint_BRK = 100;

hf_max();
  //turn the PID on
  myPID.SetMode(AUTOMATIC);
    myPID2.SetMode(AUTOMATIC);

    startMillis = millis();
}


bool wait_for_reset = false;
void loop()
{




Input_VEL = map(analogRead(PIN_INPUT_VEL), 0, 1024, 0, 100);
Input2_VEL = 100-Input_VEL;

Input_BRK = 100;//map(analogRead(PIN_INPUT_BRK), 0, 1024, 0, 100);
Input2_BRK = 100-Input_BRK;



//WAIT FOR RESET LOGIC
if(wait_for_reset){
  //vel in mittelstellung; federspeicehr angelegt
  if(Input_VEL >48 && Input_VEL < 52 && Input_BRK > 90){
  send_to_ctl[4] = 1;
  digitalWrite(PIN_LED_NOTAUS, LOW);     
    }

if(millis()-led_blink_milis > 150){
  led_blink_milis = millis();
  led_blink_state = !led_blink_state;
 
digitalWrite(PIN_LED_NOTAUS, led_blink_state);  
  }
  


 
  }


send_to_ctl[0] = Input_VEL;
send_to_ctl[1] = Input_BRK;



if (millis() - startMillis >= 50)  //test whether the period has elapsed
  {
    if(digitalRead(PIN_BUTTON_HUPE) == HIGH){
      send_to_ctl[3] = 0;
      }else{
    send_to_ctl[3] = 1;
        }
    

    
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
          
          if(rec_from_ctl[2] == 0){
            //TODO FOR BREAK SLIDER
         // analogWrite(PIN_OUTPUT_A, 0);
         // analogWrite(PIN_OUTPUT_B, 0);
          }

          
          if(rec_from_ctl[0] == 0){
          analogWrite(PIN_OUTPUT_A, 0);
          analogWrite(PIN_OUTPUT_B, 0);
          }

          

          //wait for sliders in reset postion
          if(rec_from_ctl[5] == 1){
           wait_for_reset  = true;
            send_to_ctl[4] = 0;
            Setpoint_VEL = 50;
           }

          //led show notaus
          if(rec_from_ctl[4]  >0){
            digitalWrite(PIN_LED_NOTAUS,HIGH);
            }
          
          }
   
      }
      
  }

  

   
    
    

if(rec_from_ctl[5] == 1|| rec_from_ctl[2] > 0){

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
   analogWrite(PIN_OUTPUT_B, abs(Output_VEL));
}
  if((Setpoint_VEL-Input_VEL)< 0){ 
    myPID2.Compute();
   analogWrite(PIN_OUTPUT_B, 0);
   analogWrite(PIN_OUTPUT_A, abs(Output_VEL));
}  
  }else{
    
    }



}
