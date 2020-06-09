// Resistor Cutter functions

void cutResistorOn(char cutter){   // char argument denotes what cutter is being cut

  switch(cutter) {
    case 'a':
      instructionA.startByte = 0x42; 
      instructionA.cutterTag = 'A';
      instructionA.Command = 0x15;
      //instructionA.pressure = pressure;
      instructionA.checksum = 0;
      instructionA.stopByte = 0x53;

      byte dataHolder[6] = {0}; // change to 10 if sending pressure data
      memcpy(&dataHolder, &instructionA, 3);

      for (uint8_t i=0; i<20; i++) instructionA.checksum+=dataHolder[i];

      byte extraStuff[2] = {0};
      memcpy(&extraStuff, &instructionA.checksum, 2);
      dataHolder[4] = extraStuff[0];
      dataHolder[5] = extraStuff[1];
      dataHolder[6] = extraStuff[2];

      blueSerialA.write(dataHolder,6);
      cutterOnA = true;
      cutStampA = millis();
    break;

    case 'b':
      instructionB.startByte = 0x42; 
      instructionB.cutterTag = 'B';
      instructionB.Command = 0x25;
      //instructionB.pressure = pressure;
      instructionB.checksum = 0;
      instructionB.stopByte = 0x53;

      byte dataHolderB[6] = {0};
      memcpy(&dataHolderB, &instructionB, 3);

      for (uint8_t i=0; i<20; i++) instructionB.checksum+=dataHolderB[i];

      byte extraStuffB[2] = {0};
      memcpy(&extraStuffB, &instructionB.checksum, 2);
      dataHolderB[4] = extraStuffB[0];
      dataHolderB[5] = extraStuffB[1];
      dataHolderB[6] = extraStuffB[2];

      blueSerialB.write(dataHolderB,6);
      cutterOnB = true;
      cutStampB = millis();
    break;
  }
}


void cutResistorOff(char cutter) {

  switch(cutter) {
    case 'a':
      instructionA.startByte = 0x42; 
      instructionA.cutterTag = 'A';
      instructionA.Command = 0x35;
      //instructionA.pressure = pressure;
      instructionA.checksum = 0;
      instructionA.stopByte = 0x53;

      byte dataHolder[6] = {0}; // change to 10 if sending pressure data
      memcpy(&dataHolder, &instructionA, 3);

      for (uint8_t i=0; i<20; i++) instructionA.checksum+=dataHolder[i];

      byte extraStuff[2] = {0};
      memcpy(&extraStuff, &instructionA.checksum, 2);
      dataHolder[4] = extraStuff[0];
      dataHolder[5] = extraStuff[1];
      dataHolder[6] = extraStuff[2];

      blueSerialA.write(dataHolder,6);
      cutterOnA = false;

    case 'b':
      instructionB.startByte = 0x42; 
      instructionB.cutterTag = 'B';
      instructionB.Command = 0x35;
      //instructionB.pressure = pressure;
      instructionB.checksum = 0;
      instructionB.stopByte = 0x53;

      byte dataHolderB[6] = {0}; // change to 10 if sending pressure data
      memcpy(&dataHolderB, &instructionB, 3);

      for (uint8_t i=0; i<20; i++) instructionB.checksum+=dataHolderB[i];

      byte extraStuffB[2] = {0};
      memcpy(&extraStuffB, &instructionB.checksum, 2);
      dataHolderB[4] = extraStuffB[0];
      dataHolderB[5] = extraStuffB[1];
      dataHolderB[6] = extraStuffB[2];

      blueSerialB.write(dataHolderB,6);
      cutterOnB = false;
      break;
  }
}
