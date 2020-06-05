
void initTemperatures() {

}


void updateTemperatures() {

}


bool checkTempReading(float temp) {
  // function to check if a temp sensor's recorded temperatures is valid
  if(temp < -120) { //-127 degrees C designates a temp sensor issue, so check for that (with some room for error)
    return false;
  }
  else {
    return true;
  }
}


bool tempCheck() {
  // return true if temperatures are at critical levels
  // there's probably a better way to work through this logic
  if(checkTempReading(t1) && checkTempReading(t2)){
    // if both temp sensors are working
    if(t1 < MIN_TEMP && t2 < MIN_TEMP) {
      cutReasonA = F("below min temp");
      cutReasonB = F("below min temp");
      return true;
    }
    else if(t1 > MAX_TEMP && t2 > MAX_TEMP) {
      cutReasonA = F("above max temp");
      cutReasonB = F("above max temp");
      return true;
    }
    else {
      return false ;       
    }
  }

  if(checkTempReading(t1) && !checkTempReading(t2)){
    // if temp sensor 1 is working
    if(t1 < MIN_TEMP) {
      cutReasonA = F("below min temp");
      cutReasonB = F("below min temp");
      return true;
    }
    else if(t1 > MAX_TEMP) {
      cutReasonA = F("above max temp");
      cutReasonB = F("above max temp");
      return true;
    }
    else {
      return false ;       
    }
  }

  if(!checkTempReading(t1) && checkTempReading(t2)){
    // if temp sensor 2 is working
    if(t2 < MIN_TEMP) {
      cutReasonA = F("below min temp");
      cutReasonB = F("below min temp");
      return true;
    }
    else if(t2 > MAX_TEMP) {
      cutReasonA = F("above max temp");
      cutReasonB = F("above max temp");
      return true;
    }
    else {
      return false ;       
    }
  }

  if(!checkTempReading(t1) && !checkTempReading(t2)){
    // neither temp sensor is working
    return false;
  }
return false;
}
