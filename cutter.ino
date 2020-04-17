// Resistor Cutter functions

void cutResistorOn(char cutter) {   // char argument denotes what cutter is being cut

  switch(cutter) {
    case 'a':

    break;

    case 'b':

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
      
      break;

    case 'b':
      cutStatusB = true;
      
      break;
  }
//  digitalWrite(CUTTER_PIN, LOW);
//
//  Serial.println("Cutter off...");
//
//  cutterOn = false;
}
