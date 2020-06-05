// Resistor Cutter functions

void cutResistorOnA() {   // char argument denotes what cutter is being cut
    digitalWrite(CUTTER_PIN1,HIGH);
    // digitalWrite(CUTTER_PIN2,HIGH);
    cutStatusA = true;
    cutterOnA = true;
}


void cutResistorOffA() {
    digitalWrite(CUTTER_PIN1,LOW);
    // digitalWrite(CUTTER_PIN2,LOW);
    cutterOnA = false;
}
