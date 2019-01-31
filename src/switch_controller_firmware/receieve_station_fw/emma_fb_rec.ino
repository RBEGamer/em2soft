
#include<SPI.h>
#include<nRF24L01.h>
#include<RF24.h>

#define CAN_ADDR_STATE_SEND 0x48 //the can addr to send the current values
#define MCP_CAN_CE_PIN 10
#include <mcp_can.h>
MCP_CAN CAN(MCP_CAN_CE_PIN);

#define ERROR_NOTAUS_CHECK_TIMEOUT 555 //x fehler in 150ms
#define NOTAUS_ERROR_COUNT 3 //fehler bis notaus
#define PACKET_LOSS_TIMEOUT 80 //alle x ms muss ein paket von FB gesendet worden sein

#define PIN_NOTAUS_RELAIS 2
#define NOTAUS_RELAIS_ACTIVE LOW
#define NOTAUS_RELAIS_INACTIVE HIGH

#define PIN_HEARTBEAT 3
bool heartbeat_state  = 0;
#define HEARTBEAT_TOGGLE_TIME 3000

unsigned int errors_in_time = 0;
const byte addresses[][6] = {"00001", "00002"};
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




bool fb_reset_ok = false;


void send_fb_default(){
  //sends default
  send_to_fb[5] = 1;
  rec_from_fb[4] = 0;

  fb_reset_ok = false;
  }


void fb_error_notaus(){
  //notaus an fb senden
   digitalWrite(PIN_NOTAUS_RELAIS,NOTAUS_RELAIS_ACTIVE);
   send_to_fb[4] = 1; //SEND FB NOTAUS
   Serial.println("notaus");
  while(1){
    delay(1000);
    Serial.println("waiting for reset notaus");
    }
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
   delay(1000);
  radio.openWritingPipe(addresses[0]); // 00002
  radio.openReadingPipe(1, addresses[1]); // 00001
  radio.setPALevel(RF24_PA_MAX);
  radio.startListening();

  
  send_fb_default();


  while (CAN_OK != CAN.begin(CAN_250KBPS))
    {
        Serial.println("CAN BUS init Failed");
        delay(300);
        heartbeat_state = !heartbeat_state;
         digitalWrite(PIN_HEARTBEAT, heartbeat_state);
          digitalWrite(PIN_NOTAUS_RELAIS,NOTAUS_RELAIS_ACTIVE);
    
    }
Serial.println("CAN BUS Shield Init OK!");


  mili_timer_crc = millis();
  mili_timer_last_packet = millis();
  mili_timer_heartbeat = millis();
}
void loop()
{



    
    
  if ( radio.available() ) {
   
    while (radio.available()) {
    radio.read( &rec_from_fb,sizeof(rec_from_fb) );
    }
     unsigned short crc_from_fb = crc16(rec_from_fb,sizeof(rec_from_fb)-2);
    crc_data crcfbrec;
    crcfbrec.bytes[0] = rec_from_fb[sizeof(rec_from_fb)-2];
    crcfbrec.bytes[1] = rec_from_fb[sizeof(rec_from_fb)-1];

    if(crcfbrec.value == crc_from_fb){

  //SHOW HEARTBEAT
  if(millis()-mili_timer_heartbeat > HEARTBEAT_TOGGLE_TIME){
    mili_timer_heartbeat = millis();
    heartbeat_state = !heartbeat_state;
    digitalWrite(PIN_HEARTBEAT, heartbeat_state);
    Serial.println("hb");
    }

  
 //fb hat den reset status eingenommen nach RESET
if(send_to_fb[5] == 1 && rec_from_fb[4] == 1){
  send_to_fb[5] = 0;
  rec_from_fb[4] = 0;
  fb_reset_ok = true;
  Serial.println("got reset state");
  }

 //fb hat den reset status eingenommen nach NOTAUS
 if(send_to_fb[4] == 1 && rec_from_fb[4] == 1){
  send_to_fb[4] = 0;
  send_to_fb[5] = 0;
  fb_reset_ok = true;
  }

  //  send_fb_data_toctl();

if(fb_reset_ok){
 // Serial.println(rec_from_fb[0]);
   
   bool is_ebraking = false;
   byte vel_amount = 0;
   byte ebrk_amount = 0;
   byte pneu_brake = map(rec_from_fb[1],0,100,0,3);
   
   if(rec_from_fb[0] < 50){
      is_ebraking = true;
      vel_amount = 0;
      ebrk_amount = map(rec_from_fb[0],0,49,0,100);
   }else{
      ebrk_amount = 0;
      ebrk_amount = map(rec_from_fb[0],50,100,0,100);
   }
  
   
   
   //TODO TEST
   //activeates haptic feedback for penumatic controller
   send_to_fb[2] = 1;
   send_to_fb[3] = pneu_brake*10;
   
   
 unsigned char stmp[8] = {vel_amount, ebrk_amount,pneu_brake , is_ebraking, rec_from_fb[3], 0, 0, 0};
 CAN.sendMsgBuf(CAN_ADDR_STATE_SEND, 0, 8, stmp);
   
   
  }

  

 radio.stopListening();
delay(10);
    crc_data crcfbsend;
    crcfbsend.value = crc16(send_to_fb,sizeof(send_to_fb)-2);
    send_to_fb[send_to_fb_len-2] = crcfbsend.bytes[0];
    send_to_fb[send_to_fb_len-1] = crcfbsend.bytes[1];
    radio.write(send_to_fb, sizeof(send_to_fb) );
delay(10);
 radio.startListening();

 
          
      }else{
        errors_in_time++;
        Serial.println("crc err");
        }




mili_timer_last_packet = millis();
        
    
}






//CHECK LAST PACKET TIME
          if(millis() - mili_timer_last_packet >PACKET_LOSS_TIMEOUT){
          errors_in_time++;
          Serial.println("cpkg err");
          mili_timer_last_packet = millis();
          }
            

//CHECK INTERVAL FOR CRC ERRORS
if(millis() - mili_timer_crc > ERROR_NOTAUS_CHECK_TIMEOUT){
  mili_timer_crc = millis();

  if(errors_in_time > NOTAUS_ERROR_COUNT){  
    fb_error_notaus();//sende notaus an fb
     errors_in_time = 0;
    }
    errors_in_time = 0;
  }




}
