void sendData(){
  // recording current data
  dataPacket.startByte = 0x42; 
  dataPacket.cutterTag = 'B';
  dataPacket.latitude = gps.getLat();
  dataPacket.longitude = gps.getLon();
  dataPacket.Altitude = gps.getAlt_feet();
  dataPacket.cutStatus = cutStatusB;
  dataPacket.currentState = state;
  dataPacket.checksum = 0;
  dataPacket.stopByte = 0x53;

  byte dataHolder[19] = {0};              // define output array
  memcpy(&dataHolder, &dataPacket, 16);   // pass data packet to output array as bytes 

  for( uint8_t i=0; i<20; i++) dataPacket.checksum+=dataHolder[i];

  byte extraStuff[2] = {0};
  memcpy(&extraStuff, &dataPacket.checksum, 2);
  dataHolder[16] = extraStuff[0];
  dataHolder[17] = extraStuff[1];
  dataHolder[18] = 0x53;

  blueSerial.write(dataHolder,19);        // write output array to main computer
}

bool readInstruction(){
  byte inputHolder[6] = {0};               // initialize data collection byte
  uint16_t checksumCheck = 0;
  
        if(blueSerial.available()>5) 
        {
          for(int i=0; i<6; i++)
          {
            inputHolder[i] = blueSerial.read(); // saves each byte into byte array input
            if (i<3) checksumCheck += inputHolder[i];
          }
          memcpy(&inputPacket,&inputHolder,6); // copies input onto struct dataPacket1, a readable format
        }
      
    if(inputPacket.checksum != checksumCheck)
          {
            while(blueSerial.available()>0) blueSerial.read();
            return false;
          }
     if((inputPacket.startByte != 0x42) || (inputPacket.stopByte != 0x53))
          {
            while(blueSerial.available()>0) blueSerial.read();
            return false; 
          }
    
    if(inputPacket.command == 0x25 && inputPacket.cutterTag == 0x42)           // cut command
      cutResistorOnB();
    else
      cutResistorOffB();

    timeOut = 0;
    Serial.println("Autonomous Mode OFF");
    return true;
  }

void requestCut(){
    if(autonomousNow && !cutterOnB) cutResistorOnB();
  }

void fixBlueLEDSchema() {
  // for LED2:
  // fix LED timing schema
  // if connection, solid light
  // if no connection, blink

  if(!autonomousNow && !LED2On){
    digitalWrite(LED2,HIGH);
    LED2On = true;
  }
  else if (autonomousNow && !LED2On && millis()-LED2OffStamp > LED_INTERVAL){
    digitalWrite(LED2,HIGH);
    LED2OnStamp = millis();
    LED2On = true;
  }
  else if (autonomousNow && LED2On && millis()-LED2OnStamp > LED_INTERVAL){
    digitalWrite(LED2,LOW);
    LED2OffStamp = millis();
    LED2On = false;
  }  
}
