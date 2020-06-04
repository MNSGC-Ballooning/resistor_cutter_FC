// Resistor Cutter functions

void cutResistorOnA() {   // char argument denotes what cutter is being cut
    digitalWrite(CUTTER_PIN,HIGH);
    cutStatusA = true;
    cutterOnA = true;
}


void cutResistorOffA() {
    digitalWrite(CUTTER_PIN,LOW);
    cutterOnA = false;
}
