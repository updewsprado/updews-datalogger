/*
  / power supply module: router side
  / DEPLOYED IN MARIRONG
  / Pinaganda ko lang, inayos yung mga indent tapos binura yung mga naka-comment
*/

 
#define BAUD 9600
char code[2] = "";

#include "ds3234.h"
#include "Timer.h"

#include <XBee.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <elapsedMillis.h>
#include <SoftwareSerial.h>

#define XBLEN 83 //paylenght+2(identifier)+3(randnum)+1(null)
#define PAYLEN 80

SoftwareSerial customDue(3, 4); //Rx, Tx
SoftwareSerial altserial(5,6);

XBee xbee = XBee();
XBeeResponse response = XBeeResponse();
// create reusable response objects for responses we expect to handle 
ZBRxResponse rx = ZBRxResponse();
ModemStatusResponse msr = ModemStatusResponse();

elapsedMillis timeElapsed;

uint8_t payload[XBLEN];

short cs = 10; //chip select
short sv = 9; //switch for voltage reading
short rv = A0; //read voltage reading
short trigForDue = 7; //enable due switch
short getDataFlag = 0; //0 default; 1 if ready na

char *command = "ARQCMD6T";

short loopnum = 0;
short partnum = 0;
String timestamp = "";

short globalChecker = 0;
uint8_t sleep_period = 2;       // the sleep interval in minutes between 2 consecutive alarms

int lastSec, lastMin, lastHr;
short retryForSD=0;

Timer t;
short x = 0;

float voltage=-1;
 
char streamBuffer[180];

short exc=0;
short parts=0;
short i=0;
short j=0;
short length=0;
short paylength=77;
short datalen=0;

short count_success=0;
short verify_send[24]={0};

short xbFlag=0;


// SH + SL Address of receiving XBee
XBeeAddress64 addr64 = XBeeAddress64(0x00, 0x00);  //Gateway
//XBeeAddress64 addr64 = XBeeAddress64(0x0013a200, 0x40BAD1D2);  //Coordinator
ZBTxRequest zbTx = ZBTxRequest(addr64, payload, sizeof(payload));
ZBTxStatusResponse txStatus = ZBTxStatusResponse();


#include <SPI.h>

void setup() {
        
	// put your setup code here, to run once:
	Serial.begin(BAUD);
        delay(500);
	customDue.begin(BAUD);
	altserial.begin(BAUD);
	xbee.begin(altserial);

	RTC_init();
	DS3234_init(cs, DS3234_INTCN);
	DS3234_clear_a2f(cs);
	Serial.println(ReadTimeDate(&lastSec, &lastMin, &lastHr));
	pinMode(sv, OUTPUT);
	pinMode(trigForDue, OUTPUT);
	Serial.println(F("STARTING"));
}

void loop() {

	altserial.listen();
	delay(500);
  
	//getData();		//for testing without xbee
        
	getXBFlag();
	if (xbFlag == 1){
		readVoltage();
		sendVoltage();
		getData();

		xbFlag=0;
		
	}
       
	if (Serial.find("PM")) {
		if ((char)Serial.read() == '+') {
			Serial.readBytesUntil('+', code, 2);

			switch (code[0]) {

				case 'P': { //first tried in kibawe
					//Poll for customdue
					getData();
					break;
				}

				case 'S': { //oks
					//SETUP TIME
					Serial.println(F("Set current time"));
					setupTime();
					break;
				}

				case 'R': { //oks
					//READ TIME
					Serial.println(F("Read Time"));
					Serial.println(ReadTimeDate(&lastSec, &lastMin, &lastHr));
					break;
				}

				case 'V': { //oks
					//VOLTAGE MONITOR
					readVoltage();
					break;
				}
  
			}
		}	
		else {
			Serial.println(F("OK"));
		}

	}

	delay(1000);
}

void readVoltage(){
  
	Serial.println(F("Read voltage: "));
	
	digitalWrite(sv, HIGH);
	delay(10000);
	voltage = (analogRead(A0) * (12.5 / 1023.0)); //mapped with 12.5V as maximum
	//voltage = ((((analogRead(A0) * 3.3)/1023)-0.1973)/0.1844);                                                          
	Serial.println(voltage);
	digitalWrite(sv, LOW);

	return;
}

