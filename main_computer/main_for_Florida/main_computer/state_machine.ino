// State machine functions for the cutaway system

////**** VERY IMPORTANT VARIABLES THAT YOU WILL NEED TO INCORPORATE ****\\\\
uint8_t fixStatus; // gps fix status
float ascentRate; // current ascent rate (in feet/s)
float temp; // internal temperature
// Cut command functions for cutter computers A and B --> fill in where labeled in stateMachine()
// Make sure to update Boundaries!!!


// States
#define INITIALIZATION 0x00   // corresponding hex values are chosen so as to avoid bit-flipping in the stratosphere
#define ASCENT 0x01
#define SLOW_ASCENT 0x02
#define SLOW_DESCENT 0x04
#define DESCENT 0x08
#define FLOAT 0x10
#define OUT_OF_BOUNDARY 0x20
#define TEMPERATURE_FAILURE 0x40
#define RECOVERY 0x80

// Fix statuses
#define NOFIX 0x00
#define FIX 0x01

// Boundaries
///////CHANGE BEFORE EACH FLIGHT////////
#define EASTERN_BOUNDARY 40.0            // longitudes
#define WESTERN_BOUNDARY -2.0
#define NORTHERN_BOUNDARY 77.0            // latitudes
#define SOUTHERN_BOUNDARY 50.0
#define SLOW_DESCENT_CEILING 110000.0     // max altitude stack can reach before balloon is cut and stack enters slow descent state
#define SLOW_DESCENT_FLOOR 80000.0        // min altitude for the slow descent state
#define INIT_ALTITUDE 5000.0              // altitude at which the state machine begins
#define RECOVERY_ALTITUDE 7000.0          // altitude at which the recovery state intializes on descent
#define MIN_TEMP -60.0                    // minimum acceptable internal temperature
#define MAX_TEMP 90.0                     // maximum acceptable interal temperature

// Velocity Boundaries
#define MAX_SA_RATE 375                 // maximum velocity (ft/min) that corresponds to a slow ascent state
#define MAX_FLOAT_RATE 100              // maximum velocity that corresponds to a float state, or minimum for a slow ascent state
#define MIN_FLOAT_RATE -100             // minimum velocity that corresponds to a float state, or maximum for a slow descent state
#define MIN_SD_RATE -600                // minimum velocity that corresponds to a slow desent state

// Intervals
#define FIX_INTERVAL 5000               // GPS with a fix—will flash for 5 seconds
#define NOFIX_INTERVAL 2000             // GPS with no fix—will flash for 2 seconds
#define GPS_LED_INTERVAL 10000          // GPS LED runs on a 10 second loop
#define UPDATE_INTERVAL 2000            // update all data and the state machine every 4 seconds
#define CUT_INTERVAL 30000              // ensure the cutting mechanism is on for 30 seconds
#define MASTER_INTERVAL 135             // master timer that cuts balloon after 2hr, 15min
#define PRESSURE_TIMER_INTERVAL 50      // timer that'll cut the balloon 50 minutes after pressure reads 70k feet
#define ASCENT_INTERVAL 135             // timer that cuts balloon A 2 hours and 15 minutes after ASCENT state initializes
#define SLOW_DESCENT_INTERVAL 60        // timer that cuts both balloons (as a backup) an hour after SLOW_DESCENT state initializes

// State Machine
uint8_t state; 
bool stateSwitched;
bool maxAltReached = false;
bool cutStatusA = false, cutStatusB = false;
bool cutterOnA = false,  cutterOnB = false;
String cutReasonA,  cutReasonB;
String stateString;

