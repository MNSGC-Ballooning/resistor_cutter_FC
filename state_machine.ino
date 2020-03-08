// State machine functions for the cutaway system

#define INITIALIZATION 0x00
#define ASCENT 0x01
#define SLOW_ASCENT 0x02
#define DESCENT 0x04
#define OUT_OF_BOUNDARY 0x08

void stateMachine() {
  static bool initDone = false;
  static byte initCounter = 0;

  // ensure the state machine does not start until a certain intial altitude is reached
  if(!initDone && fixStatus == FIX) {
    state = INITIALIZATION;
    stateString = F("Initialization");
    if(alt[0] > INIT_ALTITUDE) {
      initCounter++;
      if(initCounter >= 10) {
        initCounter = 0;
        initDone = true; 
      }
    }
  }

  // run state switch function if the state machine is intialized
  if (initDone && fixStatus == FIX) { 
    stateSwitch();
  }

  

  switch(state) {
    // Ascent
    case 0x01:
      // do nothing but note the state
      stateString = F("Ascent");

      break;

    // Slow Ascent
    case 0x02:
      // cut the resistor and note state
      stateString = F("Slow Ascent");
      cutResistorOn();

      break;

    // Descent
    case 0x04:
      // do nothing but note state
      stateString = F("Descent");

      break;

    // Out of boundary
    case 0x08:
      // cut resistor and note state
      stateString = F("Out of Boundary");
      cutResistorOn();

      break;

  }
  
}


void stateSwitch() {
  static byte ascentCounter = 0;
  static byte slowAscentCounter = 0;
  static byte descentCounter = 0;
  static byte boundaryCounter = 0;

  if(stateSwitched) {     // reset all states if the state was just switched
    ascentCounter = 0;
    slowAscentCounter = 0;
    descentCounter  = 0;
    boundaryCounter = 0;
    stateSwitched = false;
  }

  if(ascentRate > 300 && state != ASCENT) {
    ascentCounter++;
    if(ascentCounter >= 5) {
      state = ASCENT;
      ascentCounter = 0;
      stateSwitched = true;
    }
  }

  if(ascentRate <= 300 && ascentRate > 0 && state != SLOW_ASCENT) {
    slowAscentCounter++;
    if(slowAscentCounter >= 5) {
      state = SLOW_ASCENT;
      cutReason = F("slow ascent state");
      slowAscentCounter = 0;
      stateSwitched = true;
    }
  }

  if(ascentRate < 0 && state !=DESCENT) {
    descentCounter++;
    if(descentCounter >= 5) {
      state = DESCENT;
      descentCounter = 0;
      stateSwitched = true;
    }
  }

  if(boundaryCheck() && state != OUT_OF_BOUNDARY) {
    boundaryCounter++;
    if(boundaryCounter >= 5 ) {
      state = OUT_OF_BOUNDARY;
      boundaryCounter = 0;
      stateSwitched = true;
    }
  }
  
}


// function to check if the payload is out of the flight boundaries
bool boundaryCheck() {
  if (longitude > EASTERN_BOUNDARY) {
    cutReason = F("reached eastern boundary");
    return true;
  }
  else if (longitude < WESTERN_BOUNDARY) {
    cutReason = F("reached western boundary");
    return true;
  }
  else if (latitude > NORTHERN_BOUNDARY) {
    cutReason = F("reached northern boundary");
    return true;
  }
  else if (latitude < SOUTHERN_BOUNDARY) {
    cutReason = F("reached southern boundary");
    return true; 
  }
  else if (alt[0] > MAX_ALTITUDE) {
    cutReason = F("reached termination altitude");
    return true;
  }
  else {
    return false;
  }
  
}
