<<<<<<< HEAD:computerA/pressure.ino
// Pressure sensor functions

//void initPressure() {
//  while(!baro.begin())
//  {
//    Serial.println(F("Could not find a valid MS5611 sensor, check wiring!"));
//    delay(500);
//  }
//
//  baroReferencePressure = baro.readPressure();                    // Get a reference pressure for relative altitude
//
//  Serial.println(F("MS5611 barometer setup successful..."));
//}


//void calibrateBaro(float pressure, float alt) {
//  // inputs: pressure (PA), alt (feet)
//  // Read a pressure value in order get an accurate sea level pressure value
//  seaLevelPressure = baro.getSeaLevel(pressure, alt/FEET_PER_METER);
//
//  Serial.println(F("MS5611 calibrated..."));
//  
//}

//
//void updatePressure(){
//
//  // Read true temperature & Pressure
//  baroTemp = baro.readTemperature();
//  pressurePa = baro.readPressure();
//
//  // Calculate altitude
//  pressureAltitude = baro.getAltitude(pressurePa, seaLevelPressure)*FEET_PER_METER;
//  pressureRelativeAltitude = baro.getAltitude(pressurePa, baroReferencePressure)*FEET_PER_METER; 
//}
=======
/*// Pressure sensor functions

void initPressure() {
  while(!baro.begin())
  {
    Serial.println(F("Could not find a valid MS5611 sensor, check wiring!"));
    delay(500);
  }

  baroReferencePressure = baro.readPressure();                    // Get a reference pressure for relative altitude

  Serial.println(F("MS5611 barometer setup successful..."));
}


void calibrateBaro(float pressure, float alt) {
  // inputs: pressure (PA), alt (feet)
  // Read a pressure value in order get an accurate sea level pressure value
  seaLevelPressure = baro.getSeaLevel(pressure, alt/FEET_PER_METER);

  Serial.println(F("MS5611 calibrated..."));
  
}


void updatePressure(){

  // Read true temperature & Pressure
  baroTemp = baro.readTemperature();
  pressurePa = baro.readPressure();

  // Calculate altitude
  pressureAltitude = baro.getAltitude(pressurePa, seaLevelPressure)*FEET_PER_METER;
  pressureRelativeAltitude = baro.getAltitude(pressurePa, baroReferencePressure)*FEET_PER_METER; 
}*/
>>>>>>> origin/development_splitComps:pressure.ino
