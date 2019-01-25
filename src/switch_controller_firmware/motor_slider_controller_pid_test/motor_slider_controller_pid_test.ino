/********************************************************
 * PID Basic Example
 * Reading analog input 0 to control analog PWM output 3
 ********************************************************/



#include <PID_v1.h>

#define PIN_INPUT 1
#define PIN_OUTPUT_B 3
#define PIN_OUTPUT_A 6
//Define Variables we'll be connecting to
double Setpoint, Input,Input2, Output;

//Specify the links and initial tuning parameters
double Kp=40, Ki=1.1, Kd=0.5;
PID myPID(&Input, &Output, &Setpoint, Kp, Ki, Kd, DIRECT);
PID myPID2(&Input2, &Output, &Setpoint, Kp, Ki, Kd, DIRECT);


byte randNumber; // if your random number constraint is 1 through 5, why do you need 4 bytes? One will do.
byte prevRand = 0;

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
void setup()
{

pinMode(4,INPUT);
pinMode(11,INPUT);
pinMode(12,INPUT);


randomSeed(analogRead(7));

 // Wire.begin(9);
//   Wire.onReceive(receiveEvent);
  Serial.begin(9600);
  //initialize the variables we're linked to
  Input = map(analogRead(A1), 0, 1024, 0, 100);
  Setpoint = 50;

hf_normal();
  //turn the PID on
  myPID.SetMode(AUTOMATIC);
    myPID2.SetMode(AUTOMATIC);
}

void loop()
{

 //Serial.println(digitalRead(4));
 
if(digitalRead(4) == HIGH){
  while(digitalRead(4) == HIGH){delay(50);}
//delay(100);
    randNumber = random(1,10); // generate random number between 1 & 5 (minimum is inclusive, maximum is exclusive)
  Setpoint = randNumber*10;
  Serial.println(Setpoint);
  }


  if(digitalRead(11) == HIGH){
  while(digitalRead(11) == HIGH){delay(50);}
  hf_mode++;
  if(hf_mode >= 3){
    hf_mode = 0;
    }
  
  switch(hf_mode){
    case 0:hf_max();break;
     case 1:hf_normal();break;
      case 2:hf_off();break;
      default:hf_mode=0;break;
    }
    
  }
Input = map(analogRead(A1), 0, 1024, 0, 100);
Input2 = 100-Input;

if(abs(Setpoint-Input)< 5){
   analogWrite(PIN_OUTPUT_A, 0);
   analogWrite(PIN_OUTPUT_B, 0);
  return;
  }


  
if((Setpoint-Input)> 0){ 
  myPID.Compute();
//Serial.println(Output);
   analogWrite(PIN_OUTPUT_A, 0);
   analogWrite(PIN_OUTPUT_B, abs(Output));
}


  if((Setpoint-Input)< 0){ 
  myPID2.Compute();
//Serial.println(Output);
   analogWrite(PIN_OUTPUT_B, 0);
   analogWrite(PIN_OUTPUT_A, abs(Output));
}


}
