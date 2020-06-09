// State machine functions for the cutaway system

#define INITIALIZATION 0x00   // corresponding hex values are chosen so as to avoid bit-flipping in the stratosphere
#define ASCENT 0x01
#define SLOW_ASCENT 0x02
#define SLOW_DESCENT 0x04
#define DESCENT 0x08
#define FLOAT 0x10
#define OUT_OF_BOUNDARY 0x20
#define TEMPERATURE_FAILURE 0x40
#define RECOVERY 0x80


void stateMachine() {
  // ensure the state machine does not start until a certain intial altitude is reached
  static bool initDone = false;
  static byte initCounter = 0;
  if(!initDone && fixStatus[0] == FIX) {
    state = INITIALIZATION;
    stateString = F("Initialization");
    //Serial.println(alt[0]);
    //Serial.println(gps.getAlt_feet());
    if(alt[0] > INIT_ALTITUDE) {
      initCounter++;
      if(initCounter >= 10) {
        initCounter = 0;
        initDone = true; 
      }
    }
  }

  // run state switch function if the state machine is intialized
  if (initDone) { 
    stateSwitch();
  }

  
  // run functions based off of the current state
  switch(state) {
    ///// Ascent /////
    case 0x01:
      stateString = F("Ascent");

      static unsigned long ascentStamp = millis();

      // cut balloon A if the ascent timer runs out
      if(millis() - ascentStamp > ASCENT_INTERVAL*M2MS) {
        cutResistorOn('a');
        cutReasonA = F("expired ascent timer");
      }
      // cut balloon A if the termination altitude is reached
      if (alt[0] > SLOW_DESCENT_CEILING) {
        cutResistorOn('a');
        cutReasonA = F("reached termination altitude");
      }
      else cutReasonA = F("0");

      break;

    ///// Slow Ascent /////
    case 0x02:
      // cut the resistor and note state
      stateString = F("Slow Ascent");

      // cut both balloons as the stack is ascending too slowly
      cutResistorOn('a');
      cutResistorOn('b');
      cutReasonA = F("slow ascent state");
      cutReasonB = F("slow ascent state");

      break;

    ///// Slow Descent /////      THIS STATE STILL NEEDS TO BE WORKED OUT
    case 0x04:
      // organize timing schema for slow descent state
      stateString = F("Slow Descent");

      static unsigned long slowDescentStamp = millis(); // initializaed upon first time in this state
      static byte SDTerminationCounter = 0;

      if(millis() - slowDescentStamp > SLOW_DESCENT_INTERVAL*M2MS || (alt[0] < SLOW_DESCENT_FLOOR && alt[0] != 0)) {
        SDTerminationCounter++;

        if(SDTerminationCounter >= 10) {
          cutResistorOn('a');
          cutResistorOn('b');
          cutReasonA = F("reached slow descent floor");
          cutReasonB = F("reached slow descent floor");
        }
      }

      break;

    ///// Descent /////
    case 0x08:
      // do nothing but note state
      stateString = F("Descent");

      static byte floorAltitudeCounter = 0;   // increments if the stack is below the slow descent altitude floor
      static bool cutCheck = false;

      if(alt[0] < SLOW_DESCENT_FLOOR && alt[0] != 0) {
        floorAltitudeCounter++;

        if(floorAltitudeCounter >= 10 && !cutCheck) {
          floorAltitudeCounter = 0;

          cutResistorOn('a');
          cutResistorOn('b');
          cutReasonA = F("descent state cut check");
          cutReasonB = F("descent state cut check");
        }
      }

      break;

    ///// Float /////
    case 0x10:
      // abort flight
      stateString = F("Float");

      // cut both balloons as the stack is in a float state
      cutResistorOn('a');
      cutResistorOn('b');
      cutReasonA = F("float state");
      cutReasonB = F("float state");      

      break;

    ///// Out of Boundary /////
    case 0x20:
      // cut resistor and note state
      stateString = F("Out of Boundary");
      
      // cut both balloons as the stack is out of the predefined flight boundaries
      cutResistorOn('a');
      cutResistorOn('b');
      // cut reasons are more specifically defined in the boundaryCheck() function

      break;

    ///// Temperature Failure /////
    case 0x40:
      // cut resistor and note state
      stateString = F("Temperature Failure");

      // cut balloon as temps are at critical levels
      cutResistorOn('a');
      cutResistorOn('b');

      // cut reasons defined within tempCheck() function

      break;

    ///// Recovery /////
    case 0x80:
      // reserved for any functions near the ground
      stateString = F("Recovery");

      break;

  }
  
}


void stateSwitch() {
  // initialize all counters as static bytes that begin at zero
  static byte ascentCounter = 0,  slowAscentCounter = 0,  descentCounter = 0, slowDescentCounter = 0, floatCounter = 0, recoveryCounter = 0, boundaryCounter = 0, tempCounter = 0;

  if(stateSwitched) {     // reset all state counters if the state was just switched
    ascentCounter = 0;  slowAscentCounter = 0;  descentCounter  = 0;  slowDescentCounter = 0;   floatCounter  = 0; recoveryCounter = 0;  boundaryCounter = 0, tempCounter = 0;
    stateSwitched = false;
  }

  if(ascentRate > MAX_SA_RATE && state != ASCENT) {
    ascentCounter++;
    if(ascentCounter >= 40) {
      state = ASCENT;
      ascentCounter = 0;
      stateSwitched = true;
    }
  }
  else if(ascentRate <= MAX_SA_RATE && ascentRate > MAX_FLOAT_RATE && state != SLOW_ASCENT) {
    slowAscentCounter++;
    if(slowAscentCounter >= 40) {
      state = SLOW_ASCENT;
      slowAscentCounter = 0;
      stateSwitched = true;
    }
  }
  else if(ascentRate <= MAX_FLOAT_RATE && ascentRate >= MIN_FLOAT_RATE && state != FLOAT) {
    floatCounter++;
    if(floatCounter >= 600) {
      state = FLOAT;
      floatCounter = 0;
      stateSwitched = true;
    }
  }
  else if(ascentRate < MIN_FLOAT_RATE && ascentRate >= MIN_SD_RATE && state != SLOW_DESCENT) {
    slowDescentCounter++;
    if(slowDescentCounter >= 40) {
      state = SLOW_DESCENT;
      slowDescentCounter = 0;
      stateSwitched = true;
    }
  }
  else if(ascentRate < MIN_SD_RATE && state !=DESCENT) {
    descentCounter++;
    if(descentCounter >= 40) {
      state = DESCENT;
      descentCounter = 0;
      stateSwitched = true;
    }
  }
  else if(state != RECOVERY && (state == DESCENT || state == SLOW_DESCENT) && alt[0] < RECOVERY_ALTITUDE) {
    recoveryCounter++;
    if(recoveryCounter >= 40) {
      state = RECOVERY;
      recoveryCounter = 0;
      stateSwitched = true;
    }
  }

  // part of a separate series of if/else statements as criteria for this state is different
  if(boundaryCheck() && state != OUT_OF_BOUNDARY) {
    boundaryCounter++;
    if(boundaryCounter >= 40) {
      state = OUT_OF_BOUNDARY;
      boundaryCounter = 0;
      stateSwitched = true;
    }
  } 

  if(tempCheck() && state != TEMPERATURE_FAILURE)  {
    tempCounter++;
    if(tempCounter >= 40) {
      state = TEMPERATURE_FAILURE;
      tempCounter  = 0;
      stateSwitched = true;
    }
  }  
}