void stateMachine() {
  // ensure the state machine does not start until a certain intial altitude is reached
  static bool initDone = false;
  static byte initCounter = 0;
  if(!initDone && fixStatus == FIX) { 
    state = INITIALIZATION;
    stateString = F("Initialization");
    if(Altitude > INIT_ALTITUDE) {
      initCounter++;
      if(initCounter >= 10) {
        initCounter = 0;
        initDone = true; 
      }
    }
  }

  // run state switch function if the state machine is intialized
  if (initDone) { 
    CompareGPS();
    stateSwitch();
  }

  
  // run functions based off of the current state
  switch(state) {
    ///// Ascent /////
    case 0x01:
      stateString = F("Ascent");

      static unsigned long ascentStamp = millis();

      // cut balloon A if the ascent timer runs out
      if(millis() - ascentStamp > ASCENT_INTERVAL*60000) {
        //******************* SEND CUT COMMAND TO CUTTER COMPUTER A ********************\\\\\\\\\\\\\
        cutReasonA = F("expired ascent timer");
      }
      // cut balloon A if the termination altitude is reached
      if (Altitude > SLOW_DESCENT_CEILING) {
        //******************* SEND CUT COMMAND TO CUTTER COMPUTER A *********************\\\\\\\\\\\\
        cutReasonA = F("reached termination altitude");
      }
      else cutReasonA = F("0");

      break;

    ///// Slow Ascent /////
    case 0x02:
      // cut the resistor and note state
      stateString = F("Slow Ascent");

      // cut both balloons as the stack is ascending too slowly
      //******************* SEND CUT COMMAND TO CUTTER COMPUTERS A AND B ********************\\\\\\\\\\\\\
      cutReasonA = F("slow ascent state");
      cutReasonB = F("slow ascent state");

      break;

    ///// Slow Descent /////      
    case 0x04:
      // organize timing schema for slow descent state
      stateString = F("Slow Descent");

      static unsigned long slowDescentStamp = millis(); // initializaed upon first time in this state
      static byte SDTerminationCounter = 0;

      if(millis() - slowDescentStamp > SLOW_DESCENT_INTERVAL*60000 || (Altitude < SLOW_DESCENT_FLOOR && Altitude != 0)) {
        SDTerminationCounter++;

        if(SDTerminationCounter >= 10) {
          //******************* SEND CUT COMMAND TO CUTTER COMPUTERS A AND B ********************\\\\\\\\\\\\\
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

      if(Altitude < SLOW_DESCENT_FLOOR && Altitude != 0) {
        floorAltitudeCounter++;

        if(floorAltitudeCounter >= 10 && !cutCheck) {
          floorAltitudeCounter = 0;

          //******************* SEND CUT COMMAND TO CUTTER COMPUTERS A AND B ********************\\\\\\\\\\\\\
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
      c//******************* SEND CUT COMMAND TO CUTTER COMPUTERS A AND B ********************\\\\\\\\\\\\\
      cutReasonA = F("float state");
      cutReasonB = F("float state");      

      break;

    ///// Out of Boundary /////
    case 0x20:
      // cut resistor and note state
      stateString = F("Out of Boundary");
      
      // cut both balloons as the stack is out of the predefined flight boundaries
      //******************* SEND CUT COMMAND TO CUTTER COMPUTERS A AND B ********************\\\\\\\\\\\\\
      // cut reasons are more specifically defined in the boundaryCheck() function

      break;

    ///// Temperature Failure /////
    case 0x40:
      // cut resistor and note state
      stateString = F("Temperature Failure");

      // cut balloon as temps are at critical levels
      //******************* SEND CUT COMMAND TO CUTTER COMPUTERS A AND B ********************\\\\\\\\\\\\\
      
      cutReasonA = F("Temperature failure");
      cutReasonB = F("Temperature failure");

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
  else if(state != RECOVERY && (state == DESCENT || state == SLOW_DESCENT) && Altitude < RECOVERY_ALTITUDE) {
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

  if(!(MIN_TEMP<temp<MAX_TEMP) && state != TEMPERATURE_FAILURE)  {
    tempCounter++;
    if(tempCounter >= 40) {
      state = TEMPERATURE_FAILURE;
      tempCounter  = 0;
      stateSwitched = true;
    }
  }  
}

bool boundaryCheck() {
  // function to check if the payload is out of the flight boundaries
  if (longitude > EASTERN_BOUNDARY) {
    cutReasonA = F("reached eastern boundary");
    cutReasonB = F("reached eastern boundary");
    return true;
  }
  else if (longitude < WESTERN_BOUNDARY) {
    cutReasonA = F("reached western boundary");
    cutReasonB = F("reached western boundary");
    return true;
  }
  else if (latitude > NORTHERN_BOUNDARY) {
    cutReasonA = F("reached northern boundary");
    cutReasonB = F("reached northern boundary");
    return true;
  }
  else if (latitude < SOUTHERN_BOUNDARY) {
    cutReasonA = F("reached southern boundary");
    cutReasonB = F("reached southern boundary");
    return true; 
  }
  else {
    return false;
  }
}
