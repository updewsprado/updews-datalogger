/*
  / power supply module: router side
  / DEPLOYED IN MARIRONG
  / Pinaganda ko lang, inayos yung mga indent tapos binura yung mga naka-comment
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
#define PAYLEN 80

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
const int rv = A0; //read voltage reading
const int trigForDue = 7; //enable due switch
const int en = 5; //enable due switch?
const int getDataFlag = 0; //0 default; 1 if ready na

char *command = "ARQCMD6T";

int loopnum = 0;
int partnum = 0;
String timestamp = "";

int globalChecker = 0;
uint8_t sleep_period = 2;       // the sleep interval in minutes between 2 consecutive alarms

int lastSec, lastMin, lastHr;

Timer t;
int x = 0;
 
int breaker = 0;

char streamBuffer[250];

int exc=0;
int parts=0;
unsigned char i=0;
unsigned char j=0;
int length=0;
int paylength=77;
int datalen=0;

int count_success=0;
int verify_send[24]={0};

int xbFlag=0;


// SH + SL Address of receiving XBee
XBeeAddress64 addr64 = XBeeAddress64(0x0013a200, 0x40E2DE9B);  //Coordinator
ZBTxRequest zbTx = ZBTxRequest(addr64, payload, sizeof(payload));
ZBTxStatusResponse txStatus = ZBTxStatusResponse();


#include <SPI.h>

void setup() {

	// put your setup code here, to run once:
	Serial.begin(BAUD);
	customDue.begin(BAUD);
	altserial.begin(9600);
	xbee.begin(altserial);

	RTC_init();
	DS3234_init(cs, DS3234_INTCN); //nagawa na
	DS3234_clear_a2f(cs);
	Serial.println(ReadTimeDate(&lastSec, &lastMin, &lastHr));
	pinMode(sv, OUTPUT);
	pinMode(trigForDue, OUTPUT);
	pinMode(en, OUTPUT);
	Serial.println(F("STARTING"));
}

void loop() {
	altserial.listen();
	delay(500);
  
	//getData();		//for testing without xbee
	getXBFlag();
	if (xbFlag == 1){
    
		getData();
		xbFlag=0;    
	}

	if (Serial.find("PM")) {
		if ((char)Serial.read() == '+') {
			Serial.readBytesUntil('+', code, 2);

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

	//20201231 23:55:00
	//160129235500 
	temp.concat(TimeDate[6]); // YEAR
	//temp.concat("-") ;
	if (TimeDate[5] < 10)
		temp.concat("0");
	temp.concat(TimeDate[5]); //MONTH
	//temp.concat("-") ;
	if (TimeDate[4] < 10)
		temp.concat("0");
	temp.concat(TimeDate[4]); //DAY
	//temp.concat(" ") ;
	if (TimeDate[2] < 10)
		temp.concat("0");
	temp.concat(TimeDate[2]); // HH
	//temp.concat(":") ;
	if (TimeDate[1] < 10)
		temp.concat("0");
	temp.concat(TimeDate[1]); //MIN
	//temp.concat(":") ;
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
	Serial.print(F("length="));
	Serial.println(length);
	Serial.print(F("parts="));
	Serial.println(parts);
	Serial.print(F("excess="));
	Serial.println(exc);
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
					if (verify_send[i] == 0){
						count_success=count_success+1;
						verify_send[i]=1;
						if (count_success==parts+1){
						}
					}	
				} 
				else {
					// the remote XBee did not receive our packet. is it powered on?
					Serial.println(F("myb no pwr"));
				}
			} 
			else{
			}

		} 
		else if (xbee.getResponse().isError()) {
			Serial.println(F("Error1"));
		} 
		else {
			// local XBee did not provide a timely TX Status Response -- should not happen
			// but happens because 	
			Serial.println(F("Error2"));
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
	//customDue.write("/");
	customDue.println(timestamp);
  
	Serial.print(command);
	//Serial.print(F("/"));
	Serial.println(timestamp);
	
	
	char Ctimestamp[12] = "";
            
	for (i=0; i<11; i++){
		Ctimestamp[i]= timestamp[i];
	}
            
	Serial.println(Ctimestamp);
    
	while ( globalChecker == 0 ) {
		t.update();
		customDue.listen();
		//customDue.flush();
		//streamBuffer= "";	
		for (i=0; i<250; i++) streamBuffer[i]=0x00;
     
		Serial.println(F("CD is available"));
		customDue.readBytesUntil('\n',streamBuffer,250);


		delay (500);
		Serial.println(streamBuffer);   
		if (strstr(streamBuffer, "ARQWAIT")) {
			Serial.println(F("Ditong streambuffer?"));
			Serial.println(F("Timer Reset"));
			t.stop(x);
			x = t.every(60000, printna);
			streamBuffer[0] = '\0';
		}
          
		else if (strstr(streamBuffer, "ARQSTOP")) {
			//Serial.println(streamBuffer);
			Serial.println(F("tapos na"));
			t.stop(x);
			streamBuffer[0] = '\0';
			globalChecker = 1;      
		}
          
		else if (strstr(streamBuffer, "#")) {    //trash kaya di nya makita yung #
			//Serial.println(streamBuffer);
			Serial.println(F("gettting data"));
			t.stop(x);
			x = t.every(60000, printna);
			Serial.println(streamBuffer);
            
			streamBuffer[strlen(streamBuffer) - 3] = '\0';
            
			strncat(streamBuffer, "*", 1);
			strncat(streamBuffer, Ctimestamp, 12);
			strncat(streamBuffer, "<<", 2);
			Serial.println(streamBuffer);
			sendMessage();
			Serial.println(F("sending ok to custom due"));
			customDue.write("OK");
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
	strcpy(streamBuffer,">>#No data from Senslope<<");
	sendMessage();
	customDue.write("OK");
	digitalWrite(trigForDue, LOW);
	t.stop(x);
	globalChecker = 1;
	breaker = 1;  
	return;
}
