  // The definitions are to see what I used. These will have to be changed to the main code once you implement them Steele
 // #define INITIALIZATION 0x00
  #define MAX_LAT_CHANGE 0.05 // In degrees (because the gps function returns this in degrees)
  #define MAX_LON_CHANGE 0.02 // In degrees this is smaller because the latitude in sweeden is smaller per degree
  #define MAX_ALT_CHANGE 300 // I think this is in meters, I believe thats what the gps outputs but can easily be changed to feet
//  #define BAD_A = 0x63
//  #define BAD_B = 0x64
//  #define BAD_C = 0x65
//  #define GPS_FAILURE 0x43 // I made CUT_C and FAILURE whatever I wanted so these can be changed as well
  
//  byte gpsCounter = 0;
//  uint8_t gpsLatSTATE = INITIALIZATION;
//  uint8_t gpsLonSTATE = INITIALIZATION;
//  uint8_t gpsAltSTATE = INITIALIZATION;

  //The code above is the data recieved from the computer A and B, with C denoting the main gondolas data. These can be changed to better fit the variables
  float latA, latB, latC, latAPrev, latBPrev, latCPrev;
  float lonA, lonB, lonC, lonAPrev, lonBPrev, lonCPrev;
  float altA, altB, altC, altAPrev, altBPrev, altCPrev;
  
  bool AWorking, BWorking, CWorking;

  // This is used for the difference between data measurements and will be evaluated later
//  float latAB, latBC, latCA;
//  float lonAB, lonBC, lonCA;
//  float altAB, altBC, altCA;
//
//  // These are used to describe the current state of the system: N for no problem (which is the default). A for data A is not correct, B for B is not correct, and C for C is not correct.
//  // X is used if two or more GPS' go out, in which case we wont know which is correct and I believe we must cut at that point.
//  char faultylat = 'N';
//  char faultylon = 'N';
//  char faultyalt = 'N';
//
//  // These are used for the final calculation of the latitude, longitude, and altitude. They will be an average of the correct ones left.
//  float LAT;
//  float LON;
//  float ALT;
//
//  // These are for the counters if we get a faulty measurement for any of them (getting an X)
//  byte gpsCounterLat = 0;
//  byte gpsCounterLon = 0;
//  byte gpsCounterAlt = 0;
//  
//  // This is the reason the box had to cut the system and the current state of the box
//  String reason;
//  String CurrentState = "Everything is fine";
//  

void CompareGPS() {
    ////////////////////////////////////////////////
    // WE NEED THE LATITUDE ANDS LONGITUDE VARIABLES
    ////////////////////////////////////////////////
    latA = dataPacketA.latitude;
    latB = dataPacketB.latitude;
    latC = latitude[0];
    lonA = dataPacketA.longitude;
    lonB = dataPacketB.longitude;
    lonC = longitude[0];
    altA = dataPacketA.Altitude;
    altB = dataPacketB.Altitude;
    altC = alt[0];
    
    AWorking = workingGPS(latAPrev, latA, lonAPrev, lonA, altAPrev, altA);
    BWorking = workingGPS(latBPrev, latB, lonBPrev, lonB, altBPrev, altB);
    CWorking = workingGPS(latCPrev, latC, lonCPrev, lonC, altCPrev, altC);
    //Serial.println(fixStatus[0]);
    //Serial.println(CWorking);
    if(fixStatus[0] == FIX && CWorking) {
      CWorking = true;
    }
    else {
      CWorking = false;
    }

    if (!AWorking && !BWorking && !CWorking) { // None working, must use linear regression for C
      latitude[0] = getNextLat(latitude[1],heading,dt,groundSpeed);
      longitude[0] = getNextLong(longitude[1],latitude[1],heading,dt,groundSpeed);
      if (abs(alt[1] - pressureAltitude) < 1000 && alt[1] < 80000) {
        // only log pressure altitude if it is within 1000 feet of the most recent altitude calculation AND below 80,000 ft (where pressure sensors are reliable)
        alt[0] = pressureAltitude;
      }
      else {
        // if pressure sensor altitude isn't reliable, find next altitude through a linear regression method
        alt[0] = getNextAlt(ascentRate,dt,alt[1]);
      }
    }
    else if(AWorking && !BWorking && !CWorking) { // A only working
      latitude[0] = latA;
      longitude[0] = lonA;
      alt[0] = altA;
      Serial.println("2");
    }
    else if(!AWorking && BWorking && !CWorking) { // B only working
      latitude[0] = latB;
      longitude[0] = lonB;
      alt[0] = altB;
      Serial.println("3");
    }
    else if(!AWorking && !BWorking && CWorking) { // C only working
      latitude[0] = latC;
      longitude[0] = lonC;
      alt[0] = altC;
      Serial.println("4");
    }
    else if(AWorking && BWorking && !CWorking) { // A and B working
      latitude[0] = (latA+latB)/2;
      longitude[0] = (lonA+lonB)/2;
      alt[0] = (altA+altB)/2;
      Serial.println("5");
    }
    else if(AWorking && !BWorking && CWorking) { // A and C working
      latitude[0] = (latA+latC)/2;
      longitude[0] = (lonA+lonC)/2;
      alt[0] = (altA+altC)/2;
      Serial.println("6");
    }
    else if(!AWorking && BWorking && CWorking) { // B and C working
      latitude[0] = (latB+latC)/2;
      longitude[0] = (lonB+lonC)/2;
      alt[0] = (altB+altC)/2;
      Serial.println("7");
    }
    else if(AWorking && BWorking && CWorking) { // A B and C working
      latitude[0] = (latA+latB+latC)/3;
      longitude[0] = (lonA+lonB+lonC)/3;
      alt[0] = (altA+altB+altC)/3;
      Serial.println("8");
    }


    // Setting the previous values to the current values so they can be used for computations next time.
latAPrev = latA;
latBPrev = latB;
latCPrev = latC;
lonAPrev = lonA;
lonBPrev = lonB;
lonCPrev = lonC;
altAPrev = altA;
altBPrev = altB;
altCPrev = altC;
}

