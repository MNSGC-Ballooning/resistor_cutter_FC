// Resistor Cutaway Standalone System

#define SERIAL_BUFFER_SIZE 32

// Libraries
#include <SD.h>
#include <SoftwareSerial.h>
#include <UbloxGPS.h>
#include <Arduino.h>


// Pin Definitions
#define UBLOX_RX 2
#define UBLOX_TX 3
#define CUTTER_PIN 5
#define LED_SD 6
#define LED_GPS 7
#define CHIP_SELECT 8
#define PRESSURE_ANALOG_PIN A0

// Intervals
#define FIX_INTERVAL 2000               // GPS with a fix—will flash for 2 seconds
#define NOFIX_INTERVAL 5000             // GPS with no fix—will flash for 5 seconds
#define GPS_LED_INTERVAL 10000          // GPS LED runs on a 10 second loop
#define UPDATE_INTERVAL 4000            // update all data and the state machine every 4 seconds
#define CUT_INTERVAL 30000              // ensure the cutting mechanism is on for 30 seconds
#define MASTER_INTERVAL 135             // master timer that cuts balloon after 2hr, 15min
#define PRESSURE_TIMER_INTERVAL 50      // timer that'll cut the balloon 50 minutes after pressure reads 70k feet
#define ASCENT_INTERVAL 120             // timer that cuts balloon A 2 hours after ASCENT state initializes
#define SLOW_DESCENT_INTERVAL 60        // timer that cuts both balloons (as a backup) an hour after SLOW_DESCENT state initializes

// Constants
#define PSI_TO_ATM 0.068046             // PSI to ATM conversion ratio
#define SEA_LEVEL_PSI 14.7              // average sea level pressure in PSI
#define M2MS 60000                      // milliseconds per minute

// Fix statuses
#define NOFIX 0x00
#define FIX 0x01

// Boundaries
///////CHANGE BEFORE EACH FLIGHT////////
#define EASTERN_BOUNDARY -92            // longitudes
#define WESTERN_BOUNDARY -95
#define NORTHERN_BOUNDARY 45            // latitudes
#define SOUTHERN_BOUNDARY 42
#define SLOW_DESCENT_CEILING 110000     // max altitude stack can reach before balloon is cut and stack enters slow descent state
#define SLOW_DESCENT_FLOOR 80000        // min altitude for the slow descent state
#define INIT_ALTITUDE 5000              // altitude at which the state machine begins
#define RECOVERY_ALTITUDE 7000          // altitude at which the recovery state intializes on descent

// Velocity Boundaries
#define MAX_SA_RATE 375                 // maximum velocity (ft/min) that corresponds to a slow ascent state
#define MAX_FLOAT_RATE 100              // maximum velocity that corresponds to a float state, or minimum for a slow ascent state
#define MIN_FLOAT_RATE -100             // minimum velocity that corresponds to a float state, or maximum for a slow descent state
#define MIN_SD_RATE -600                // minimum velocity that corresponds to a slow desent state

#define PRESSURE_TIMER_ALTITUDE 70000   // altitude at which the pressure timer begins

// Time Stamps
unsigned long updateStamp = 0;
unsigned long cutStampA = 0,  cutStampB = 0;  
unsigned long pressureStamp = 0;
unsigned long gpsLEDStamp = 0;

// State Machine
uint8_t state; 
bool stateSwitched;
bool maxAltReached = false;
bool cutStatusA = false, cutStatusB = false;
String cutReasonA,  cutReasonB;
String stateString;

// SD Variables
File dataLog;
char fileName[] = "rCut00.csv";
bool sdActive = false;

// GPS Variables
SoftwareSerial ubloxSerial(UBLOX_RX,UBLOX_TX);
UbloxGPS gps(&ubloxSerial);
float alt[10];                  // altitude in feet, also there exists a queue library we can use instead
unsigned long timeStamp[10];    // time stamp array that can be used with alt array to return a velocity
float latitude;
float longitude;
float ascentRate;
uint8_t sats;
uint8_t fixStatus = NOFIX;
bool gpsLEDOn = false;

// Pressure Sensor Variables
int pressureAnalog;
float pressurePSI;
float pressureAltitude = 0;         // back up altitude to be used when things get desperate


boolean cutterOnA = false,  cutterOnB = false;

void setup() {
  Serial.begin(9600);   // initialize serial monitor
  
  initGPS();            // initialze GPS

  initSD();             // initialize SD card

  pinMode(LED_SD,OUTPUT);
  pinMode(LED_GPS,OUTPUT);

  pinMode(CUTTER_PIN,OUTPUT);

}

void loop() {

  if(millis() - updateStamp > UPDATE_INTERVAL) {
    updateGPS();        // update GPS data
    
    updatePressure();   // update pressure data
    
    logData();          // log the data
    
    stateMachine();     // update the state machine
  }

  // cut balloon if the master timer expires
  if(millis() > MASTER_INTERVAL*M2MS) {
    cutResistorOn('a');
    cutResistorOn('b');
    cutReasonA = F("master timer expired");
    cutReasonB = F("master timer expired");
  }

  if(millis() - cutStampA > CUT_INTERVAL && cutterOnA) cutResistorOff('a');
  if(millis() - cutStampB > CUT_INTERVAL && cutterOnB) cutResistorOff('b');

  fixLEDSchema();

}
