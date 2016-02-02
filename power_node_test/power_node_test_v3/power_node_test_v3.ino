/*
  / power supply module: router side
*/
//#include <stdio.h>


#define BAUD 9600
char code[2] = "";

#include "ds3234.h"
#include "Timer.h"

#include <XBee.h>
#include <stdlib.h>
#include <string.h>
#include <elapsedMillis.h>
#include <SoftwareSerial.h>

#define XBLEN 83 //paylenght+2(identifier)+3(randnum)+1(null)

SoftwareSerial customDue(3, 4); //Rx, Tx
SoftwareSerial altserial(8,9);

XBee xbee = XBee();
XBeeResponse response = XBeeResponse();
// create reusable response objects for responses we expect to handle 
ZBRxResponse rx = ZBRxResponse();
ModemStatusResponse msr = ModemStatusResponse();

elapsedMillis timeElapsed;

uint8_t payload[XBLEN];


const int cs = 10; //chip select
const int sv = 9; //switch for voltage reading
//const int al = 2; //interrupt for alarm
//const int rv= A1; //read voltage reading
const int rv = A0; //read voltage reading
const int trigForDue = 7; //enable due switch
const int en = 5; //enable due switch?
const int getDataFlag = 0; //0 default; 1 if ready na

char *command = "ARQCMD6T";

char *streamBuffer;

int timestart = millis();
int timenow = millis();
char* messageStr = NULL;
int loopnum = 0;
int partnum = 0;
String timestamp = "";

int globalChecker = 0;
//default
uint8_t sleep_period = 2;       // the sleep interval in minutes between 2 consecutive alarms

int lastSec, lastMin, lastHr;

Timer t;
int x = 0;
int breaker = 0;

/*********************/
int exc=0;
int parts=0;
unsigned char i=0;
unsigned char j=0;
int skip=0;
int length=0;
int paylength=77;
int datalen=0;
char *tosend[2];
char *xbsend[2];
char xsend[XBLEN];
//char id[16][3]={"A0","A1","A2","A3","A4","A5","A6","A7","A8","A9","AA","AB","AC","AD","AE","AF"};
//char id[16][3]={"B0","B1","B2","B3","B4","B5","B6","B7","B8","B9","BA","BB","BC","BD","BE","BF"};
char id='B';
char end_id[]="^";
char *dummy;

int randummy=0;
char randchar[4]="222";
char pad[10]="";


int count_success=0;
int verify_send[24]={0};
char *data;

int xbFlag=0;
//char dataXB[160];

char *start;
/*********************/

// SH + SL Address of receiving XBee
XBeeAddress64 addr64 = XBeeAddress64(0x0013a200, 0x40BAD1D0);  //Coordinator
//XBeeAddress64 addr64 = XBeeAddress64(0x0013a200, 0x40bf920e);  //R1
//XBeeAddress64 addr64 = XBeeAddress64(0x0013a200, 0x40309ef7);  //R2
ZBTxRequest zbTx = ZBTxRequest(addr64, payload, sizeof(payload));
ZBTxStatusResponse txStatus = ZBTxStatusResponse();

//int pin5 = 0;

#include <SPI.h>

void setup() {
  //setup softserial muna
  //customdue

  // put your setup code here, to run once:
  Serial.begin(BAUD);
  customDue.begin(BAUD);
  //customDue.begin(C
  altserial.begin(9600);
  xbee.begin(altserial);

  RTC_init();
  DS3234_init(cs, DS3234_INTCN); //nagawa na
  DS3234_clear_a2f(cs);
  //regularInterval(sleep_period);
  Serial.println(ReadTimeDate(&lastSec, &lastMin, &lastHr));
  pinMode(sv, OUTPUT);
  pinMode(trigForDue, OUTPUT);
  pinMode(en, OUTPUT);
  Serial.println(F("STARTING"));
}

