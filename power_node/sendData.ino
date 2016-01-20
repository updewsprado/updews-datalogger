void sendDataXB(String dataXB){
  char data[160];
  dataXB.toCharArray(data,sizeof(dataXB));
  
  ch_int();
  
  delay(6000);
  //memset(tosend[0],'/0',sizeof(tosend[0]));
  //memset(tosend[1],'/0',sizeof(tosend[1]));
  //memset(tosend[2],'/0',sizeof(tosend[2]));
  // break down 10-bit reading into two bytes and place in payload
//  pin5 = analogRead(5);
//  payload[0] = pin5 >> 8 & 0xff;
//  payload[1] = pin5 & 0xff;
  
    //String buffer=data;
  Serial.println(data);  
  Serial.println("Start");
  length=strlen(data);
  
  exc=length%paylength;
  parts=length/paylength;
  Serial.print("length=");
  Serial.println(length);
  Serial.print("parts=");
  Serial.println(parts);
  Serial.print("excess=");
  Serial.println(exc);
  
  Serial.println("TESTING");
    
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
  /*
  //memset(tosend[i], '\0', sizeof(tosend[i]));
  strncpy(tosend[i],data+skip,exc);
  
  


  //add identifier (A1, A2, etc)
  for(i=0;i<parts+1;i++){
    memset(dummy,'/0',sizeof(dummy));
    strncpy(dummy,id[i],sizeof(id[i]));
    //memcpy(dummy+sizeof(id[i]),"111",sizeof("111"));
    strcat(dummy, randchar);
    strcat(dummy,tosend[i]);
    strncpy(xbsend[i],dummy,sizeof(dummy)); 
    Serial.println(xbsend[i]);
  }
  
    //xbsend[i]="TESTINGFGSUDUFBUDFHDSAJKEFHHASDJKFHWEUIHFASNFIUAWEHFIAWGAKJHIUDNFASJKHDFGUAYSDEDFB";
    for (i=0;i<parts+1;i++){
    memcpy(payload,xbsend[i],sizeof(xbsend[i]));
    Serial.println(xbsend[i]);
    xbee.send(zbTx);

    //ERROR CHECKS
    Serial.println("Packet sent");

    // flash TX indicator
    flashLed(statusLed, 1, 100);

    // after sending a tx request, we expect a status response
    // wait up to half second for the status response
    if (xbee.readPacket(1000)) {
      // got a response!
      Serial.println("Got a response!");


      // should be a znet tx status               
      if (xbee.getResponse().getApiId() == ZB_TX_STATUS_RESPONSE) {
        xbee.getResponse().getZBTxStatusResponse(txStatus);

        // get the delivery status, the fifth byte
        if (txStatus.getDeliveryStatus() == SUCCESS) {
          // success.  time to celebrate
          flashLed(statusLed, 5, 50);
          Serial.println("Success!");
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
          flashLed(errorLed, 6, 500);
          Serial.println("The remote XBee did not receive our packet. is it powered on?");
        }
      } 
      else{
        Serial.println("Dunno wat happened huhu.");
      }

    } 
    else if (xbee.getResponse().isError()) {
      //nss.print("Error reading packet.  Error code: ");  
      //nss.println(xbee.getResponse().getErrorCode());
      Serial.println("Error1");
    } 
    else {
      // local XBee did not provide a timely TX Status Response -- should not happen
      flashLed(errorLed, 2, 50);
      Serial.println("Error2");
    }
    //

  }

  Serial.println("End of loop");

  if (timeElapsed > 180000) {
    {				
      ch_int();
      timeElapsed = 0;			 // reset the counter to 0 so the counting starts over...
    }

  }
  //interval=120000;

  delay(3000);
  return;
}

