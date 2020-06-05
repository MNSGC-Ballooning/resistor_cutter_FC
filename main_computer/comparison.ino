  // The definitions are to see what I used. These will have to be changed to the main code once you implement them Steele
  #define LAT_LIMIT 0.05; // In degrees (because the gps function returns this in degrees)
  #define INITIALIZATION 0x00;
  #define LON_LIMIT 0.02; // In degrees this is smaller because the latitude in sweeden is smaller per degree
  #define ALT_LIMIT 100; // I think this is in meters, I believe thats what the gps outputs but can easily be changed to feet
  #define CUT_A = 0x15;
  #define CUT_B = 0x25;
  #define CUT_C = 0x35;
  #define GPS_FAILURE 0x45 // I made C and FAILURE whatever I wanted so these can be changed as well
  
  byte gpsCounter = 0;
  uint8_t gpsLatSTATE = INITIALIZATION;
  uint8_t gpsLonSTATE = INITIALIZATION;
  uint8_t gpsAltSTATE = INITIALIZATION;

  //The code above is the data recieved from the computer A and B, with C denoting the main gondolas data. These can be changed to better fit the variables
  float latA = dataPacketA.latitude;
  float latB = dataPacketB.latitude;
  float latC = gps.getLat();
  float lonA = dataPacketA.longitude; 
  float lonB = dataPacketB.longitude;
  float lonC = gps.getLon();
  float altA = dataPacketA.Altitude;
  float altB = dataPacketB.Altitude;
  float altC = gps.getAlt_feet();

  // This is used for the difference between data measurements and will be evaluated later
  float latAB, latBC, latCA;
  float lonAB, lonBC, lonCA;
  float altAB, altBC, altCA;

  // These are used to describe the current state of the system: N for no problem (which is the default). A for data A is not correct, B for B is not correct, and C for C is not correct.
  // X is used if two or more GPS' go out, in which case we wont know which is correct and I believe we must cut at that point.
  char faultylat = 'N';
  char faultylon = 'N';
  char faultyalt = 'N';

  // These are used for the final calculation of the latitude, longitude, and altitude. They will be an average of the correct ones left.
  float LAT;
  float LON;
  float ALT;

  // These are for the counters if we get a faulty measurement for any of them (getting an X)
  byte gpsCounterLat = 0;
  byte gpsCounterLon = 0;
  byte gpsCounterAlt = 0;
  
  // This is the reason the box had to cut the system
  String reason;
  String CurrentState = "Everything is fine";
  

