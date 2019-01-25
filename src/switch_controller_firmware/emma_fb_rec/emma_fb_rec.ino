
#include<SPI.h>
#include<nRF24L01.h>
#include<RF24.h>
const uint64_t pipe[1]= {0xF0F0F0F0E1LL};
RF24 radio(7,8);
int rec[2] = {0,30};
const int rec_len = 2;
int red[2] = {0,0};
void setup()
{
  Serial.begin(9600);
  radio.begin();
  delay(500);
  radio.setAutoAck(true);
  radio.enableAckPayload();
  radio.enableDynamicPayloads();
  radio.openReadingPipe(1,pipe[0]);
  radio.startListening();
  radio.setRetries(15,15);
}
void loop()
{
  if ( radio.available() ) {
    radio.writeAckPayload( 1, rec, sizeof(int)*rec_len );
    
    radio.read( &red,sizeof(red) );
    Serial.print("integer got is : ");
    Serial.print(red[0]);
     Serial.print("-");
    Serial.println(red[1]);
}
}