void loop() {
  // put your main code here, to run repeatedly:

  //if (getDataFlag == 1 ) { //na receiev na yung flag mua sa xbee :o
  //getData();
  //}
  altserial.listen();
  delay(500);
  getXBFlag();
  if (xbFlag == 1){
    
    getData();
    xbFlag=0;
    
    
  }

  if (Serial.find("PM")) {
    if ((char)Serial.read() == '+') {
      Serial.readBytesUntil('+', code, 2);
      //baka mag hang

      switch (code[0]) {

        case 'S': { //oks
            //SETUP TIME
            Serial.println(F("Set current time"));
            setupTime();
            Serial.println();
            break;
          }

        case 'R': { //oks
            //READ TIME
            Serial.println(F("Read Time"));
            Serial.println(ReadTimeDate(&lastSec, &lastMin, &lastHr));
            Serial.println();
            break;
          }

        case 'A': { //oks
            //SET ALARM
            Serial.println(F("Set Alarm Interval"));
            //setAlarmInterval();
            Serial.println();
            break;
          }

        case 'I': { //oks
            //READ INTERVAL
            Serial.println(F("Read Current Alarm Interval:"));
            Serial.println(sleep_period);

            Serial.println();
            break;
          }

        case 'D': { //oks
            //SHUT DOWN
            Serial.println(F("Shut down custom due"));
            digitalWrite(trigForDue, LOW);// or high? depends
            Serial.println();
            break;
          }

        case 'V': { //oks
            //VOLTAGE MONITOR
            Serial.println(F("Read voltage: "));
            digitalWrite(sv, HIGH);
            float voltage = (analogRead(A0) * (14.0 / 1023.0)); //mapped with 14V as maximum
            Serial.println(voltage);
            digitalWrite(sv, LOW);
            Serial.println();
            break;
          }

      }
    }
    else {
      Serial.println(F("OK"));
      Serial.println();
    }

  }

  delay(1000);
}


int RTC_init() {
  /*added code for changing stuff */
  pinMode(cs, OUTPUT); // chip select
  SPI.begin();
  SPI.setBitOrder(MSBFIRST);
  SPI.setDataMode(SPI_MODE1); // both mode 1 & 3 should work
  digitalWrite(cs, LOW);
  SPI.transfer(0x8E);
  SPI.transfer(0x60); //60= disable Osciallator and Battery SQ wave @1hz, temp compensation, Alarms disabled
  SPI.transfer(0x4); // for interrupt
  digitalWrite(cs, HIGH);
  delay(10);
}

int SetTimeDate(int d, int mo, int y, int h, int mi, int s) {
  SPI.setBitOrder(MSBFIRST);
  SPI.setDataMode(SPI_MODE1); // both mode 1 & 3 should work

  int TimeDate [7] = {s, mi, h, 0, d, mo, y};
  for (i = 0; i <= 6; i++) {
    if (i == 3)
      i++;
    int b = TimeDate[i] / 10;
    int a = TimeDate[i] - b * 10;
    if (i == 2) {
      if (b == 2)
        b = B00000010;
      else if (b == 1)
        b = B00000001;
    }
    TimeDate[i] = a + (b << 4);
    digitalWrite(cs, LOW);
    SPI.transfer(i + 0x80);
    SPI.transfer(TimeDate[i]);
    digitalWrite(cs, HIGH);
  }
}

String ReadTimeDate(int *secp, int *minp, int *hrp) {
  String temp;
  int TimeDate [7]; //second,minute,hour,null,day,month,year

  SPI.setBitOrder(MSBFIRST);
  SPI.setDataMode(SPI_MODE1); // both mode 1 & 3 should work
  for (i = 0; i <= 6; i++) {
    if (i == 3)
      i++;
    digitalWrite(cs, LOW);
    SPI.transfer(i + 0x00);
    unsigned int n = SPI.transfer(0x00);
    digitalWrite(cs, HIGH);
    int a = n & B00001111;
    if (i == 2) {
      int b = (n & B00110000) >> 4; //24 hour mode
      if (b == B00000010)
        b = 20;
      else if (b == B00000001)
        b = 10;
      TimeDate[i] = a + b;
    }
    else if (i == 4) {
      int b = (n & B00110000) >> 4;
      TimeDate[i] = a + b * 10;
    }
    else if (i == 5) {
      int b = (n & B00010000) >> 4;
      TimeDate[i] = a + b * 10;
    }
    else if (i == 6) {
      int b = (n & B11110000) >> 4;
      TimeDate[i] = a + b * 10;
    }
    else {
      int b = (n & B01110000) >> 4;
      TimeDate[i] = a + b * 10;
    }
  }

  //2020-12-31 23:55:00
  temp.concat("20");
  temp.concat(TimeDate[6]); // YEAR
  temp.concat("-") ;
  if (TimeDate[5] < 10)
    temp.concat("0");
  temp.concat(TimeDate[5]); //MONTH
  temp.concat("-") ;
  if (TimeDate[4] < 10)
    temp.concat("0");
  temp.concat(TimeDate[4]); //DAY
  temp.concat(" ") ;
  if (TimeDate[2] < 10)
    temp.concat("0");
  temp.concat(TimeDate[2]); // HH
  temp.concat(":") ;
  if (TimeDate[1] < 10)
    temp.concat("0");
  temp.concat(TimeDate[1]); //MIN
  temp.concat(":") ;
  if (TimeDate[0] < 10)
    temp.concat("0");
  temp.concat(TimeDate[0]);  // SEC

  *secp = TimeDate[0];
  *minp = TimeDate[1];
  *hrp = TimeDate[2];
  return (temp);
}


