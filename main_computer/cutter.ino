// Resistor Cutter functions

void cutResistorOn(char cutter) {   // char argument denotes what cutter is being cut

  switch(cutter) {
    case 'a':
      // send a cut command to box A    ***WAITING ON COMM PROTOCOL TO BE FIGURED OUT BEFORE THIS IS IMPLEMENTED***
    break;

    case 'b':
      // send a cut command to box B
    break;
  }
//  digitalWrite(CUTTER_PIN, HIGH);
//
//  cutterOn = true;
//  cutStamp = millis();
}


void cutResistorOff(char cutter) {

  switch(cutter) {
    case 'a':
      cutStatusA = true;
      // send stop command to box A (likely won't be received if box floats away with balloon, but still try)
      break;

    case 'b':
      cutStatusB = true;
      // send stop command to box B (likely won't be received if box floats away with balloon, but still try)
      break;
  }
//  digitalWrite(CUTTER_PIN, LOW);
//
//  Serial.println("Cutter off...");
//
//  cutterOn = false;
}
