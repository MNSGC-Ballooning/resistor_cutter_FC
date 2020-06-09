// Resistor Cutaway Standalone System

// This is the code for the main flight computer, presumably flown with a Teensy 3.5 and an SD logger
// Communications devices still need to be decided and implemented

// Libraries
#include <SPI.h>
#include <SD.h>
#include <UbloxGPS.h>
#include <MS5611.h>
#include <Arduino.h>




// Pin Definitions
#define UBLOX_RX 34
#define UBLOX_TX 33
#define CUTTER_PIN 5
#define LED_SD 6
#define LED_GPS 7
#define CHIP_SELECT 8
#define ONE_WIRE_BUS 28
#define TWO_WIRE_BUS 29
#define PRESSURE_ANALOG_PIN A0



// Constants
#define PA_TO_ATM 1/101325              // PSI to ATM conversion ratio
#define SEA_LEVEL_PSI 14.7              // average sea level pressure in PSI
#define M2MS 60000                      // milliseconds per minute
#define SIZE 10                         // size of arrays that store values
#define D2R PI/180                      // degrees to radians conversion
#define R2D 180/PI                      // radians to degrees conversion
#define SECONDS_PER_HOUR 3600           // seconds per hour
#define FPM_PER_MPH 88                  // feet per minute per mile per hour
#define FEET_PER_METER 3.28084          // feet per meter

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



#define PRESSURE_TIMER_ALTITUDE 70000   // altitude at which the pressure timer begins

#define blueSerialA Serial4
#define blueSerialB Serial3

struct data{                                   // setting up data structure for communication
  uint8_t startByte;
  uint8_t cutterTag;
  float latitude;
  float longitude;
  float Altitude;
  float AR;
  uint8_t cutStatus;
  uint8_t currentState;
  uint16_t checksum;
  uint8_t stopByte;
}dataPacketA;                                // shortcut to create data object
data dataPacketB; 

struct instruction{
  uint8_t startByte;
  uint8_t cutterTag;
  uint8_t Command;
  //float pressure;
  uint16_t checksum;
  uint8_t stopByte;
}instructionA;
instruction instructionB;

// Time Stamps
unsigned long updateStamp = 0;
unsigned long cutStampA = 0,  cutStampB = 0;  
unsigned long gpsLEDStamp = 0;


// SD Variables
File dataLog;
char fileName[] = "rCut00.csv";
bool sdActive = false;

// GPS Variables

UbloxGPS gps(&Serial5);
float alt[SIZE];                  // altitude in feet, also there exists a queue library we can use instead
unsigned long timeStamp[SIZE];    // time stamp array that can be used with alt array to return a velocity
float latitude[SIZE];
float longitude[SIZE];
bool fixFlag[SIZE];
uint8_t fixStatus[10];
float ascentRate;
float groundSpeed;
float heading;
uint8_t sats;
bool gpsLEDOn = false;

// MS5611 Pressure Sensor Variables
MS5611 baro;
float seaLevelPressure;         // in Pascals
float baroReferencePressure;    // some fun pressure/temperature readings below 
float baroTemp;                 // non-"raw" readings are in Pa for pressure, C for temp, meters for altitude
unsigned int pressurePa;
float pressureAltitude;
float pressureRelativeAltitude;
boolean baroCalibrated = false; // inidicates if the barometer has been calibrated or not

// Thermistors
float t1,t2 = -127.00;                          //Temperature values, will probably be thermistors!!!!!!!
float dt = 0;

void setup() {
  Serial.begin(9600);   // initialize serial monitor
  Serial4.begin(9600);
  while(!Serial4);
  Serial3.begin(9600);

  Serial4.println("Goodnight moon!");
  
  initGPS();            // initialze GPS

 // initPressure();       // initialize pressure sensor

  initTemperatures();   // initalize temperature sensors

  initSD();             // initialize SD card

  

  pinMode(LED_SD,OUTPUT);
  pinMode(LED_GPS,OUTPUT);

  pinMode(CUTTER_PIN,OUTPUT);

}

void loop() {

  gps.update();

  if(millis() - updateStamp > UPDATE_INTERVAL) { 
    updateStamp = millis();
      
    //updatePressure();   // update pressure data

    updateTemperatures(); // update temperature sensors

    updateTelemetry();  // update GPS data

    checkComms('A');
    checkComms('B');
    
    logData();          // log the data

    //NEEDED: Function to read data from comms from cutter boxes
    
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