void CompareGPS() {

    
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

    if( (((latA && (latB || latC)) > WESTERN_BOUNDARY) || ((latB && latC) > WESTERN_BOUNDARY)) WestRequests+=1; // if any two readings exceed West bound, requests cut
    else if ((((latA && (latB || latC)) < WESTERN_BOUNDARY) || ((latB && latC) < WESTERN_BOUNDARY)) WestReqests = 0; // resets request counter if two disagree
    if( ((latA && (latB || latC)) < EASTERN_BOUNDARY ) || ((latB && latC)) < EASTERN_BOUNDARY)) EastRequests+=1;
    else if( ((latA && (latB || latC)) > EASTERN_BOUNDARY ) || ((latB && latC)) > EASTERN_BOUNDARY)) EastRequests=0;

  
    /*if(latAB>LAT_LIMIT && latBC>LAT_LIMIT && latCA<LAT_LIMIT) {
      faultylat = 'B';
      LAT = (latA+latC)/2;
      gpsLatSTATE = CUT_B;
      CurrentState = "Getting faulty measurements for the B system latitude";
      reason = "Using an average of A and C measurements for latitude";
    }
    else if(latAB>LAT_LIMIT && latBC<LAT_LIMIT && latCA>LAT_LIMIT) {
      faultylat = 'A';
      LAT = (latB+latC)/2;
      gpsLatSTATE = CUT_A;
      CurrentState = "Getting faulty measurements for the A system latitude";
      reason = "Using an average of B and C measurements for latitude";
    }
    else if(latAB<LAT_LIMIT && latBC>LAT_LIMIT && latCA>LAT_LIMIT) {
      faultylat = 'C';
      LAT = (latA+latB)/2;
      gpsLatSTATE = CUT_C;
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
    }*/





    /////////////////////////////////////
    //////////// Longitude //////////////
    /////////////////////////////////////
    
    if( (((lonA && (lonB || lonC)) > NORTHERN_BOUNDARY) || ((lonB && lonC) > NORTHERN_BOUNDARY)) NorthRequests+=1; // if any two readings exceed north bound, requests cut
    else if ((((lonA && (lonB || lonC)) < NORTHERN_BOUNDARY) || ((lonB && lonC) < NORTHERN_BOUNDARY)) NorthReqests = 0; // resets request counter if two disagree
    if( ((lonA && (lonB || lonC)) < SOUTHERN_BOUNDARY ) || ((lonB && lonC)) < SOUTHERN_BOUNDARY)) SouthRequests+=1;
    else if( ((lonA && (lonB || lonC)) > SOUTHERN_BOUNDARY ) || ((lonB && lonC)) > SOUTHERN_BOUNDARY)) SouthRequests=0;

    if( (EastRequests >= 10) || (WestRequests >= 10) || (NorthRequests >= 10) || (SouthRequests >= 10)){ // if 10 consecutive requests have been received, cut
      cutResistorOn('a');
      cutResistorOn('b');
    }
  
    /*// Because we are in Sweden, the longitudes are closer together, and therefore the tolerance can be smaller for them
    // This is the longitude buffer
    if(lonAB>LON_LIMIT && lonBC>LON_LIMIT && lonCA<LON_LIMIT) {
      faultylon = 'B';
      LON = (lonA+lonC)/2;
      gpsLonSTATE = CUT_B;
      CurrentState = "Getting faulty measurements for the B system longitude";
      reason = "Using an average of A and C measurements for longitude";
    }
    else if(lonAB>LON_LIMIT && lonBC<LON_LIMIT && lonCA>LON_LIMIT) {
      faultylon = 'A';
      LON = (lonB+lonC)/2;
      gpsLonSTATE = CUT_A;
      CurrentState = "Getting faulty measurements for the A system longitude";
      reason = "Using an average of B and C measurements for longitude";
    }
    else if(lonAB<LON_LIMIT && lonBC>LON_LIMIT && lonCA>LON_LIMIT) {
      faultylon = 'C';
      LON = (lonA+lonB)/2;
      gpsLonSTATE = CUT_C;
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
    }*/
}



void compareAlt(){    
    /////////////////////////////////////
    //////////// Altitude ///////////////
    /////////////////////////////////////

    
    
   /* if(altAB>ALT_LIMIT && altBC>ALT_LIMIT && altAC<ALT_LIMIT) {
      faultyalt = 'B';
      ALT = (altA+altC)/2;
      gpsSTATE = CUT_B;
      CurrentState = "Getting faulty measurements for the B system altitude";
      reason = "Using an average of A and C measurements for altitude";
    }
    else if(altAB>ALT_LIMIT && altBC<ALT_LIMIT && altAC>ALT_LIMIT) {
      faultyalt = 'A';
      ALT = (altB+altC)/2;
      gpsAltSTATE = CUT_A;
      CurrentState = "Getting faulty measurements for the A system altitude";
      reason = "Using an average of B and C measurements for altitude";
    }
    else if(altAB<ALT_LIMIT && altBC>ALT_LIMIT && altAC>ALT_LIMIT) {
      faultyalt = 'C';
      ALT = (altA+altB)/2;
      gpsAltSTATE = CUT_C;
      CurrentState = "Getting faulty measurements for the C system altitude";
      reason = "Using an average of A and B measurements for altitude";
      
    }
    else if(altAB>ALT_LIMIT && altBC>ALT_LIMIT && altAC>ALT_LIMIT) {
      faultyalt = 'X'; // This is the case in which two or more GPS' go out. In this case, there is no way to tell which GPS is correct and we must terminate immediately
      ALT = 0;
      gpsCounterAlt++;
      CurrentState = "Two or more wrong altitude measurements. Will cut soon";
      if(gpsCounterAlt == 20) {
        gpsAltSTATE = GPS_FAILURE;
        reason = "The altitude measurements were faulty and needed to be cut";
      }
    }*/
}

void compareAR(){
  // compares Ascent rates
  // if stack ascending too slowly, need to cut away both balloons
}
