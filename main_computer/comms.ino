void checkComms(char cutter){
   switch (cutter) {
      case 'A': // if cutter A selected
        byte input[21] = {0}; // initialize data collection byte array
        if(blueSerialA.available()>21) // collects 21 bytes, total number sent (change to 23 if using checksum)
        {
          for(int i=0; i<21; i++)
          {
            input[i] = blueSerialA.read(); // saves each byte into byte array input
          }
          memcpy(&dataPacketA,&input,21); // copies input onto struct dataPacket1, a readable format
          break;
        }
          
      case 'B': // if cutter B selected
        byte input[21] = {0}; // initialize data collection byte array
        if(blueSerialB.available()>21) // collects 21 bytes, total number sent (change to 23 if using checksum)
        {
          for(int i=0; i<21; i++)
          {
            input[i] = blueSerialB.read(); // saves each byte into byte array input
          }
          memcpy(&dataPacketB,&input,21); // copies input onto struct dataPacket1, a readable format
          break;
        }
   }
}
