/*

  *ARQ Emulation in Taijuino Due
  *Jo Hanna Lindsey Serato
  *Created: 10/29/2015
  *Updated: 10/04/2016
  
update:
//

send message na yung itetest ko huehue
GSM RESPONSE!!!


tsaka yung over the air! :/
yung pagbasa ng reply lagi ng gsm module: ayusin si gsm response
rain gaugeng
*/

#include "Timer.h"
#include <DueFlashStorage.h>

#define customDue Serial2
#define powerM Serial3
#define gsmM Serial1
#define trigForDue 12
#define powerKey 13

//defaults

bool debugFlag= false;
char command[9] = "ARQCMD6T";
char serverNum[12] = "09153294417";
String timestamp = "";
char streamBuffer[250] = "";

int timestart = millis();
int timenow = millis();
char* messageStr = NULL;
int globalChecker = 0;

Timer t;
int x = 0;
int d = 0;
int breaker = 0;

int retryForSD=0;

DueFlashStorage dueFlashStorage;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  gsmM.begin(9600); //GSM
  customDue.begin(9600); //customDue
  powerM.begin(9600); //green board

  gsmInitialization();

  pinMode(trigForDue, OUTPUT);
  digitalWrite(trigForDue, LOW);

  Serial.println("Taijuino turned on");
  Serial.println("Sampling data in 5 seconds. Type in 'debug' to enter in debug mode.");
  setDefault();
  
  timestart = millis();

  while ( timenow - timestart < 5000 ) {
    timenow = millis();

    if (Serial.available()) {
      debugFlag= true;
      Serial.println("Waiting for commands... DBGCMD9 TO EXIT");
      
      displaymenu();
      while (debugFlag== true){
        menu();
      }
      delay(1000);
    }//IF SERIAL.AVAILS
  }

  x = t.every(60000, printna); //whatever happens, mag po-poll pa din //okay sana na merong magbabalik sa menu
}

void loop() {
  
Serial.println(F("Turning ON CustomDue "));
          
        char *start= "";  
        digitalWrite(trigForDue, HIGH);
	delay(10000);
	timestamp= getTimestamp();
	customDue.write(command);
	customDue.println(timestamp);
	Serial.print(command);
	Serial.println(timestamp);
	
	
	char Ctimestamp[12] = "";
            
	for (int i=0; i<12; i++){
		Ctimestamp[i]= timestamp[i];
	}
        Ctimestamp[12]= '\0';    
	
        Serial.println(Ctimestamp);
    
	while ( globalChecker == 0 ) {
		t.update();
		
		for (int i=0; i<250; i++) streamBuffer[i]=0x00;
     
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
			Serial.println(F("tapos na"));
			t.stop(x);
                        customDue.write("OK");
	                digitalWrite(trigForDue, LOW);			
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
                           //getData();
                           loop();
                        }
                        
			Serial.println(F("gettting data"));
			t.stop(x);
			//x = t.every(60000, printna);
			Serial.println(streamBuffer);
            
			streamBuffer[strlen(streamBuffer) - 3] = '\0';
            
			strncat(streamBuffer, "*", 1);
			strncat(streamBuffer, Ctimestamp, 12);
			start= streamBuffer + 5; // 

                        Serial.println(start);
			for (int i=0; i<5; i++){
                              if (sendMessage(start)== 1){
                              Serial.println("Message sent successfully");
                              break;
                        }
        //break if message is successfully sent 
        delay (1000);
      }
			Serial.println(F("sending ok to custom due"));
			customDue.write("OK");
		}
	}
	digitalWrite(trigForDue, LOW);
	globalChecker=0;
	Serial.println(F("TIME OUT"));
	return;
}  
	


void gsmInitialization() {
  //if status pin is
  /*powerkey turning chenes*/
  char *res= "";
  Serial.println("GSM Initialization");
  
  if (gsmStatus() == 0)
    power_ON_Down();
  delay(5000);
  
  Serial.println("Checking AT command");
  gsmM.println("ATQ1"); // para may carriage return sila
  Serial.println("ATQ1");
  //Serial.println(gsmResponse());
 
  gsmM.println("ATE0");
  Serial.println("ATE0");
 // Serial.println(gsmResponse());

  gsmM.println("AT");
  Serial.println("AT");
  Serial.println(gsmResponse());
  
  if (strstr(res, "OK")) {
  Serial.println("yey");
  }  //gsmM.println( "AT+CMGF=1" );
    //gsmM.println("ATE0"); //turn on echo- 1; turn off 0
 //   Serial.println("GSM initialized.");

//    simAvailable();
  //  checkNetwork();
   // checkSignal();
//  }
  //if nag reply ng ok, do the ff:

}

