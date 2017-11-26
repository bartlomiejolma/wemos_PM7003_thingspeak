#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include "my_credentials.h"

WiFiClient client;

const int led = 2;
const int set = 5;


#define LENG 31   //0x42 + 31 bytes equal to 32 bytes
unsigned char buf[LENG];




void setup(void){
  pinMode(led, OUTPUT);
  pinMode(set, OUTPUT);
  digitalWrite(led, 1);
  digitalWrite(set, 1);
  Serial.begin(9600);
  WiFi.begin(ssid, password);

  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
  }

}

void loop(void){
  if(Serial.find(0x42))
  {    
    Serial.readBytes(buf,LENG);
    if(buf[0] == 0x4d)
    {
      if(checkValue(buf,LENG))
      {
        int PM01Value=transmitPM01(buf); //count PM1.0 value of the air detector module
        int PM2_5Value=transmitPM2_5(buf);//count PM2.5 value of the air detector module
        int PM10Value=transmitPM10(buf); //count PM10 value of the air detector module 

        if (client.connect(server,80)) 
        {
          String postStr = apiKey;
          postStr +="&field1=";
          postStr += String(PM01Value);
          postStr +="&field2=";
          postStr += String(PM2_5Value);
          postStr +="&field3=";
          postStr += String(PM10Value);
          postStr += "\r\n\r\n";
           
          client.print("POST /update HTTP/1.1\n");
          client.print("Host: api.thingspeak.com\n");
          client.print("Connection: close\n");
          client.print("X-THINGSPEAKAPIKEY: "+apiKey+"\n");
          client.print("Content-Type: application/x-www-form-urlencoded\n");
          client.print("Content-Length: ");
          client.print(postStr.length());
          client.print("\n\n");
          client.print(postStr);
           
        }
        client.stop();

        digitalWrite(set, 0);   
        delay(5*60*1000);
        digitalWrite(set, 1);
      }
     }           
   }
     
}

char checkValue(unsigned char *thebuf, char leng)
{  
  char receiveflag=0;
  int receiveSum=0;
 
  for(int i=0; i<(leng-2); i++){
  receiveSum=receiveSum+thebuf[i];
  }
  receiveSum=receiveSum + 0x42;
  
  if(receiveSum == ((thebuf[leng-2]<<8)+thebuf[leng-1]))  //check the serial data 
  {
    receiveSum = 0;
    receiveflag = 1;
  }
  return receiveflag;
}

int transmitPM01(unsigned char *thebuf)
{
  int PM01Val;
  PM01Val=((thebuf[3]<<8) + thebuf[4]); //count PM1.0 value of the air detector module
  return PM01Val;
}
 
//transmit PM Value to PC
int transmitPM2_5(unsigned char *thebuf)
{
  int PM2_5Val;
  PM2_5Val=((thebuf[5]<<8) + thebuf[6]);//count PM2.5 value of the air detector module
  return PM2_5Val;
  }
 
//transmit PM Value to PC
int transmitPM10(unsigned char *thebuf)
{
  int PM10Val;
  PM10Val=((thebuf[7]<<8) + thebuf[8]); //count PM10 value of the air detector module  
  return PM10Val;
}
