// GPS functions

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

  checkFix();
  
}

void checkFix() {
  if(gps.getFixAge() < 4000) {
    fixStatus = FIX;
  }
  else if(gps.getFixAge() > 4000) {
    fixStatus = NOFIX;
  }
  else{
    fixStatus = NOFIX;
  }
}

void fixLEDSchema() {
  // fix LED timing schema
  if(millis() - gpsLEDStamp > GPS_LED_INTERVAL && !gpsLEDOn) {
    gpsLEDStamp = millis();
    digitalWrite(LED_GPS,HIGH);
    gpsLEDOn = true;
  }
  else if(millis() - gpsLEDStamp > FIX_INTERVAL && fixStatus == FIX && gpsLEDOn) {
    digitalWrite(LED_GPS,LOW);
  }
  else if(millis() - gpsLEDStamp > NOFIX_INTERVAL && fixStatus == NOFIX && gpsLEDOn) {
    digitalWrite(LED_GPS,LOW);
  }
  
}


float getVelocity(float alt1, float alt2, long time1, long time2) {
  float velocity = 0;
  if(alt1 !=0 && alt2!= 0) {
    time1 /= 1000;    // divide milliseconds into seconds
    time2 /= 1000;
    velocity = ((alt1-alt2)/(time1-time2)) * 60;
  }

  return velocity;      // returns a vleocity in feet/minute
}
