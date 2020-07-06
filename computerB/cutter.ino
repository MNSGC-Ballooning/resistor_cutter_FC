// Resistor Cutter functions

void cutResistorOnB() {   // char argument denotes what cutter is being cut
    if(!cutterOnB){
      digitalWrite(CUTTER_PIN1,HIGH);
      digitalWrite(CUTTER_PIN3,HIGH);
      cutStatusB = true;
      cutterOnB = true;
      Serial.println("Cutting");
      cutStampA = millis();
    }
}


void cutResistorOffB() {
    if(cutterOnB)
    {
      digitalWrite(CUTTER_PIN1,LOW);
      digitalWrite(CUTTER_PIN3,LOW);
      cutterOnB = false;
      Serial.println("Shutting cutter off");
    }
}
