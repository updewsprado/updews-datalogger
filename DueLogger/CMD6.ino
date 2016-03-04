void getdataBroadcastNew(int mode,char readCode){
	columnData[0] =0;
	char *columnPointer = columnData;
	digitalWrite(trigSW, LOW);
	digitalWrite(trigSW, HIGH);
	delay(2000);
	commandCAN = 0;
	clear_can_array(can_default_buffer);  //clear out temp buffer
	
  switch(readCode)
  {
	case 'T':  //tilt only

		if (PRINT_MODE == 1) Serial.println("  --function: getdataBroadcastNew(), case T -- called");
		Serial1.print("ARQWAIT");        
		delay(2000);
                sprintf(columnData,"SDAT20A SAMPLEATA SAMPLASASAE ATA SAMPLE ATA SAMPLE ATA SAMPLE ATA SAMPLE ATA SAMPLEATA SAMPLE ATA SAMPLE  SAMPLEATA SAMPLASASAE ATA SAMPLE ATA SAMPLE ATA SAMPLE ATA SAMPLE ATA SAMPLEATA SAMPLE ATA SAMPLE  ");
                //GET_DATA(columnData,PASS_AXEL1_ADC_CALIB_MINMAX); 
		Serial.println(columnData);
		parsedData = parser(columnData,8);
		writeData(parsedData);													//write data to sdcard
		Serial.println(parsedData);
		sprintf(allData,parsedData ,strlen(parsedData));		

		Serial1.print("ARQWAIT");
                delay(2000);
                sprintf(columnData,"SAMP21LEATA1 SAMPLE ATA2 SAMPLE ATA SAM3PLE ATA SAM4PLE ATA SAMPLE5 ATA SAMPLE6ATA SAMPLE ATA7 SAMPLE ATA 8SAMPLE ATA 9SAMPLE ATA SAMPL0E ATA SAMP1LEATA SAMPLE ATA 2SAMPLE ATA 3SAMPLE ATA SAMP4LE ATA SAMPLE 5ATASAMPLEATA1 SAMPLE ATA2 SAMPLE ATA SAM3PLE ATA SAM4PLE ATA SAMPLE5 ATA SAMPLE6ATA SAMPLE ATA7 SAMPLE ATA 8SAMPLE ATA 9SAMPLE ATA SAMPL0E ATA SAMP1LEATA SAMPLE ATA 2SAMPLE ATA 3SAMPLE ATA SAMP4LE ATA SAMPLE 5ATA");		
                //GET_DATA(columnData,PASS_AXEL2_ADC_CALIB_MINMAX);
		Serial.println(columnData);
		parsedData = parser(columnData,8);
		writeData(parsedData);													//write data to sdcard
		Serial.println(parsedData);
		strncat(allData,"+" ,1);
		strncat(allData,parsedData ,strlen(parsedData));
			   
		printData(allData, mode);

		digitalWrite(RELAYPIN, LOW);

        break;
    case 'S':  //with soms 
				
		if (PRINT_MODE == 1) Serial.println("  --function: getdataBroadcastNew(), case S -- called");
		Serial1.print("ARQWAIT");
		GET_DATA(columnData,PASS_AXEL1_ADC_CALIB_MINMAX); 
		//GET_DATA(columnData,100); // random axel1 cmd para walang datang reply yung nodes
		Serial1.print("ARQWAIT");
		Serial.println(columnData);
		parsedData = parser(columnData,8);
		writeData(parsedData);													//write data to sdcard
		Serial.println(parsedData);
		sprintf(allData,parsedData ,strlen(parsedData));		
				
		Serial1.print("ARQWAIT");        
		GET_DATA(columnData,PASS_AXEL2_ADC_CALIB_MINMAX);
		Serial1.print("ARQWAIT");
		Serial.println(columnData);
		parsedData = parser(columnData,8);
		writeData(parsedData);													//write data to sdcard
		Serial.println(parsedData);
		strncat(allData,"+" ,1);
		strncat(allData,parsedData ,strlen(parsedData));

		Serial1.print("ARQWAIT");                
		GET_DATA(columnData,POLL_SOMS_RAW_NEW);
		Serial.print("columnData: ");
		Serial.println(columnData);
		parsedData = parser(columnData,2);
		writeData(parsedData);
		Serial.print("parsedData: ");													//write data to sdcard
		Serial.println(parsedData);
		strncat(allData,"+" ,1);
		strncat(allData,parsedData ,strlen(parsedData));
		
		Serial1.print("ARQWAIT");                
		GET_DATA(columnData,POLL_SOMS_CALIB_NEW);
		Serial.print("columnData: ");
		Serial.println(columnData);
		parsedData = parser(columnData,3);
		writeData(parsedData);
		Serial.print("parsedData: ");													//write data to sdcard
		Serial.println(parsedData);
		strncat(allData,"+" ,1);
		strncat(allData,parsedData ,strlen(parsedData));		

		printData(allData, mode);
			
		digitalWrite(RELAYPIN, LOW);
		break;

   }
   digitalWrite(trigSW, LOW);
}


