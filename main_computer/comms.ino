bool checkComms(char cutter){
   switch (cutter) {
      case 'A': // if cutter A selected
        byte input[23] = {0}; // initialize data collection byte array
        uint16_t checksumCheck = 0;
        if(blueSerialA.available()>22) // collects 20 bytes
        {
          for(int i=0; i<23; i++)
          {
            input[i] = blueSerialA.read(); // saves each byte into byte array input
            if (i<20) checksumCheck += input[i];
          }
          memcpy(&dataPacketA,&input,23); // copies input onto struct dataPacket1, a readable format
          if(dataPacketA.checksum != checksumCheck) return false;
          if((dataPacketA.startByte != 0x42) || (dataPacketA.stopByte != 0x53)) return false; // what to do if these don't match??
          
          break;
        }
          
      case 'B': // if cutter B selected
        byte input[23] = {0}; // initialize data collection byte array
        uint16_t checksumCheck = 0;
        if(blueSerialB.available()>22) // collects 20 bytes
        {
          for(int i=0; i<23; i++)
          {
            input[i] = blueSerialB.read(); // saves each byte into byte array input
            if (i<20) checksumCheck += input[i];
          }
          memcpy(&dataPacketB,&input,23); // copies input onto struct dataPacket1, a readable format
          if(dataPacketB.checksum != checksumCheck) return false;
          if((dataPacketB.startByte != 0x42) || (dataPacketB.stopByte != 0x53)) return false; // what to do if these don't match??
          
          break;
   }
}
