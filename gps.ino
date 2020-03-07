
void initGPS() {
  ubloxSerial.begin(UBLOX_BAUD);                                       //initiate GPS
  gps.init();                                                          //Initiate GPS Data lines

  Serial.println("GPS initialized...");
  delay(50);
  if(gps.setAirborne()){
    Serial.println("Airborne mode set...");
  }
  Serial.println("GPS configured");
  
}


void updateGPS() {
  latitude = gps.getLat();
  longitude = gps.getLon();
  sats = gps.getSats();

  for(int i=0; i++; i<(sizeof(alt)/sizeof(float)-1)) {
    alt[i+1] = alt[i];
    timeStamp[i+1] = timeStamp[i];
  }
  alt[0] =  gps.getAlt_feet();
  timeStamp[0] = millis();

  ascentRate = getVelocity(alt[0],alt[9],timeStamp[0],timeStamp[9]);
  
}


float getVelocity(float alt1, float alt2, long time1, long time2) {
  time1 /= 1000;    // divide milliseconds into seconds
  time2 /= 1000;
  float velocity = (alt1-alt2)/(time1-time2);

  return velocity;      // returns a vleocity in feet/second 
}
