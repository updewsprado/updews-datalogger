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
          
				xbFlag = 1;
				Serial.println(F("xbFlag is set"));
          
				if (rx.getOption() == ZB_PACKET_ACKNOWLEDGED) {
					Serial.println(F("And sender got an ACK"));
				} else {
					Serial.println(F("But sender did not receive ACK"));
				}
			}
	}
	return;
}
