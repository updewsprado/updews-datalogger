#include <SoftwareSerial.h>

/*
  / power supply module
*/
//#include <stdio.h>

SoftwareSerial rPi(10, 11); //Rx, Tx

#define BAUD 9600
char code[10] = "";
#include "ds3234.h"

const int cs= 10; //chip select 
const int sv= 9; //switch for voltage reading
const int al= 2; //interrupt for alarm
//const int rv= A1; //read voltage reading
const int rv= A0; //read voltage reading
const int ed= 5; //enable due switch
const int en= 5; //enable due switch?

//default
uint8_t sleep_period = 2;       // the sleep interval in minutes between 2 consecutive alarms


int lastSec, lastMin, lastHr;

#include <SPI.h>

void setup() {
  // put your setup code here, to run once:
    Serial.begin(BAUD);
    RTC_init();
    DS3234_init(cs, DS3234_INTCN); //nagawa na
    DS3234_clear_a2f(cs);
    regularInterval(sleep_period);
    
//    Serial.println(ReadTimeDate(&lastSec, &lastMin, &lastHr));
    
    pinMode(sv, OUTPUT);
    pinMode(ed, OUTPUT);   
    pinMode(en, OUTPUT);
  }

void loop() {
  // put your main code here, to run repeatedly:
   
   if (DS3234_triggered_a2(cs)) {
            //Serial.println(ReadTimeDate(&lastSec, &lastMin, &lastHr));
//            Serial.println("ALARM");
            set_next_alarm();
            // clear a2 alarm flag and let INT go into hi-z
            DS3234_clear_a2f(cs);          
        }
  
  Serial.readBytesUntil('\r',code,10);
 //Serial.println(code); 
  if (strstr(code, "PM")){
 //Serial.println("recognized PM");    
     if (code[2]== '+'){
    // Serial.println(code[3]);
    switch(code[3]){
    
    case 'S':{ //oks
      //SETUP TIME
  //    Serial.println("Set current time");
      setupTime();
      Serial.println();
      break;
    }
    
    case 'R':{ //oks
      //READ TIME
    //  Serial.println("Read Time");
        Serial.println(ReadTimeDate(&lastSec, &lastMin, &lastHr));
      //Serial.println();
      break;
    }
    
    case 'A':{ //oks
      //SET ALARM 
    //  Serial.println("Set Alarm Interval");
      setAlarmInterval();   
     // Serial.println();
      break;
    }
    
    case 'I':{ //oks
      //READ INTERVAL 
      //Serial.println("Read Current Alarm Interval:");
      Serial.println(sleep_period);
      
      //Serial.println();
      break;
    }
    
    case 'D':{ //oks
      //SHUT DOWN
      //Serial.println("Shut down custom due");
      digitalWrite(ed, LOW);// or high? depends
      //Serial.println();
      break;
    }
    
    case 'V':{ //oks
      //VOLTAGE MONITOR
      Serial.println("Read voltage: ");
      digitalWrite(sv, HIGH);
      float voltage = (analogRead(A0)*(14.0 / 1023.0)); //mapped with 14V as maximum
      Serial.println(voltage);
      digitalWrite(sv, LOW);
      Serial.println();
      break;
    }
    
    }
    }
    else {
      Serial.println("OK");
    }
     
  } 
    delay(1000);
}


int RTC_init(){ 
	
	/*added code for changing stuff */
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
String ReadTimeDate(int *secp, int *minp, int *hrp){
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

//2020-12-31 23:55:00
  temp.concat("20");
  temp.concat(TimeDate[6]); // YEAR
  temp.concat("-") ;
  if (TimeDate[5] <10)
    temp.concat("0");
  temp.concat(TimeDate[5]); //MONTH 
  temp.concat("-") ;
  if (TimeDate[4] <10)
    temp.concat("0");
  temp.concat(TimeDate[4]); //DAY
  temp.concat(" ") ;
  if (TimeDate[2] <10)
    temp.concat("0");
  temp.concat(TimeDate[2]); // HH
  temp.concat(":") ;
  if (TimeDate[1] <10)
    temp.concat("0");
  temp.concat(TimeDate[1]); //MIN
  temp.concat(":") ;
  if (TimeDate[0] <10)
    temp.concat("0");
  temp.concat(TimeDate[0]);  // SEC

        *secp= TimeDate[0];
        *minp= TimeDate[1];
        *hrp= TimeDate[2];
        //Serial.println(temp);
  return(temp);
}

void set_next_alarm(void){
  
    struct ts t;
    unsigned char wakeup_min;

    DS3234_get(cs, &t);

    //calculate the minute when the next alarm will be triggered
    wakeup_min = (t.min / sleep_period + 1) * sleep_period;
    
    //wakeup_min = (t.min + sleep_period);
    
    if (wakeup_min > 59) {
        wakeup_min -= 60;
    }
      
    //Serial.println(wakeup_min);
    
    // flags define what calendar component to be checked against the current time in order
    // to trigger the alarm
    // A2M2 (minutes) (0 to enable, 1 to disable)
    // A2M3 (hour)    (0 to enable, 1 to disable) 
    // A2M4 (day)     (0 to enable, 1 to disable)
    // DY/DT          (dayofweek == 1/dayofmonth == 0)
    uint8_t flags[4] = { 0, 1, 1, 1 };

    // set Alarm2. only the minute is set since we ignore the hour and day component
    DS3234_set_a2(cs, wakeup_min, 0, 0, flags);

    // activate Alarm2
    DS3234_set_creg(cs, DS3234_INTCN | DS3234_A2IE);
}

void regularInterval (int interval){
    int sec1, min1, hr1, min2;
    
    //Serial.println(ReadTimeDate(&sec1, &min1, &hr1)); // an example string
    
    min2 = min1;
      while (min2%interval != 0){
        ReadTimeDate(&sec1, &min1, &hr1); // an example string
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
       Serial.print("Recognized: ");
       Serial.print(MM);
       Serial.print("/");
       Serial.print(DD);
       Serial.print("/");
       Serial.print(YY);
       Serial.print(",");
       Serial.print(hh);
       Serial.print(":");
       Serial.print(mm);
       Serial.print(":");
       Serial.print(ss);
       
       SetTimeDate(DD, MM, YY,hh,mm,ss);
       Serial.println("Time now is: ");
       Serial.println(ReadTimeDate(&lastSec, &lastMin, &lastHr));
       
}

void setAlarmInterval(){
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
