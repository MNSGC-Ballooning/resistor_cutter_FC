// Resistor Cutter functions

void cutResistorOnA() {   // char argument denotes what cutter is being cut
    if(!cutterOnA){
      digitalWrite(CUTTER_PIN1,HIGH);
      cutStatusA = true;
      cutterOnA = true;
      Serial.println("Cutting");
      cutStampA = millis();
    }
}


void cutResistorOffA() {
    if(cutterOnA)
    {
      digitalWrite(CUTTER_PIN1,LOW);
      delay(5000);
      cutterOnA = false;
      Serial.println("Shutting cutter off");
    }
}