void sendVoltage(){
	String volt;
	volt=(String)voltage;        
	char voltagedummy[10]="";        
	volt.toCharArray(voltagedummy,sizeof(volt));
	for (i=0; i<180; i++) streamBuffer[i]=0x00;
	strcpy(streamBuffer,">>VOLTAGE#");
	strncat(streamBuffer, voltagedummy, sizeof(voltagedummy));
	strncat(streamBuffer, "<<", 2);
	Serial.println(streamBuffer);
	sendMessage();
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

	//20201231 23:55:00
	//160129235500 
	temp.concat(TimeDate[6]); // YEAR
	if (TimeDate[5] < 10)
		temp.concat("0");
	temp.concat(TimeDate[5]); //MONTH
	if (TimeDate[4] < 10)
		temp.concat("0");
	temp.concat(TimeDate[4]); //DAY
	if (TimeDate[2] < 10)
		temp.concat("0");
	temp.concat(TimeDate[2]); // HH
	if (TimeDate[1] < 10)
		temp.concat("0");
	temp.concat(TimeDate[1]); //MIN
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
	
	SetTimeDate(DD, MM, YY, hh, mm, ss);
	Serial.println(F("Time now is: "));
	Serial.println(ReadTimeDate(&lastSec, &lastMin, &lastHr));
}

void sendMessage() {
	// altserial.listen();
	delay(500);
	// delay(4000);
	Serial.println(F("Start"));
	length=strlen(streamBuffer);
	
	exc=length%PAYLEN;
	parts=length/PAYLEN;
	
	datalen = 0;
      
	for (i=0;i<parts+1;i++){
		for (j=0;j<XBLEN+1;j++) payload[j]=0x00;

		delay(500);
     
		for (j=0;j<PAYLEN;j++){
			payload[j]=(uint8_t)streamBuffer[datalen];
			datalen++;
		}

		Serial.println(datalen);
		Serial.println(F("sending before xbee.send"));

		xbee.send(zbTx);
		Serial.println(F("sending"));
      
		//ERROR CHECKS
		Serial.println(F("Packet sent"));

		// after sending a tx request, we expect a status response
		// wait up to half second for the status response
		//altserial.listen();
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
				} 
				else {
					// the remote XBee did not receive our packet. is it powered on?
					Serial.println(F("myb no pwr"));
				}
			} 
		}
	}
	Serial.println(F("exit send"));

	delay(1000);
	return;
}


void getData() { 

	Serial.println(F("Turning ON CustomDue "));

	digitalWrite(trigForDue, HIGH);
	delay(10000);
	timestamp= ReadTimeDate(&lastSec, &lastMin, &lastHr);
	customDue.write(command);
	customDue.println(timestamp);
	
	Serial.print(command);
	Serial.println(timestamp);
	
	char Ctimestamp[12] = "";
	
	for (i=0; i<12; i++){
		Ctimestamp[i]= timestamp[i];
	}
	Ctimestamp[12]= '\0';    
	
	Serial.println(Ctimestamp);
    
	while ( globalChecker == 0 ) {
		t.update();
		customDue.listen();
		
		for (i=0; i<180; i++) streamBuffer[i]=0x00;
		streamBuffer[0] = '\0';
		
		Serial.println(F("CD is available"));
		customDue.readBytesUntil('\n',streamBuffer,180);

		delay (500);
		//Serial.println(streamBuffer);   
		if (strstr(streamBuffer, "ARQWAIT")) {
			Serial.println(F("Timer Reset"));
			t.stop(x);
			x = t.every(60000, printna);
			for (i=0; i<180; i++) streamBuffer[i]=0x00;
			streamBuffer[0] = '\0';
		}
		
		else if (strstr(streamBuffer, "ARQSTOP")) {
			Serial.println(F("tapos na"));
			t.stop(x);
			customDue.println(F("OK"));
			digitalWrite(trigForDue, LOW);			
			for (i=0; i<180; i++) streamBuffer[i]=0x00;
			streamBuffer[0] = '\0';
			globalChecker = 1;
		}
          
		else if (strstr(streamBuffer, "#")) {
			if (strstr(streamBuffer, "SD")){
				if (retryForSD == 10){
					retryForSD= 0;
					t.stop(x);
					globalChecker = 1; 
				}
				
				retryForSD ++;
				digitalWrite(trigForDue, LOW);
				delay(1000);
				getData();
		
			}
			if (strstr(streamBuffer, "<<")){
				Serial.println(F("gettting data"));
				t.stop(x);
				//x = t.every(60000, printna);
				Serial.println(streamBuffer);
            
				streamBuffer[strlen(streamBuffer) - 3] = '\0';
				
				strncat(streamBuffer, "*", 1);
				strncat(streamBuffer, Ctimestamp, 12);
				strncat(streamBuffer, "<<", 2);
				Serial.println(streamBuffer);
				sendMessage();
			}
			else{
				Serial.println(F("Message did not finish"));
			}
			Serial.println(F("sending ok to custom due"));
			customDue.println(F("OK"));
		}
	}

	digitalWrite(trigForDue, LOW);
	globalChecker=0;
	Serial.println(F("TIME OUT"));
	return;
}

void printna() {

	Serial.println(F("No data from Senslope"));
	streamBuffer[0] = '\0';
	strcpy(streamBuffer,">>#NO DATA FROM SENSLOPE<<");
	sendMessage();
	customDue.println(F("OK"));
	digitalWrite(trigForDue, LOW);
	t.stop(x);
	globalChecker = 1;
	return;
}
