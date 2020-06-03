void sendData(){
  // recording current data
  dataPacket.startByte = 0x42; 
  dataPacket.cutterTag = 'A';
  dataPacket.latitude = gps.getLat();
  dataPacket.longitude = gps.getLon();
  dataPacket.Altitude = gps.getAlt_feet();
  dataPacket.AR = ascentRate;
  dataPacket.cutStatus = cutStatusA;
  dataPacket.currentState = state;
//  dataPacket.checksum = 
  dataPacket.stopByte = 0x53;

  byte dataHolder[21] = {0};              // define output array (change 21 to 23 if using checksum --> total number of bits increases to 23)
  memcpy(&dataHolder, &dataPacket, 21);   // pass data packet to output array as bytes 
  blueSerial.write(dataHolder,21);        // write output array to main computer
}

void readInstruction(){
  byte input = 0;               // initialize data collection byte array
  if(blueSerial.available()>0)
  {
    input = blueSerial.read();  // saves byte sent from main
    if(input == 0x15)           // cut command
      cutResistorOnA();
    else if (input == 0x53)     // end cut command
      cutResistorOffA();
  }
}
