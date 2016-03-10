void getXBFlag() { 
	Serial.println(F("Wait for xb"));  
	xbee.readPacket();
    
	if (xbee.getResponse().isAvailable()) {
		// got something
		Serial.println(F("We got something!"));
      
		if (xbee.getResponse().getApiId() == ZB_RX_RESPONSE) {
			// got a zb rx packet
			Serial.println(F("Izz zb rx packet!"));
        
			// now fill our zb rx class
			xbee.getResponse().getZBRxResponse(rx);
			for (int i = 0; i < rx.getDataLength (); i++)
				Serial.print((char) rx.getData(i));
				Serial.println();
          
				xbFlag = 1;
				Serial.println(F("xbFlag is set"));
          
				if (rx.getOption() == ZB_PACKET_ACKNOWLEDGED) {
					// the sender got an ACK
					//flashLed(statusLed, 10, 10);
					Serial.println(F("And sender got an ACK"));
				} else {
					// we got it (obviously) but sender didn't get an ACK
					//flashLed(errorLed, 2, 20);
					Serial.println(F("But sender did not receive ACK"));
				}
				// set dataLed PWM to value of the first byte in the data
				//analogWrite(dataLed, rx.getData(0));
			} else if (xbee.getResponse().getApiId() == MODEM_STATUS_RESPONSE) {
				xbee.getResponse().getModemStatusResponse(msr);
				// the local XBee sends this response on certain events, like association/dissociation
				//Serial.println(F("Izz not a zb rx packet. Maybe status."));
        
				if (msr.getStatus() == ASSOCIATED) {
					// yay this is great.  flash led
					//flashLed(statusLed, 10, 10);
					//Serial.println(F("ASSOCIATED"));
				} else if (msr.getStatus() == DISASSOCIATED) {
					// this is awful.. flash led to show our discontent
					//flashLed(errorLed, 10, 10);
					//Serial.println(F("DISASSOCIATED"));
				} else {
					// another status
					//flashLed(statusLed, 5, 10);
					//Serial.println(F("Some other status"));
				}
			} else {
				// not something we were expecting
				//flashLed(errorLed, 1, 25);
				//Serial.println(F("Izz something we are not expecting"));    
			}
	} else if (xbee.getResponse().isError()) {
		//nss.print("Error reading packet.  Error code: ");  
		//nss.println(xbee.getResponse().getErrorCode());
		//Serial.println(F("Error"));
	}
	return;
}
