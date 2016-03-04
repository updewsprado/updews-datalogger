 /*
  / power supply modulen for regular and coordinator
  Modified: 02/04/2016
  Last uploaded in Magsaysay
*/

#define BAUD 9600
char streamBuffer[5] = "";
#include "ds3234.h"

const int cs= 10; //chip select 
const int sv= 9; //switch for voltage reading
const int al= 2; //interrupt for alarm
const int rv= A0; //read voltage reading
const int ed= 5; //enable due switch
const int en= 5; //enable due switch?
//default
uint8_t sleep_period = 2;       // the sleep interval in minutes between 2 consecutive alarms

#include <SPI.h>

void setup() {
  // put your setup code here, to run once:
    Serial.begin(BAUD);
    RTC_init();
    DS3234_init(cs, DS3234_INTCN); //nagawa na
    DS3234_clear_a2f(cs);
    regularInterval(sleep_period);    
    pinMode(sv, OUTPUT);
    pinMode(ed, OUTPUT);   
    pinMode(en, OUTPUT);
  }

void loop() {
  // put your main code here, to run repeatedly:
   streamBuffer[0]= '\0';
   if (DS3234_triggered_a2(cs)) {
            //Serial.println("ALARM");
            set_next_alarm();
            //clear a2 alarm flag and let INT go into hi-z
            DS3234_clear_a2f(cs);          
        }
  Serial.flush();
  while (!Serial.available());      
  Serial.readBytesUntil('\r', streamBuffer, 5);
  
  if (strstr(streamBuffer,"PM")){
     if (streamBuffer[2] == '+'){
       switch(streamBuffer[3]){
         case 'S':{ //SETUP TIME
            //Serial.println("Set current time");
            setupTime();
            break; }
        case 'R':{ //READ TIME
          Serial.println(ReadTimeDate());
          break; }
        case 'A':{ //SET ALARM INTERVAL 
          setAlarmInterval();   
          break; }
        case 'I':{ //READ INTERVAL 
          Serial.println(sleep_period);
          break; }
        case 'D':{ //SHUT DOWN CUSTOM DUE
          digitalWrite(ed, LOW);// or high? depends
          break; }
        case 'V':{ //VOLTAGE MONITOR
          digitalWrite(sv, HIGH);
          delay(1000);
          float voltage = (analogRead(A0)*(12.5 /1023.0)); //mapped with 14V as maximum
          //float voltage = (analogRead(A0)); //mapped with 14V as maximum
          delay(1000);
          Serial.println(voltage);
          digitalWrite(sv, LOW);
          break; }
      }
    }
    else {
      Serial.println("OK");
    }
     
  } 
    
    delay(1000);
}


int RTC_init(){   
    pinMode(cs,OUTPUT); // chip select
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

int SetTimeDate(int d, int mo, int y, int h, int mi, int s){ 
  SPI.setBitOrder(MSBFIRST);
  SPI.setDataMode(SPI_MODE1); // both mode 1 & 3 should work  	
	
  int TimeDate [7]={s,mi,h,0,d,mo,y};
  for(int i=0; i<=6;i++){
    if(i==3)
      i++;
    int b= TimeDate[i]/10;
    int a= TimeDate[i]-b*10;
    if(i==2){
      if (b==2)
        b=B00000010;
      else if (b==1)
        b=B00000001;
    }
    TimeDate[i]= a+(b<<4);
    digitalWrite(cs, LOW);
    SPI.transfer(i+0x80); 
    SPI.transfer(TimeDate[i]);
    digitalWrite(cs, HIGH);
  }
}
//=====================================
String ReadTimeDate(){
  String temp;
  int TimeDate [7]; //second,minute,hour,null,day,month,year   
  
   SPI.setBitOrder(MSBFIRST);
    SPI.setDataMode(SPI_MODE1); // both mode 1 & 3 should work   
  for(int i=0; i<=6;i++){
    if(i==3)
      i++;
    digitalWrite(cs, LOW);
    SPI.transfer(i+0x00);
	
    unsigned int n = SPI.transfer(0x00);

    digitalWrite(cs, HIGH);
    int a=n & B00001111;    
    if(i==2){ 
      int b=(n & B00110000)>>4; //24 hour mode
      if(b==B00000010)
        b=20;        
      else if(b==B00000001)
        b=10;
      TimeDate[i]=a+b;
    }
    else if(i==4){
      int b=(n & B00110000)>>4;
      TimeDate[i]=a+b*10;
    }
    else if(i==5){
      int b=(n & B00010000)>>4;
      TimeDate[i]=a+b*10;
    }
    else if(i==6){
      int b=(n & B11110000)>>4;
      TimeDate[i]=a+b*10;
    }
    else{ 
      int b=(n & B01110000)>>4;
      TimeDate[i]=a+b*10; 
      }
  }
//        160129235500
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

	return (temp);}

void set_next_alarm(void){
    struct ts t;
    unsigned char wakeup_min;
    DS3234_get(cs, &t);
    wakeup_min = (t.min / sleep_period + 1) * sleep_period;
    if (wakeup_min > 59) {
        wakeup_min -= 60;
    }
    //Serial.println(wakeup_min);
    uint8_t flags[4] = { 0, 1, 1, 1 };
    DS3234_set_a2(cs, wakeup_min, 0, 0, flags);
    DS3234_set_creg(cs, DS3234_INTCN | DS3234_A2IE);
}

void regularInterval (int interval){
    int sec1, min1, hr1, min2;
    min2 = min1;
    while (min2%interval != 0){
        ReadTimeDate(); // an example string
        min2 = min1;
        delay(1000); 
    }
    set_next_alarm();
} 

void setupTime(){
int MM=0,DD=0, YY=0, hh=0, mm=0, ss = 0;      
      Serial.println("\nSet time and date in this format: MM,DD,YY,hh,mm,ss");
      delay (5000);
  
      while (!Serial.available()){}
      if (Serial.available()){
             
         MM  = Serial.parseInt();
         DD = Serial.parseInt();
         YY= Serial.parseInt();
         hh= Serial.parseInt();
         mm= Serial.parseInt();
         ss= Serial.parseInt();
       }       
       
       SetTimeDate(DD, MM, YY,hh,mm,ss);
       Serial.print("Time now is: ");
       Serial.println(ReadTimeDate());
       
}

void setAlarmInterval(){
   Serial.println("Enter interval value");
   while (!Serial.available()){}
   int x = 0;
   if (Serial.available()){
         x = Serial.parseInt();
   }     
       
   Serial.println(x);
       
   if ((x != 2) && (x != 10) && (x != 15) && (x != 30) && (x != 60) )
      Serial.println("Not a valid interval. 2, 10, 15, 30, 60");
   else{ 
          sleep_period = x;
          Serial.print("Successfully changed the alarm interval to ");
          Serial.println(x);
   }

}
