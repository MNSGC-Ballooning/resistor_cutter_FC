// Resistor Cutter functions

void cutResistorOn(char cutter){   // char argument denotes what cutter is being cut

  switch(cutter) {
    case 'a':
      blueSerialA.write(0x15);     // cut command for A
    break;

    case 'b':
      blueSerialB.write(0x25);     // cut command for B
    break;
  }

  cutterOn = true;
  cutStamp = millis();
}


void cutResistorOff(char cutter) {

  switch(cutter) {
    case 'a':
      cutStatusA = true;
      blueSerialA.write(0x53); // send stop command to box A (likely won't be received if box floats away with balloon, but still try)
      break;

    case 'b':
      cutStatusB = true;
      blueSerialB.write(0x53); // send stop command to box B (likely won't be received if box floats away with balloon, but still try)
      break;
  }
  cutterOn = false;
}
