void initRelays(){
  sensorHeatRelay.init(false);      //Initialize relays
  
  sensorHeat_Status = "OFF";
}

void actHeat(){  
  
/* 

  sensTemp = [[TEMP READING FROM THERMISTORS]];
  
  if(-100 < sensTemp && sensTemp < LOW_TEMP){
    coldSensor = true;                                              // Setting the coldsensor variable to true or false based on the temperature readings
  }
  if(sensTemp > HIGH_TEMP){
    coldSensor = false;
  }

  if(coldSensor && sensorHeatRelay.getState()==false){
    sensorHeatRelay.setState(true);                                 // Some logic values that set the state of the sensor heat relay to that of the sensed temperature
  }
  else if(!coldSensor && sensorHeatRelay.getState()==true){
    sensorHeatRelay.setState(false);                                // Some logic values that set the state of the sensor heat relay to that of the sensed temperature
  }*/
}