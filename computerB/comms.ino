void sendData(){
  // recording current data
  dataPacket.startByte = 0x42; 
  dataPacket.cutterTag = 'B';
  dataPacket.latitude = gps.getLat();
  dataPacket.longitude = gps.getLon();
  dataPacket.Altitude = gps.getAlt_feet();
  dataPacket.AR = ascentRate;
  dataPacket.cutStatus = cutStatusB;
  dataPacket.currentState = state;
  dataPacket.checksum = 0;
  dataPacket.stopByte = 0x53;

  byte dataHolder[23] = {0};              // define output array (change 21 to 23 if using checksum --> total number of bytes increases to 23)
  memcpy(&dataHolder, &dataPacket, 20);   // pass data packet to output array as bytes 

  for( uint8_t i=0; i<20; i++) dataPacket.checksum+=dataHolder[i];

  byte extraStuff[2] = {0};
  memcpy(&extraStuff, &dataPacket.checksum, 2);
  dataHolder[20] = extraStuff[0];
  dataHolder[21] = extraStuff[1];
  dataHolder[22] = 0x53;

  blueSerial.write(dataHolder,23);        // write output array to main computer
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

        Serial.println(inputPacket.checksum);
        Serial.println(checksumCheck);
        Serial.println();
      
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
    
    if(inputPacket.command == 0x15 && inputPacket.cutterTag == 0x42)           // cut command
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