void setupTime() {
  int MM = 0, DD = 0, YY = 0, hh = 0, mm = 0, ss = 0;
  Serial.println(F("\nSet time and date in this format: MM,DD,YY,hh,mm,ss"));
  delay (5000);

  while (!Serial.available()) {}
  if (Serial.available()) {
    MM  = Serial.parseInt();
    DD = Serial.parseInt();
    YY = Serial.parseInt();
    hh = Serial.parseInt();
    mm = Serial.parseInt();
    ss = Serial.parseInt();
  }

  Serial.print(F("Recognized: "));
  Serial.print(MM);
  Serial.print(F("/"));
  Serial.print(DD);
  Serial.print(F("/"));
  Serial.print(YY);
  Serial.print(F(","));
  Serial.print(hh);
  Serial.print(F(":"));
  Serial.print(mm);
  Serial.print(F(":"));
  Serial.print(ss);

  SetTimeDate(DD, MM, YY, hh, mm, ss);
  Serial.println(F("Time now is: "));
  Serial.println(ReadTimeDate(&lastSec, &lastMin, &lastHr));
}

String sendMessage() {
    altserial.listen();
    delay(500);

    
    //dataXB.toCharArray(data,sizeof(dataXB));
  
    ch_int();
  
    delay(4000);
    Serial.println(F("Start"));
    length=strlen(start);
    
    exc=length%paylength;
    parts=length/paylength;
    Serial.print(F("length="));
    Serial.println(length);
    Serial.print(F("parts="));
    Serial.println(parts);
    Serial.print(F("excess="));
    Serial.println(exc);
  
 

      
    for (i=0;i<parts+1;i++){
      skip=0;
      payload[0]=(uint8_t)id;
      /*if (i==parts){
        for (j=1;j<exc+1;j++){        
          payload[j]=(uint8_t)start[datalen+j-1];
        }
        payload[exc+1]=(uint8_t)end_id[0];
        datalen=datalen+80;
      }
      else {*/
        for (j=1;j<XBLEN-1;j++){        //-2 kasi id tsaka ^
          payload[j]=(uint8_t)start[datalen+j-1];
        }
        payload[XBLEN-1]=(uint8_t)end_id[0];
        datalen=datalen+80;
      //}
      
      Serial.println(F("sending before xbee.send"));
    
      xbee.send(zbTx);
      Serial.println(F("sending"));
      
      for (j=0;j<XBLEN+1;j++){
        payload[i]=(uint8_t)'0';
      }
      /*************ERROR CHECKS**************/
      Serial.println(F("Packet sent"));

    // after sending a tx request, we expect a status response
    // wait up to half second for the status response
      if (xbee.readPacket(1000)) {
        // got a response!
        Serial.println(F("Got a response!"));
      


        // should be a znet tx status               
        if (xbee.getResponse().getApiId() == ZB_TX_STATUS_RESPONSE) {
          xbee.getResponse().getZBTxStatusResponse(txStatus);

          // get the delivery status, the fifth byte
          if (txStatus.getDeliveryStatus() == SUCCESS) {
            Serial.println(F("Success!"));
          //success means nareceive ni coordinator
            if (verify_send[i] == 0){
              count_success=count_success+1;
              verify_send[i]=1;
              if (count_success==parts+1){
              }
            }
          } 
          else {
            // the remote XBee did not receive our packet. is it powered on?
            //flashLed(errorLed, 6, 500);
            Serial.println(F("myb no pwr"));
          }
        } 
        else{
        }

      } 
      else if (xbee.getResponse().isError()) {
        //nss.print("Error reading packet.  Error code: ");  
        //nss.println(xbee.getResponse().getErrorCode());
        Serial.println(F("Error1"));
      } 
      else {
        // local XBee did not provide a timely TX Status Response -- should not happen
        //flashLed(errorLed, 2, 50);
        Serial.println(F("Error2"));
      }
    /***************************************/

  }


  delay(3000);
  return "OK";
}


