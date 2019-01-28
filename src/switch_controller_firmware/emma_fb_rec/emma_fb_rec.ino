
#include<SPI.h>
#include<nRF24L01.h>
#include<RF24.h>


#define ERROR_NOTAUS_CHECK_TIMEOUT 150 //x fehler in 150ms
#define NOTAUS_ERROR_COUNT 3 //fehler bis notaus
#define PACKET_LOSS_TIMEOUT 80 //alle x ms muss ein paket von FB gesendet worden sein

#define PIN_NOTAUS_RELAIS 2
#define NOTAUS_RELAIS_ACTIVE LOW
#define NOTAUS_RELAIS_INACTIVE HIGH

#define PIN_HEARTBEAT 3
bool heartbeat_state  = 0;


unsigned int errors_in_time = 0;
const uint64_t pipe[1]= {0xF0F0F0F0E1LL};
RF24 radio(7,8);

//die letzen beiden bytes sind immer crc daten -> also nutzdaten immer zwei byte weniger
const byte send_to_fb_len = 8;
byte send_to_fb[send_to_fb_len] = {0,30,0,50,0,0,0,0};

const byte rec_from_fb_len = 8;
byte rec_from_fb[rec_from_fb_len] = {0,0,0,0,0,0,0,0};


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





void send_fb_data_toctl(){
  //daten senden mosbus oder serial
}

bool fb_reset_ok = false;
void send_fb_default(){
  //sends default
  send_to_fb[5] = 1;
  fb_reset_ok = false;
  }


void fb_error_notaus(){
  //notaus an fb senden
   digitalWrite(PIN_NOTAUS_RELAIS,NOTAUS_RELAIS_ACTIVE);
   send_to_fb[4] = 1; //SEND FB NOTAUS
   
  }


unsigned long mili_timer_crc = 0;
unsigned long mili_timer_last_packet = 0;
unsigned long mili_timer_heartbeat = 0;
void setup()
{
  Serial.begin(9600);
  pinMode(PIN_HEARTBEAT,OUTPUT);
  
  pinMode(PIN_NOTAUS_RELAIS,OUTPUT);
  digitalWrite(PIN_NOTAUS_RELAIS,NOTAUS_RELAIS_INACTIVE);
  radio.begin();
  delay(500);
  radio.setAutoAck(true);
  radio.enableAckPayload();
  radio.enableDynamicPayloads();
  radio.openReadingPipe(1,pipe[0]);
  radio.startListening();
  radio.setRetries(15,15);
  send_fb_default();

  mili_timer_crc = millis();
  mili_timer_last_packet = millis();
  mili_timer_heartbeat = millis();
}
void loop()
{
  if ( radio.available() ) {
    crc_data crcfbsend;
    crcfbsend.value = crc16(send_to_fb,sizeof(send_to_fb)-2);
    send_to_fb[send_to_fb_len-2] = crcfbsend.bytes[0];
    send_to_fb[send_to_fb_len-1] = crcfbsend.bytes[1];
    radio.writeAckPayload( 1, send_to_fb, sizeof(send_to_fb) );
    
    radio.read( &rec_from_fb,sizeof(rec_from_fb) );

     unsigned short crc_from_fb = crc16(rec_from_fb,sizeof(rec_from_fb)-2);
    crc_data crcfbrec;
    crcfbrec.bytes[0] = rec_from_fb[sizeof(rec_from_fb)-2];
    crcfbrec.bytes[1] = rec_from_fb[sizeof(rec_from_fb)-1];

    if(crcfbrec.value == crc_from_fb){
      
  //SHOW HEARTBEAT
  if(millis()-mili_timer_heartbeat > 500){
    mili_timer_heartbeat = millis();
    heartbeat_state = !heartbeat_state;
    digitalWrite(PIN_HEARTBEAT, heartbeat_state);
    }

  
 //fb hat den reset status eingenommen nach RESET
if(send_to_fb[5] == 1 && rec_from_fb[4] == 1){
  send_to_fb[5] = 0;
  fb_reset_ok = true;
  Serial.println("got reset state");
  }

 //fb hat den reset status eingenommen nach NOTAUS
 if(send_to_fb[4] == 1 && rec_from_fb[4] == 1){
  send_to_fb[4] = 0;
  fb_reset_ok = true;
  }

          
      }else{
        errors_in_time++;
        Serial.println(errors_in_time);
        }

mili_timer_last_packet = millis();
        
    
}

//CHECK LAST PACKET TIME

          if(millis() - mili_timer_last_packet >PACKET_LOSS_TIMEOUT){
          errors_in_time++;
          }
            

//CHECK INTERVAL FOR CRC ERRORS
if(millis() - mili_timer_crc > ERROR_NOTAUS_CHECK_TIMEOUT){
  mili_timer_crc = millis();
  if(errors_in_time > NOTAUS_ERROR_COUNT){  
    fb_error_notaus();//sende notaus an fb
    }
    errors_in_time = 0;
  }




}
