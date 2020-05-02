// SD functions

void initSD(){
  pinMode(CHIP_SELECT, OUTPUT);
  
  Serial.print("Initializing SD card...");
  
  if (!SD.begin(CHIP_SELECT))                                                           //Attempt to start SD communication
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
  String header = F("minutes, gpsFix, latitude, longitude, altitude, ascentRate, satellites, PSI, ATM, pressureAnalog, pressureAltitude,");
  header+= F("state, stateValue, cutReason, cutterState, cutStamp, pressureStamp");
  if (sdActive) {
    dataLog.println(header);
    dataLog.close();
  }
}

void logData() {
  String data = "";

  // there's gotta be a better way to do this
  data = String(timeStamp[0]/(float)60000) + ", " + String(fixStatus) + ", ";
  data += String(latitude[0]) + ", " + String(longitude[0]) + ", " + String(alt[0]) + ", " + String(ascentRate) + ", " + String(sats) + ", ";
  data += String(pressureAnalog) + ", " + String(pressurePSI) + ", " + String(pressurePSI*PSI_TO_ATM) + ", " + String(pressureAltitude) + ", ";
  data += stateString + ", " + String(state) + ", " + cutReasonA + ", " + cutReasonB + ", " + String(cutterOnA) + ", " + String(cutterOnB) + ", ";
  data += String(cutStampA) + ", " + String(cutStampB) + ", " + String(pressureStamp);

  Serial.println(data);

  if(sdActive) {
    dataLog = SD.open(fileName, FILE_WRITE);
    digitalWrite(LED_SD,HIGH);
    delay(20);
    dataLog.println(data);
    dataLog.close();
    digitalWrite(LED_SD,LOW);
  }
  
}
