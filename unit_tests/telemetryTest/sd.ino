// tab containing SD functions

void initSD(){
  pinMode(chipSelect, OUTPUT);
  
  Serial.print("Initializing SD card...");
  
  if (!SD.begin(chipSelect))                                                            //Attempt to start SD communication
    Serial.println("Card failed, not present, or voltage supply is too low.");          //Print out error if failed; remind user to check card
  else {                                                                                //If successful, attempt to create file
    Serial.println("Card initialized successfully.\nCreating File...");
    for (byte i = 0; i < 100; i++) {                                                    //Can create up to 100 files with similar names, but numbered differently
      fileName[4] = '0' + i / 10;
      fileName[5] = '0' + i % 10;
      if (!SD.exists(fileName)) {                                                       //If a given filename doesn't exist, it's available
        dataLog = SD.open(fileName, FILE_WRITE);                                        //Create file with that name
        sdActive = true;                                                                //Activate SD logging since file creation was successful
        Serial.println("Logging to: " + String(fileName));                              //Tell user which file contains the data for this run of the program
        break;                                                                          //Exit the for loop now that we have a file
      }
    }
    if (!sdActive) Serial.println("No available file names; clear SD card to enable logging");
  }
  String header = F("minutes, gpsFix, latitude, longitude, altitude, nextLatitude, next Longitude, next_alt, ascentRate (ft/min), groundSpeed (ft/min), groundSpeed(mph), heading, satellites");
  //header += F(", PSI, ATM, pressureAnalog, pressureAltitude");
  if (sdActive) {
    dataLog.println(header);
    dataLog.close();
  }
}


void logData() {
  String data = "";

  // there's gotta be a better way to do this
  data = String(timeStamp[0]/(float)60000) + ", " + String(fixStatus);
  data += ", " + String(latitude[0],6) + ", " + String(longitude[0],6) + ", " + String(alt[0]);
  data += ", " + String(nextLat,6) + ", " + String(nextLong,6) + ", " + String(nextAlt) + ", " + String(ascentRate) + ", "+ String(groundSpeed) + ", " + String(groundSpeed/FPM_PER_MPH);
  data += ", " + String(heading*R2D,4) + ", " + String(sats);
  //data += "," + String(pressureAnalog) + ", " + String(pressurePSI) + ", " + String(pressurePSI*PSI_TO_ATM) + ", " + String(pressureAltitude);

  Serial.println(data);

  if(sdActive) {
    dataLog = SD.open(fileName, FILE_WRITE);
    delay(20);
    dataLog.println(data);
    dataLog.close();
  }
  
}