int sendMessage(char *message) {
  Serial.println("fxn: sendMessage");
  char *numStr = NULL;
  numStr = (char *) calloc(100, sizeof(char *));
  sprintf(numStr, "AT+CMGS=\"%s\",129", serverNum);
  gsmM.println(numStr);
  while (!gsmM.available());

  while ((char)gsmM.read() != '>') {}

  gsmM.print(message);
  delay(500);
  gsmM.write(0x1A);
  
  Serial.println("Message sent");
  //lagyan ng checker kung nagsend ba successfully
 return 1;// if okay
 //return 0// if di okay
}

String getTimestamp() {
  Serial.println("getting timestamp na");
  char timestamp[20]= "";
  
  powerM.println("PM+R");
  delay(500);
  
  timestart = millis();
  timenow = millis();
  while (!powerM.available()){
    while ( timenow - timestart < 5000 ) {
      timenow = millis();
    }
      return "xxxxxxxxxxxx";
  }
  
  powerM.readBytesUntil('\n', timestamp, 20);
  Serial.println(timestamp);
  timestamp[strlen(timestamp)] = 0x00;
  Serial.println(timestamp);
  return timestamp;

}

void printna() {

  Serial.println("Time out. Custom Due not responding.");
  //send message na di naprint, kalungkutan huhuhuh
  t.stop(x);
  digitalWrite(trigForDue, LOW);
  globalChecker = 1;
  breaker = 1;
}


void power_ON_Down() {
  pinMode(13, OUTPUT);
  Serial.println("Turning on GSM module");
  digitalWrite(13, LOW);
  delay(1000);
  digitalWrite(13, HIGH);
  delay(2000);
  digitalWrite(13, LOW);
  delay(3000);
  Serial.println("waiting. . . ");
  delay(5000);
}

char *gsmResponse() {
  char* response= "";
  
  gsmM.flush();
  while (!gsmM.available()){}
    gsmM.readBytesUntil('\r', response, 50);
    Serial.println(response);
    
  return response;
}

int gsmStatus() {
  pinMode(13, INPUT);

  if (digitalRead(13) == HIGH) {
    Serial.println("GSM module already turned on..");
    return 1;
  }
  else {
    Serial.println("GSM module not yet turned on..");
    return 0;
  }

}

void checkSignal() {
  gsmM.println( "AT+CSQ?");  //
//  char* response = gsmResponse();
  
//  Serial.println(response);

//  if (strstr(response, "+CSQ")) {
  //  Serial.print("Signal:");
  //  Serial.print(response[6]);
  //  Serial.println(response[7]);
  //  Serial.println();
  //  Serial.println("99 shows no signal; at least 15;");
 // }

  //ber shouldnt be in 99; 99 is not known or not detectable


}

void checkNetwork() {

  //check network
  gsmM.println( "AT+CREG?");  //check yung
  Serial.print("CREG ");
  Serial.print("response: ");

//  char* response = gsmResponse();
//  Serial.println(response);
//
  //if (strstr(response, "+CREG")) {
    //save data from pointer 6 lagay sa string, tas i print
    //if ok
    Serial.print("Network:");
    Serial.println("network");
    //else
    Serial.println("error");
//  }


}

/*void simAvailable() {
  //check if sim card is available
  gsmM.println( "AT+CSMINS");  //check if simcard is available
  Serial.print("CSMINS ");
  Serial.print("response: ");
  char* response = gsmResponse();
  Serial.println(response);

  if (strstr(response, "+CSMINS")) {
    if (response[11] == '1') {
      Serial.println("SIM card available");
    }
    else if (response[11] = '0') {
      Serial.println("SIM card is not available");
    }
    else {
      Serial.println("error");
    }
  }
  //available: +CSMINS: 0,1
  //not available: +CSMINS: 0,0
}*/

void c_serverNumber(char* number) { //okay 
  for (d = 0; d < 11; d++) {
    dueFlashStorage.write(d, number[d]);
  }
}

void c_columnName(char* cname) {
  //5 slots 11-15
  //8 slots for piezo 11-18
  for (d = 0; d < 5; d++) {
    dueFlashStorage.write(d + 11, cname[d]);
  }
}

void c_report_Interval(int x) {
  //1 //
  dueFlashStorage.write(19, x);
}

