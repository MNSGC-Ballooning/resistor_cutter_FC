// tab containing GPS functions

void initGPS() {
  UBLOX_SERIAL.begin(UBLOX_BAUD);                                       //initiate GPS
  gps.init();                                                          //Initiate GPS Data lines

  Serial.println("GPS initialized...");
  delay(50);
  if(gps.setAirborne()){
    Serial.println("Airborne mode set...");
  }
  Serial.println("GPS configured"); 

  // iniialize arrays
  for (int i = 0; i++; i < SIZE+1) {
    latitude[i] = 0;
    longitude[i] = 0;
    alt[i] = 0;
    timeStamp[i] = 0;
  }
}


void updateGPS() {

  for(int i=(sizeof(alt)/sizeof(float)); i--; i > 0) {
    latitude[i] = latitude[i-1];
    longitude[i] = longitude[i-1];
    alt[i] = alt[i-1];
    timeStamp[i] = timeStamp[i-1];
  }

  latitude[0] = gps.getLat();
  longitude[0] = gps.getLon();
  alt[0] =  gps.getAlt_feet();
  timeStamp[0] = millis();
  sats = gps.getSats();
  
  groundSpeed = getGroundSpeed(latitude[0],latitude[SIZE-1],longitude[0],longitude[SIZE-1],timeStamp[0],timeStamp[SIZE-1]);
  ascentRate = getAscentRate(alt[0],alt[SIZE-1],timeStamp[0],timeStamp[SIZE-1]);
  dt = (timeStamp[0] - timeStamp[1])/1000;  // best estimate of time between calculations
  heading = getHeading(latitude[0],latitude[SIZE-1],longitude[0],longitude[SIZE-1]);
  nextAlt = getNextAlt(ascentRate,dt,alt[0]);
  nextLat = getNextLat(latitude[0],heading,dt,groundSpeed);
  nextLong = getNextLong(longitude[0],latitude[0],heading,dt,groundSpeed);

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


float getAscentRate(float alt1, float alt2, long time1, long time2) {
  float velocity = 0;

  time1 /= 1000;    // divide milliseconds into seconds
  time2 /= 1000;
    
  if(alt1 !=0 && alt2 != 0) {
    velocity = ((alt1-alt2)/(time1-time2)) * 60;
  }

  return velocity;      // returns a vleocity in feet/minute
}


float getNextAlt(float ascentRate, float dt, float currentAlt) {
  float alt = 0;
  if(currentAlt != 0) {
    alt = ascentRate*dt + currentAlt;
  }

  return alt;
}


float getGroundSpeed(float lat1, float lat2, float long1, float long2, float time1, float time2) {
  float groundSpeed = 0;

  float miles_per_lat = 69; // roughly 69 miles per degree latitude
  float miles_per_long = cos(lat1*D2R)*69.172; // miles per degree longitude depending on the current latitude

  time1 /= 1000;  // divide milliseconds into seconds
  time2 /= 1000;
  
  if(lat1 != 0 && lat2 != 0 && long1 != 0 && long2 != 0) {
    float lat_per_hour = (lat1 - lat2)/(time1 - time2) * SECONDS_PER_HOUR;  // degrees latitude per hour
    float long_per_hour = (long1 - long2)/(time1- time2) *  SECONDS_PER_HOUR; // degrees longitutde per hour

    groundSpeed = sqrt(pow(lat_per_hour*miles_per_lat,2) + pow(long_per_hour*miles_per_long,2));  // find magnitude of the velocity vector
  }

  return groundSpeed*FPM_PER_MPH;   // return in feet per minute
}


float getNextLat(float lat1, float heading, float dt, float groundSpeed) {
  float nextLat = 0;

  groundSpeed /= (60*5280);  // convert feet per minute to miles per second
  float dx = groundSpeed * dt;  // gives predicted displacement in miles

  float miles_per_lat = 69; // roughly consistent for the entire Earth
  
  if(lat1 != 0) {
    nextLat = lat1 + (dx/miles_per_lat)*sin(heading); // simple vector math to get next latitude
  }

  return nextLat;
}


float getNextLong(float long1, float lat1, float heading, float dt, float groundSpeed) {
  float nextLong = 0;
  
  groundSpeed /= (60*5280);  // convert feet per minute to miles per second
  float dx = groundSpeed * dt;  // gives predicted displacement in miles

  float miles_per_long = cos(lat1*D2R)*69.172;  // varies depending on latitude
  
  if(long1 != 0) {
    nextLong = long1 + (dx/miles_per_long)*cos(heading);
  }

  return nextLong;
}

float getHeading(float lat1, float lat2, float long1, float long2) {
  // position 1 is where you are, position 2 is where you were
  
  float dLong = long1 - long2;  // delta longitude
  float headingY = cos(lat1*D2R)*sin((dLong)*D2R);
  float headingX = cos(lat2*D2R)*sin(lat1*D2R) - sin(lat2*D2R)*cos(lat1*D2R)*cos((dLong)*D2R);

  float heading = atan2(headingX,headingY);

  return heading;
}
