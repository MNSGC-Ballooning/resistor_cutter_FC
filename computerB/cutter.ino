// Resistor Cutter functions

void cutResistorOnB() {   // char argument denotes what cutter is being cut
    digitalWrite(CUTTER_PIN1,HIGH);
    // digitalWrite(CUTTER_PIN2,HIGH);
    cutStatusB = true;
    cutterOnB = true;
}


void cutResistorOffB() {
    digitalWrite(CUTTER_PIN1,LOW);
    // digitalWrite(CUTTER_PIN2,LOW);
    cutterOnB = false;
}
