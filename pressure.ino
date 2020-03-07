
void updatePressure() {
  pressureAnalog = analogRead(PRESSURE_ANALOG_PIN);
  pressureVoltage = pressureAnalog*(5.0/1024);
  pressurePSI = map(pressureVoltage, 0.5, 4.5, 0, 15);    // map the voltage into a psi measurement
}