void c_command(char * com) {
  //8 command 20-27
  for (d = 0; d < 8; d++) { //di pa nachecheck
    dueFlashStorage.write(d + 20, com[d]);
  }

}

void c_piezo(int x) {
  //28
  dueFlashStorage.write(28, x);
}

String r_serverNumber() {
String number = "";
   for (d = 0; d < 11; d++) {
    number.concat((char)dueFlashStorage.read(d));
   }
   number.concat('\0');
  return number;
}

String r_columnName() {
  String columnName= "";
  for (d = 0; d < 5; d++) {
    columnName.concat((char)dueFlashStorage.read(d + 11));
  }
  columnName.concat('\0');
  return columnName;
}

int r_report_Interval() {
   return (int)dueFlashStorage.read(19);
}

String r_command() {
  String cmd= "";
  
  for (d = 0; d < 8; d++) { //di pa nachecheck
    cmd.concat((char)dueFlashStorage.read(d + 20));;
  }
  cmd.concat('\0');
  return cmd;
}


void menu() {
  
  char x[2] = {};
  while(!Serial.available());
  Serial.readBytesUntil('\r', x, 2);
 
  switch (x[0]) {
    Serial.flush();
    case '0': { //check network
        displaymenu();
        break; }
    case '1': { //check network
        //exit
        break; }
    case 'B' : { //change sim availability
      //  simAvailable();
        break; }
    case 'C' : { //change servernumber
        char number[11] = {};
        Serial.print("Enter new server number: ");
        Serial.flush();
        while(!Serial.available());
        Serial.readBytesUntil('\r', number, 11);
        number[11]= '\0';
        Serial.println(number);
        c_serverNumber(number);
        Serial.println("Successfully changed.");
        break; }
    case 'D' : { //change column name
        char namem[5]= {};
        Serial.print("Enter new mastername: ");
        Serial.flush();
        while(!Serial.available());
        Serial.readBytesUntil('\r', namem, 5);
        namem[5]= '\0';
        Serial.println(namem);
        c_columnName(namem);
        Serial.println("Successfully changed.");
        break; }
    case 'E' : { //change report interval
        int num = 0;
        Serial.print("Enter report interval: ");
        while(!Serial.available());
        num= Serial.parseInt();
        Serial.println(num);
        c_report_Interval(num);
        Serial.println("Successfully changed.");
        break; }
    case 'F': { //change current command
        char t_command[8] = {};
        Serial.print("Enter new command: ");
        Serial.flush();
        while(!Serial.available());
        Serial.readBytesUntil('\r', t_command, 8);
        t_command[8]= '\0';
        Serial.println(t_command);
        c_command(t_command);
        Serial.println("Successfully changed.");
        break; }
    case 'G': { //change piezo status
        int num = 0;
        Serial.print("Enter new piezo status: ");
        while(!Serial.available());
        num= Serial.parseInt();
        Serial.println(num);
        c_piezo(num);
        //check nalang kung binary lang siya
        Serial.println("Successfully changed.");
        break; }
    case 'H': { //read servernumber
        Serial.println(r_serverNumber());
        break; }
    case 'I': { //read column name
        Serial.println(r_columnName());
        break; }
    case 'J': { //read report interval
        Serial.println(r_report_Interval());
        break; }
    case 'K': { //read current command
        Serial.println(r_command());
        break; }
    case 'L': { //read piezo status
        break; }
    case 'M': {
        break; }
    default: {
        Serial.println("Command not recognized");
        break; }
  }//SWITCH
}

void displaymenu(){
Serial.println("Version 3: Regular");
  Serial.println("Enter letter of your choice: ");
  Serial.println("0: Display menu again");
  Serial.println("1: Exit");
  Serial.println("A: Check Network");
  Serial.println("B: Sim Availability");
  Serial.println("C: Change Server number");
  Serial.println("D: Change Column name");
  Serial.println("E: Change Report interval");
  Serial.println("F: Change Current command");
  Serial.println("G: Change Piezo status");
  Serial.println("H: Read Server number");
  Serial.println("I: Read Column name");
  Serial.println("J: Read Report Interval");
  Serial.println("K: Read Current command");
  Serial.println("L: Read Piezo status");

  Serial.println("Waiting ...");
}

void setDefault(){
  
  c_command(command);  
//  c_columnname(namem);  
  c_serverNumber(serverNum);
  c_report_Interval(30); // 30 mins for the default
  c_piezo(0); //piezo status 0- no piezo, 1- with piezo  
  
}

