// Resistor Cutaway Standalone System

// OTHERWISE KNOWN AS BORIS

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
#define MASTER_INTERVAL 8160000         // master timer that cuts balloon after 2hr, 15min
#define PRESSURE_TIMER_INTERVAL 3000000 // timer that'll cut the balloon 50 minutes after pressure reads 70k feet

// Constants
#define PSI_TO_ATM 0.068046   // PSI to ATM conversion ratio
#define SEA_LEVEL_PSI 14.7    // average sea level pressure in PSI

// Fix statuses
#define NOFIX 0x00
#define FIX 0x01

// Boundaries
///////CHANGE BEFORE EACH FLIGHT////////
#define EASTERN_BOUNDARY -92
#define WESTERN_BOUNDARY -95
#define NORTHERN_BOUNDARY 45
#define SOUTHERN_BOUNDARY 42
#define MAX_ALTITUDE 110000
#define INIT_ALTITUDE 5000

// Time Stamps
unsigned long updateStamp = 0;
unsigned long cutStamp = 0;
unsigned long pressureStamp = 0;
unsigned long gpsLEDStamp = 0;

// State Machine
uint8_t state; 
bool stateSwitched;
String cutReason;
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


boolean cutterOn = false;

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
  if(millis() > MASTER_INTERVAL) {
    cutResistorOn();
    cutReason = F("master timer expired");
  }

  // cut balloon if the pressure timer expires
  if(millis() - pressureStamp > PRESSURE_TIMER_INTERVAL) {
    cutResistorOn();
    cutReason = F("pressure timer expired");
  }

  if(millis() - cutStamp > CUT_INTERVAL && cutterOn) cutResistorOff();

  fixLEDSchema();

}
