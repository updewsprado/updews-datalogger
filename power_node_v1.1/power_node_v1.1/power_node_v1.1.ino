/*
  / power supply module: router side
*/
//#include <stdio.h>


#define BAUD 9600
char code[2] = "";

#include "ds3234.h"
#include "Timer.h"

#include <XBee.h>
//#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <elapsedMillis.h>
#include <SoftwareSerial.h>
//#include <MemoryFree.h>

#define XBLEN 83 //paylenght+2(identifier)+3(randnum)+1(null)

SoftwareSerial customDue(10, 11); //Rx, Tx
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
const int al = 2; //interrupt for alarm
//const int rv= A1; //read voltage reading
const int rv = A0; //read voltage reading
const int trigForDue = 7; //enable due switch
const int en = 5; //enable due switch?
int getDataFlag = 0; //0 default; 1 if ready na

char *command = "ARQCMD6T";

char* streamBuffer = NULL;

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
int i=0;
int j=0;
int skip=0;
int length;
int paylength=77;
char *tosend[2];
String first;
String sec;
char *xbsend[2];
char xsend[XBLEN];
char id[16][3]={"A0","A1","A2","A3","A4","A5","A6","A7","A8","A9","AA","AB","AC","AD","AE","AF"};
//char id[16][3]={"B0","B1","B2","B3","B4","B5","B6","B7","B8","B9","BA","BB","BC","BD","BE","BF"};
char end_id[]="^";
char *dummy;

int randummy;
char randchar[4]="222";
int ran_len;
char pad[10];

int interval = 20000;

int count_success=0;
int verify_send[24]={0};
char *data;

int xbFlag=0;
char dataXB[160];
/*********************/

// SH + SL Address of receiving XBee
XBeeAddress64 addr64 = XBeeAddress64(0x0013a200, 0x40BAD1D0);  //Coordinator
//XBeeAddress64 addr64 = XBeeAddress64(0x0013a200, 0x40bf920e);  //R1
//XBeeAddress64 addr64 = XBeeAddress64(0x0013a200, 0x40309ef7);  //R2
ZBTxRequest zbTx = ZBTxRequest(addr64, payload, sizeof(payload));
ZBTxStatusResponse txStatus = ZBTxStatusResponse();

//int pin5 = 0;

int statusLed = 13;
int errorLed = 13;
int dataLed = 13;
/*
void flashLed(int pin, int times, int wait) {

  for (int i = 0; i < times; i++) {
    digitalWrite(pin, HIGH);
    delay(wait);
    digitalWrite(pin, LOW);

    if (i + 1 < times) {
      delay(wait);
    }
  }
}*/


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
  
  Serial.println(F("STARTING..."));
  
  pinMode(statusLed, OUTPUT);
  pinMode(errorLed, OUTPUT);
  pinMode(dataLed,  OUTPUT);
}