// A walkthrough in making this work so its easier to visualize:
// We will be getting inputs for the current state of each GPS for latA, B, and C and the same for lon and alt. so those are 9 inputs in total
// From those inputs we will compare them all to their previous values (initialized at 0) and if they meet certain criteria (in the function), they are determined to be valid and working GPS's
// So the first time will have to do a linear regression of nothing. This may be an error, but i think it will just output 0 as well, but every other time will be good
// There are eight possibilities. These include any combination on A, B, and C working or not working, so we have to prepare for each of those cases
// The output sets the current latitude, longitude, and altitude to the only working one, or an average of all the working ones
// Finally, we will set the previous values to the ones calculated so that the next calculation has something to base the next one off of and the code can run again

// ALMOST CERTAINLY UNNECESSARY CODE, but commented out if we really need it after testing.
 
 /*   
    // Comparing each set of latitudes from A, B, and the main (dubbed as C in this case)
    latAB = abs(latA-latB);
    latBC = abs(latB-latC);
    latCA = abs(latC-latA);
    LAT = (latA+latB+latC)/3; // Takes the average of the latitudes for redundancy
  
    // Essentially doing the same with longitude and altitude
    lonAB = abs(lonA-lonB);
    lonBC = abs(lonB-lonC);
    lonCA = abs(lonC-lonA);
    LON = (lonA+lonB+lonC)/3; // Takes the average of the longitues for redundancy
  
    altAB = abs(altA-altB);
    altBC = abs(altB-altC);
    altCA = abs(altC-altA);
    ALT = (altA+altB+altC)/3; // Takes the average of the altitudes for redundancy




    /////////////////////////////////////
    //////////// Latitude ///////////////
    /////////////////////////////////////

  
    if(latAB>LAT_LIMIT && latBC>LAT_LIMIT && latCA<LAT_LIMIT) {
      faultylat = 'B';
      LAT = (latA+latC)/2;
      gpsLatSTATE = BAD_B;
      CurrentState = "Getting faulty measurements for the B system latitude";
      reason = "Using an average of A and C measurements for latitude";
    }
    else if(latAB>LAT_LIMIT && latBC<LAT_LIMIT && latCA>LAT_LIMIT) {
      faultylat = 'A';
      LAT = (latB+latC)/2;
      gpsLatSTATE = BAD_A;
      CurrentState = "Getting faulty measurements for the A system latitude";
      reason = "Using an average of B and C measurements for latitude";
    }
    else if(latAB<LAT_LIMIT && latBC>LAT_LIMIT && latCA>LAT_LIMIT) {
      faultylat = 'C';
      LAT = (latA+latB)/2;
      gpsLatSTATE = BAD_C;
      CurrentState = "Getting faulty measurements for the C system latitude";
      reason = "Using an average of A and B measurements for latitude";
    }
    else if(latAB>LAT_LIMIT && latBC>LAT_LIMIT && latCA>LAT_LIMIT) {
      faultylat = 'X'; // This is the case in which two or more GPS' go out. In this case, there is no way to tell which GPS is correct and we must terminate immediately
      LAT = 0;
      gpsCounterLat++;
      CurrentState = "Two or more wrong latitude measurements. Will cut soon";
      if(gpsCounterLat == 20) {
        gpsLatSTATE = GPS_FAILURE;
        reason = "The latitude measurements were faulty and needed to be cut";
      }
      else {
        gpsCounterLat = 0;
      }
    }





    /////////////////////////////////////
    //////////// Longitude //////////////
    /////////////////////////////////////
    
  
    // Because we are in Sweeden, the longitudes are closer together, and therefore the tolerance can be smaller for them
    // This is the longitude buffer
    if(lonAB>LON_LIMIT && lonBC>LON_LIMIT && lonCA<LON_LIMIT) {
      faultylon = 'B';
      LON = (lonA+lonC)/2;
      gpsLonSTATE = BAD_B;
      CurrentState = "Getting faulty measurements for the B system longitude";
      reason = "Using an average of A and C measurements for longitude";
    }
    else if(lonAB>LON_LIMIT && lonBC<LON_LIMIT && lonCA>LON_LIMIT) {
      faultylon = 'A';
      LON = (lonB+lonC)/2;
      gpsLonSTATE = BAD_A;
      CurrentState = "Getting faulty measurements for the A system longitude";
      reason = "Using an average of B and C measurements for longitude";
    }
    else if(lonAB<LON_LIMIT && lonBC>LON_LIMIT && lonCA>LON_LIMIT) {
      faultylon = 'C';
      LON = (lonA+lonB)/2;
      gpsLonSTATE = BAD_C;
      CurrentState = "Getting faulty measurements for the C system longitude";
      reason = "Using an average of A and B measurements for longitude";
    }
    else if(lonAB>LON_LIMIT && lonBC>LON_LIMIT && lonCA>LON_LIMIT) {
      faultylon = 'X'; // This is the case in which two or more GPS' go out. In this case, there is no way to tell which GPS is correct and we must terminate immediately
      LON = 0;
      gpsCounterLon++;
      CurrentState = "Two or more wrong longitude measurements. Will cut soon";
      if(gpsCounterLon == 20) {
        gpsLonSTATE = GPS_FAILURE;
        reason = "The longitude measurements were faulty and needed to be cut";
      }
      else {
        gpsCounterLon = 0;
      }
    }




    
    /////////////////////////////////////
    //////////// Altitdue ///////////////
    /////////////////////////////////////
    
    if(altAB>ALT_LIMIT && altBC>ALT_LIMIT && altCA<ALT_LIMIT) {
      faultyalt = 'B';
      ALT = (altA+altC)/2;
      gpsAltSTATE = BAD_B;
      CurrentState = "Getting faulty measurements for the B system altitude";
      reason = "Using an average of A and C measurements for altitude";
    }
    else if(altAB>ALT_LIMIT && altBC<ALT_LIMIT && altCA>ALT_LIMIT) {
      faultyalt = 'A';
      ALT = (altB+altC)/2;
      gpsAltSTATE = BAD_A;
      CurrentState = "Getting faulty measurements for the A system altitude";
      reason = "Using an average of B and C measurements for altitude";
    }
    else if(altAB<ALT_LIMIT && altBC>ALT_LIMIT && altCA>ALT_LIMIT) {
      faultyalt = 'C';
      ALT = altA+altB)/2;
      gpsAltSTATE = BAD_C;
      CurrentState = "Getting faulty measurements for the C system altitude";
      reason = "Using an average of A and B measurements for altitude";
      
    }
    else if(altAB>ALT_LIMIT && altBC>ALT_LIMIT && altCA>ALT_LIMIT) {
      faultyalt = 'X'; // This is the case in which two or more GPS' go out. In this case, there is no way to tell which GPS is correct and we must terminate immediately
      ALT = 0;
      gpsCounterAlt++;
      CurrentState = "Two or more wrong altitude measurements. Will cut soon";
      if(gpsCounterAlt == 20) {
        gpsAltSTATE = GPS_FAILURE;
        reason = "The altitude measurements were faulty and needed to be cut";
      }
      else {
        gpsCounterAlt = 0;
      }
    }


    if(fixStatus[0] == FIX) {
      // Know main is correct, compare with others
      if(
    }




















    if (gpsLatSTATE == BAD_B || gpsLonSTATE == BAD_B || gpsAltSTATE == BAD_B) {
      B_counter++;
      if(B_counter >= 40) {
        current = CUT_B;
        B_counter = 0;
      }
    }

    else if (gpsLatSTATE == BAD_A || gpsLonSTATE == BAD_A || gpsAltSTATE == BAD_A) {
      A_counter++;
      if(B_counter >= 40) {
        current = CUT_A;
        A_counter = 0;
      }
    }

    else if (gpsLatSTATE == BAD_C || gpsLonSTATE == BAD_C || gpsAltSTATE == BAD_C) {
      C_counter++;
      if(B_counter >= 40) {
        current = CUT_C;
        B_counter = 0;
      }
    }
}

*/
bool workingGPS(float latPrev, float latCurrent, float lonPrev, float lonCurrent, float altPrev, float altCurrent){
  float latDiff = abs(latPrev - latCurrent);
  float lonDiff = abs(lonPrev - lonCurrent);
  //Serial.println(altPrev);
  //Serial.println(altCurrent);
  float altDiff = abs(altPrev - altCurrent);
  if(latDiff == 0 && lonDiff == 0 && altDiff == 0) {
    return false;
  }
  else if(latDiff >= MAX_LAT_CHANGE || lonDiff >= MAX_LON_CHANGE || altDiff >= MAX_ALT_CHANGE) {
    return false;
  }
  return true;
}

    // The only case not covered in both latitudes and longitudes is the case in which two GPS' are giving bad information, but the bad information they are giving is the same.
    // This means that the latitude or longitude used will be a faulty one, and I do not currently see a way to fix this problem
  
