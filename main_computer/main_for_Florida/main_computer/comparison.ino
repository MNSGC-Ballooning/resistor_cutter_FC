 
 ////////////////////////////////////////////////////////////////////////////////////////////////
 ///////////******* IMPORTANT DEFINITIONS NEEDED FOR THIS FUNCTION TO WORK**********/////////////
/////////////////////////////////////////////////////////////////////////////////////////////////
  
  #define MAX_LAT_CHANGE 0.05 // In degrees (because the gps function returns this in degrees)
  #define MAX_LON_CHANGE 0.02 // In degrees this is smaller because the latitude in sweeden is smaller per degree
  #define MAX_ALT_CHANGE 300 // I think this is in meters, I believe thats what the gps outputs but can easily be changed to feet
  #define SIZE 10 // size of arrays that store values
  #define FIX 0x01
  uint8_t fixStatus; // gps fix status
  float latA, latB, latC, latAPrev, latBPrev, latCPrev;
  float lonA, lonB, lonC, lonAPrev, lonBPrev, lonCPrev;
  float altA, altB, altC, altAPrev, altBPrev, altCPrev;
  float alt[10]; 
  float latitude, longitude, Altitude;
  bool AWorking, BWorking, CWorking;

void CompareGPS() {
    latA = ;// fill in the latitude from cutter A
    latB = ;// fill in the latitude from cutter B
    latC = ;// fill in the latitude from the main gondola
    lonA = ;// fill in the longitude from cutter A
    lonB = ;// fill in the longitude from cutter B
    lonC = ;// fill in the longitude from the main gondola
    altA = ;// fill in the altitude from cutter A
    altB = ;// fill in the altitude from cutter B
    altC = ;// fill in the altitude from the main gondola
    
    AWorking = workingGPS(latAPrev, latA, lonAPrev, lonA, altAPrev, altA);
    BWorking = workingGPS(latBPrev, latB, lonBPrev, lonB, altBPrev, altB);
    CWorking = workingGPS(latCPrev, latC, lonCPrev, lonC, altCPrev, altC);
    
    if(fixStatus == FIX && CWorking) {
      CWorking = true;
    }
    else {
      CWorking = false;
    }

    if // None are working, do a linear regression
    else if(AWorking && !BWorking && !CWorking) { // A only working
      latitude = latA;
      longitude = lonA;
      Altitude = altA;
    }
    else if(!AWorking && BWorking && !CWorking) { // B only working
      latitude = latB;
      longitude = lonB;
      Altitude = altB;
    }
    else if(!AWorking && !BWorking && CWorking) { // C only working
      latitude = latC;
      longitude = lonC;
      Altitude = altC;
    }
    else if(AWorking && BWorking && !CWorking) { // A and B working
      latitude = (latA+latB)/2;
      longitude = (lonA+lonB)/2;
      Altitude = (altA+altB)/2;
    }
    else if(AWorking && !BWorking && CWorking) { // A and C working
      latitude = (latA+latC)/2;
      longitude = (lonA+lonC)/2;
      Altitude = (altA+altC)/2;
    }
    else if(!AWorking && BWorking && CWorking) { // B and C working
      latitude = (latB+latC)/2;
      longitude = (lonB+lonC)/2;
      Altitude = (altB+altC)/2;
    }
    else if(AWorking && BWorking && CWorking) { // A B and C working
      latitude = (latA+latB+latC)/3;
      longitude = (lonA+lonB+lonC)/3;
      Altitude = (altA+altB+altC)/3;
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