void loop() {
  // put your main code here, to run repeatedly:

  //if (getDataFlag == 1 ) { //na receiev na yung flag mua sa xbee :o
  //  getData();
  //}
  
  getXBFlag();
  if (xbFlag == 1){
    
    getData();
    xbFlag=0;
    
    for(i=0;i<3;i++){
      free(tosend[i]);
      free(xbsend[i]);
    }
    free(dummy);
    free(data);
    //free(payload);
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
  for (int i = 0; i <= 6; i++) {
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
  for (int i = 0; i <= 6; i++) {
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

    //data="xbee-power module code";
    data=(char *) malloc(160);
    sprintf(data,dataXB,sizeof(dataXB));
    //dataXB.toCharArray(data,sizeof(dataXB));
    //sprintf(data,"xbee-power module code. Here comes the sun Here comes the sun^" );
    Serial.println(data);
    
    //dataXB.toCharArray(data,sizeof(dataXB));
  
    ch_int();
  
    delay(4000);
 
    Serial.println(data);  
    Serial.println(F("Start"));
    length=strlen(data);
    
    exc=length%paylength;
    parts=length/paylength;
    Serial.print(F("length="));
    Serial.println(length);
    Serial.print(F("parts="));
    Serial.println(parts);
    Serial.print(F("excess="));
    Serial.println(exc);
    
    Serial.println(F("TESTING"));
      
    String str(data);
    
    
    skip=0;
    
    for(i=0; i<parts;i++){
      //memset(tosend[i], '\0', sizeof(tosend[i]));
      tosend[i]=(char *) malloc(XBLEN);
      sprintf(tosend[i],data+skip,paylength);
      //tosend[i][paylength-1]='\0';
      Serial.println(tosend[i]);
      skip=skip+paylength;
    }
   
    //memset(tosend[i], '\0', sizeof(tosend[i]));
    tosend[i]=(char *) malloc(XBLEN);
    sprintf(tosend[i],data+skip,exc);
    Serial.println(tosend[i]);
  
    //add identifier (A1, A2, etc)
    for(i=0;i<parts+1;i++){
      Serial.println(F("Hello id"));
      
      //memset(dummy,'/0',sizeof(dummy));
      
      dummy=(char *) malloc(XBLEN);
      sprintf(dummy,id[0],2);
      Serial.println(dummy);
      //memcpy(dummy+sizeof(id[i]),"111",sizeof("111"));
      strcat(dummy, randchar);
      Serial.println(dummy);
      strcat(dummy,tosend[i]);
      Serial.println(dummy);
      xbsend[i]=(char *) malloc(XBLEN);
      sprintf(xbsend[i],dummy,sizeof(dummy)); 
      //strcpy(xbsend[i],tosend[i]);
      Serial.println(xbsend[i]);
      Serial.println(F("hello id end"));
      
    }
    
    for (i=0;i<parts+1;i++){
    memcpy(payload,xbsend[i],XBLEN);
    Serial.println(xbsend[i]);
    xbee.send(zbTx);

    /*************ERROR CHECKS**************/
    Serial.println(F("Packet sent"));

    // flash TX indicator
    //flashLed(statusLed, 1, 100);

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
          // success.  time to celebrate
          //flashLed(statusLed, 5, 50);
          Serial.println(F("Success!"));
          //lcd.clear();
          //lcd.setCursor(0,0);
          //lcd.print("packet ");
          //lcd.print(i);
          //lcd.print(" sent.");
          if (verify_send[i] == 0){
            //lcd.setCursor(0,1);
            //lcd.print("                    ");
            count_success=count_success+1;
            //lcd.setCursor(0,1);
            //lcd.print(count_success);
            verify_send[i]=1;
            if (count_success==parts+1){
              //lcd.setCursor(0,3);
              //lcd.print("COMPLETE ");
            }
           //lcd.setCursor(0,2);
           //lcd.print("                    ");
           //lcd.setCursor(0,2);
           //lcd.print(randchar);
          }
          //lcd.setCursor(0,2);
          //lcd.print(verify_send[i]);

        } 
        else {
          // the remote XBee did not receive our packet. is it powered on?
          //flashLed(errorLed, 6, 500);
          Serial.println(F("The remote XBee did not receive our packet. is it powered on?"));
        }
      } 
      else{
        Serial.println(F("Dunno wat happened huhu."));
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

  Serial.println(F("End of loop"));

  if (timeElapsed > 180000) {
    {				
      ch_int();
      timeElapsed = 0;			 // reset the counter to 0 so the counting starts over...
    }

  }
  //interval=120000;

  delay(3000);
 //////////////////////////////// 
  return "OK";
}


void getData() {
  Serial.println(F("Turning ON CustomDue "));
  //dataXB="testing testy test^";
  
  //ask Zhey kung pwede ganito magpasa ng data
  //sprintf(dataXB,"testing testy test^",sizeof("testing testy test^"));
  //Serial.println(dataXB);
  //sendMessage();
 
  //sendMessage("testing xbee-power module code");
  //Serial.println("Sending this message: testing xbee-power module code");
  digitalWrite(trigForDue, HIGH);
  timestamp= ReadTimeDate(&lastSec, &lastMin, &lastHr);
  customDue.print(command);
  customDue.print("/");
  customDue.println(timestamp);
  Serial.print(command);
  Serial.print(F("/"));
  Serial.println(timestamp);
  delay(5000); //di pa sure

  //timestart= millis();

  while ( globalChecker == 0 ) {
    t.update();
    Serial.println(F("Inside globalchecker"));
    if (customDue.available()){
      Serial.println(F("CD is available"));
      customDue.readBytesUntil('\n',streamBuffer,250);
      
    }
    
    //Serial.println(streamBuffer);

    //if (Serial.available()) {
      //Serial.readBytesUntil('\n', streamBuffer, 250);
    //}

    if (strstr(streamBuffer, "ARQWAIT")) {
      Serial.println(F("Timer Reset"));
      t.stop(x);
      x = t.every(60000, printna);
      sprintf(streamBuffer, "");
    }

    else if (strstr(streamBuffer, ">>")) {
      t.stop(x);


      loopnum = streamBuffer[7] - '0';
      partnum = streamBuffer[5] - '0';
      Serial.println(F("loopnum:"));
      Serial.println(loopnum);
      Serial.println(streamBuffer);
      char *start = strstr(streamBuffer, "#");
      start = start + 1;
      int len = strlen(start);
      start[len - 3] = '\0';
      Serial.println(start);
      sprintf(streamBuffer, "");
      //sendMessage edit
      sprintf(dataXB,start,sizeof(start));
      sendMessage();
      //sendMessage(start);
      //store unsent data- next time na to
      sprintf(streamBuffer, "");

      if (loopnum > 1)  {

        Serial.println(F("loopnum:"));
        Serial.println(loopnum);
        int i = 0;

        for (i = 0; i < loopnum - 3; i++) {
          Serial.println(F("I:"));
          Serial.println(i);

          while (!Serial.available());
          if (Serial.available()) {
            Serial.readBytesUntil('\n', streamBuffer, 250);
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
            sprintf(streamBuffer, "");
            //sendMessage edit
            sprintf(dataXB,start,sizeof(start));
            sendMessage();
            //sendMessage(start);
            sprintf(streamBuffer, "");
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
      }
    }
  }

  Serial.println(F("TIME OUT"));

}

void printna() {

  Serial.println(F("No data from Senslope"));
  //sendMessage edit
  sprintf(dataXB,"No data from Senslope",sizeof("No data from Senslope"));
  sendMessage();
  //sendMessage("No data from Senslope");
  digitalWrite(trigForDue, LOW);
  t.stop(x);
  globalChecker = 1;
  breaker = 1;
}


