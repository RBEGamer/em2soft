/********************************************************
 * PID Adaptive Tuning Example
 * One of the benefits of the PID library is that you can
 * change the tuning parameters at any time.  this can be
 * helpful if we want the controller to be agressive at some
 * times, and conservative at others.   in the example below
 * we set the controller to use Conservative Tuning Parameters
 * when we're near setpoint and more agressive Tuning
 * Parameters when we're farther away.
 ********************************************************/


#define PIN_INPUT_VEL 1
#define PIN_INPUT_BRK 2

#define PIN_OUTPUT 9
#define PIN_OUTPUT_B 5


#define PIN_OUTPUT_VEL 3
#define PIN_OUTPUT_B_VEL 6


//Define Variables we'll be connecting to
double Setpoint_BRK, Input_BTK, Output_BRK;
double Setpoint_VEL, Input_VEL, Output_VEL;
int curr_bk = 0;
double gap_brk = 0.0;
bool fw_btk = false;


int curr_vel = 0;
double gap_vel = 0.0;
bool fw_vel = false;
bool hupe = false;


#include<SPI.h>
#include<nRF24L01.h>
#include<RF24.h>


const byte addresses[][6] = {"00001", "00002"};
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

unsigned long startMillis;

void setup()
{

  radio.begin();
  delay(1000);
  radio.setPALevel(RF24_PA_MAX);
  radio.openWritingPipe(addresses[1]); // 00001
  radio.openReadingPipe(1, addresses[0]); // 00002
  radio.stopListening();

  
  //initialize the variables we're linked to
  Input_BTK = analogRead(PIN_INPUT_BRK);
  Input_VEL = analogRead(PIN_INPUT_VEL);
 
  set_bk(0); //0-4
  set_vel(0); //0-100
Serial.begin(9600);

Serial.println("__BEGIN__");
}

bool brk_ovr = false;
bool vel_ovr = false;


void set_vel(int vel){
  if(vel > 100){vel = 100;}
  if(vel < 0){vel = 0;}
  vel = 100-vel;

  if(vel < 10){vel = 0;}
Setpoint_VEL = map(vel,0,100,20,1000);

 vel_ovr = true;
  }


  void process_vel(){

    Input_VEL = analogRead(PIN_INPUT_VEL);

  gap_vel = abs(Setpoint_VEL-Input_VEL); //distance away from setpoint
  fw_vel = (Setpoint_VEL-Input_VEL)>0;
  if(gap_vel >50 && fw_vel){
  analogWrite(PIN_OUTPUT_VEL, 230);
  }else if(gap_vel >20 && fw_vel){
  analogWrite(PIN_OUTPUT_VEL, 200);
  }else{
    analogWrite(PIN_OUTPUT_VEL, 0);
   }
    
  if(gap_vel >50 && !fw_vel){
  analogWrite(PIN_OUTPUT_B_VEL, 230);
  }else if(gap_vel >30 && !fw_vel){
  analogWrite(PIN_OUTPUT_B_VEL, 200);
  }else{
    analogWrite(PIN_OUTPUT_B_VEL, 0);
   }


  if(vel_ovr && gap_brk< 5){
      vel_ovr = false;
      }
      
if(!vel_ovr){
   Setpoint_VEL=Input_VEL;
  }
 
  
curr_vel = map(Input_VEL,0,2014,0,100);

    }

void set_bk(int _bk){
  if(_bk == 0){
    Setpoint_BRK =905;
    }else if(_bk == 1){
    Setpoint_BRK =705;
    }else if(_bk == 2){
    Setpoint_BRK =505;
    }else if(_bk == 3){
    Setpoint_BRK =305;
    }else if(_bk == 4){
    Setpoint_BRK =105;
    }
    brk_ovr = true;
  }





  void process_bk(){

    Input_BTK = analogRead(PIN_INPUT_BRK);

  gap_brk = abs(Setpoint_BRK-Input_BTK); //distance away from setpoint
  fw_btk = (Setpoint_BRK-Input_BTK)>0;
  if(gap_brk >50 && fw_btk){
  analogWrite(PIN_OUTPUT, 220);
  }else if(gap_brk >20 && fw_btk){
  analogWrite(PIN_OUTPUT, 190);
  }else{
    analogWrite(PIN_OUTPUT, 0);

   
   }
    
  if(gap_brk >50 && !fw_btk){
  analogWrite(PIN_OUTPUT_B, 220);
  }else if(gap_brk >20 && !fw_btk){
  analogWrite(PIN_OUTPUT_B, 190);
  }else{
    analogWrite(PIN_OUTPUT_B, 0);
    
   }

   if(brk_ovr && gap_brk< 5){
      brk_ovr = false;
      }

      
   if(Input_BTK > 915){
    Setpoint_BRK =905;
    hupe = true;
    }else{
      hupe = false;
      }


  if(!brk_ovr){
  if(Input_BTK < 110 && Input_BTK >80){
    Setpoint_BRK =105;
    curr_bk = 4;
    }

     if(Input_BTK <310 && Input_BTK >280){
    Setpoint_BRK =305;
    curr_bk = 3;
    }

     if(Input_BTK <510 && Input_BTK >480){
    Setpoint_BRK =505;
    curr_bk = 2;
    }

      if(Input_BTK <710 && Input_BTK >680){
    Setpoint_BRK =705;
    curr_bk = 1;
    }

  if(Input_BTK <910 && Input_BTK >900){
    Setpoint_BRK =905;
    curr_bk = 0;
    }
  }else{


    }


    }
void loop()
{


  if (millis() - startMillis >= 50)  //test whether the period has elapsed
  {
    radio.stopListening();
    delay(20);
     crc_data crcfbsend;
    crcfbsend.value = crc16(send_to_ctl,sizeof(send_to_ctl)-2);
    send_to_ctl[send_to_ctl_len-2] = crcfbsend.bytes[0];
    send_to_ctl[send_to_ctl_len-1] = crcfbsend.bytes[1]; 
    radio.write( send_to_ctl, sizeof(send_to_ctl) );
     
     delay(10);
      radio.startListening();
  startMillis = millis();
  }



    if ( radio.available()) {
     

      while (radio.available()) {
        radio.read( &rec_from_ctl,sizeof(rec_from_ctl) );
        }
        
        unsigned short crc_from_ctl = crc16(rec_from_ctl,sizeof(rec_from_ctl)-2);
        crc_data crcctlrec;
        crcctlrec.bytes[0] = rec_from_ctl[sizeof(rec_from_ctl)-2];
        crcctlrec.bytes[1] = rec_from_ctl[sizeof(rec_from_ctl)-1];
    
        if(crc_from_ctl == crcctlrec.value){
          set_vel(rec_from_ctl[1]);
          set_bk(rec_from_ctl[3]);
       
  
          }else{
            Serial.println("__CRC_ERR__");}
   
      }

      
    process_bk();
    process_vel();

    send_to_ctl[0] = curr_vel;
    send_to_ctl[1] = curr_bk;
    send_to_ctl[5] = hupe;
}
