// Pressure sensor functions

void updatePressure() {
  pressureAnalog = analogRead(PRESSURE_ANALOG_PIN);
  float pressureVoltage = pressureAnalog*(5.0/1024);
  pressurePSI = map(pressureVoltage, 0.5, 4.5, 0, 15);    // map the voltage into a psi measurement

  // for equation: https://www.brisbanehotairballooning.com.au/pressure-and-altitude-conversion/
  if(pressurePSI != 0) {
    pressureAltitude = (pow(10,log10(pressurePSI/SEA_LEVEL_PSI)/5.2558797) - 1)/(-6.8755856E-6);    // that desperate altitude that was mentioned
  }
}

void setPressureTimer() {
  static bool timerSet = false;
  static byte counter = 0;
  // set the pressure stamp once, and only if the sensor calulcates an altitude above 70k feet
  if(pressureAltitude > PRESSURE_TIMER_ALTITUDE && !timerSet) {
    counter++;
    if (counter >= 20) {
      pressureStamp = millis();
      timerSet = true;
    }
  }
  
}
