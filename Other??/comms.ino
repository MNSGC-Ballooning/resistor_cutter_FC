// tab for functions relevant to usage of the RFM69HCW
// currently set up for use with the RFM69.h library, but this will have to be rewritten to work with the radiohead library

//void initRadio() {
//  radio.initialize(FREQUENCY, LOCAL_ADDRESS, NETWORKID);  // initialze the comms device
//  radio.setHighPower();   // always use for RFM69HCW
//
//  if(ENCRYPT) radio.encrypt(ENCRYPT_KEY); // encrypt the radio network if requested
//
//  Serial.print("Node ");  // verify to user that the local node has been initialized
//  Serial.print(LOCAL_ADDRESS,DEC);
//  Serial.println(" initialized.");
//}


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
  memcpy(&dataHolder, &dataPacket, 21);  // pass data packet to output array as bytes 
  blueSerial.write(dataHolder,21);      // write output array to main computer
}

void readInstruction(){
  // if(main says to cut)
  //    cutResistorOnA();
  // else if(main says stop cutting)
  //    cutResistorOffA();
}