void getData() {
  
  customDue.listen();
  delay(500);
  Serial.println(F("Turning ON CustomDue "));
  streamBuffer=(char *) malloc(250);
  //dataXB="testing testy test^";
  
  //ask Zhey kung pwede ganito magpasa ng data
  //sprintf(dataXB,"testing testy test^",sizeof("testing testy test^"));
  //Serial.println(dataXB);
  //sendMessage();
 
  //sendMessage("testing xbee-power module code");
  //Serial.println("Sending this message: testing xbee-power module code");
  digitalWrite(trigForDue, HIGH);
  delay(5000);
  timestamp= ReadTimeDate(&lastSec, &lastMin, &lastHr);
  customDue.write(command);
  customDue.write("/");
  customDue.println(timestamp);
  
  Serial.print(command);
  Serial.print(F("/"));
  Serial.println(timestamp);
  delay(5000); //di pa sure

  //timestart= millis();

  while ( globalChecker == 0 ) {
    t.update();
    //Serial.println(F("Inside globalchecker"));
    if (customDue.available()){
      Serial.println(F("CD is available"));
      customDue.readBytesUntil('\n',streamBuffer,250);
    }
    
    //Serial.println(streamBuffer);

    if (strstr(streamBuffer, "ARQWAIT")) {
      Serial.println(F("Timer Reset"));
      t.stop(x);
      x = t.every(60000, printna);
      streamBuffer[0] = '\0';
      
    }
    
    else if (strstr(streamBuffer, "ARQSTOP")) {
      Serial.println(F("tapos na"));
      t.stop(x);
      streamBuffer[0] = '\0';      
    }
    
    else if (strstr(streamBuffer, ">>")) {
      t.stop(x);

      if (streamBuffer[8]=='#')
        loopnum = streamBuffer[7] - '0';
      else{
        loopnum = ((streamBuffer[7] - '0')* 10) + (streamBuffer[8] - '0');
      }
      partnum = streamBuffer[5] - '0';
      Serial.println(F("loopnum:"));
      Serial.println(loopnum);
      Serial.println(streamBuffer);
      //char *start = strstr(streamBuffer, "#");
      start = strstr(streamBuffer, "#");
      start = start + 1;
      int len = strlen(start);
      start[len - 3] = '\0';
      Serial.println(start);
      Serial.print(F("start size: "));
      Serial.println(sizeof(start));
      //free(streamBuffer);
      //sprintf(streamBuffer, );
      streamBuffer[0] = '\0';
      //sendMessage edit
      sendMessage();
      customDue.write("OK");


      if (loopnum > 1)  {
        
        Serial.println(F("loopnum:"));
        Serial.println(loopnum);

        for (i = 0; i < loopnum - 3; i++) {
          customDue.flush();
          customDue.listen();
          delay(500);
          Serial.println(F("I:"));
          Serial.println(i);
          streamBuffer[0] = '\0';
          
          
          while (!customDue.available());
          if (customDue.available()) {
            customDue.readBytesUntil('\n', streamBuffer, 250);
            Serial.println(F("Listen successful"));
            Serial.println(streamBuffer);
          }
          //if (customDue.available()){
            //  customDue.readBytesUntil('\n',streamBuffer,250);
          //}
    
          
          Serial.println(streamBuffer);

          if (strstr(streamBuffer, ">>")) {
            Serial.println(streamBuffer);
            char *start = strstr(streamBuffer, "#");
            start = start + 1;
            int len = strlen(start);
            start[len - 3] = '\0';
            Serial.println(start);
            Serial.println(F("Tin"));
            //sprintf(streamBuffer, "");
            //sendMessage edit
            sendMessage();
            customDue.write("OK");
            //sendMessage(start);
            streamBuffer[0] = '\0';
            //store unsent data
            if (partnum + 1 == loopnum) {
              loopnum = 0;
              globalChecker = 1;
            }
          }
        }
      }

      else {
        Serial.println(F("done"));
        //dataXB="";
        
        //sprintf(dataXB,"",sizeof(""));
        digitalWrite(trigForDue, LOW);
      }
    }
  }
  free(streamBuffer);
  Serial.println(F("TIME OUT"));
 // free(streamBuffer);
 return;
}

void printna() {

  Serial.println(F("No data from Senslope"));
  //sendMessage edit
  sendMessage();
  customDue.write("OK");
  //sendMessage("No data from Senslope");
  digitalWrite(trigForDue, LOW);
  t.stop(x);
  globalChecker = 1;
  breaker = 1;
  
  return;
}


