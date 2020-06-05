//============================================================================================================================================
// MURI Resistor Cutter Box A
// Written by PJ Collins and Steele Mitchell - coll0792 & mitc0596 Spring 2020
//============================================================================================================================================
//
// Can run autonomously or communicate with a main computer. Will cut when instructed, or on its own if no communication.
//
//=============================================================================================================================================

/*  Arduino Uno w/ PCB Shield pin connections:
     ----------------------------------------------
    | Component                    | Pins used     |    
    | ---------------------------------------------|
    | UBlox Neo m8n                | 0,1           |
    | LED                          | 7             | 
    | Bluetooth hc05               | 8,9           |
    | H-Driver                     | 10,11         |
    | Latching Relay               | 12,13         |
    | Thermistor                   | A0            | 
     ----------------------------------------------
*/

#define SERIAL_BUFFER_SIZE 32

// Libraries
#include <SPI.h>
#include <SoftwareSerial.h>
#include <UbloxGPS.h>
#include <LatchRelay.h> 
#include <Arduino.h>

 
// Pin Definitions
#define UBLOX_RX 0
#define UBLOX_TX 1
#define LED 7
#define BLUE_RX 8
#define BLUE_TX 9
#define CUTTER_PIN1 10
// #define CUTTER_PIN2 11 // not actually 11 but pick a pin -_/(o_o)\_-
#define HEAT_ON 12
#define HEAT_OFF 13
#define THERM_PIN A0


// Intervals
#define FIX_INTERVAL 5000               // GPS with a fix—will flash for 5 seconds
#define NOFIX_INTERVAL 2000             // GPS with no fix—will flash for 2 seconds
#define LED_INTERVAL 10000              // GPS LED runs on a 10 second loop
#define UPDATE_INTERVAL 1000            // update all data and the state machine every 1 second
#define CUT_INTERVAL 30000              // ensure the cutting mechanism is on for 30 seconds
#define MASTER_INTERVAL 135             // master timer that cuts balloon after 2hr, 15min
#define PRESSURE_TIMER_INTERVAL 50      // timer that'll cut the balloon 50 minutes after pressure reads 70k feet
#define ASCENT_INTERVAL 135             // timer that cuts balloon A 2 hours and 15 minutes after ASCENT state initializes
#define SLOW_DESCENT_INTERVAL 60        // timer that cuts both balloons (as a backup) an hour after SLOW_DESCENT state initializes

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
#define EASTERN_BOUNDARY -92            // longitudes
#define WESTERN_BOUNDARY -95
#define NORTHERN_BOUNDARY 45            // latitudes
#define SOUTHERN_BOUNDARY 42
#define SLOW_DESCENT_CEILING 110000     // max altitude stack can reach before balloon is cut and stack enters slow descent state
#define SLOW_DESCENT_FLOOR 80000        // min altitude for the slow descent state
#define INIT_ALTITUDE 5000              // altitude at which the state machine begins
#define RECOVERY_ALTITUDE 7000          // altitude at which the recovery state intializes on descent
#define MIN_TEMP -60                    // minimum acceptable internal temperature
#define MAX_TEMP 90                     // maximum acceptable interal temperature
#define LOW_TEMP -10                    // activation temp for heating pads
#define HIGH_TEMP 0                     // deactivation temp for heating pads

// Velocity Boundaries
#define MAX_SA_RATE 375                 // maximum velocity (ft/min) that corresponds to a slow ascent state
#define MAX_FLOAT_RATE 100              // maximum velocity that corresponds to a float state, or minimum for a slow ascent state
#define MIN_FLOAT_RATE -100             // minimum velocity that corresponds to a float state, or maximum for a slow descent state
#define MIN_SD_RATE -600                // minimum velocity that corresponds to a slow desent state

#define PRESSURE_TIMER_ALTITUDE 70000   // altitude at which the pressure timer begins

//Thermistor
#define C2K 273.15
#define ADC_MAX 8196                                                    // The maximum adc value given to the thermistor, should be 8196 for a teensy and 1024 for an Arduino
#define CONST_A 0.001125308852122
#define CONST_B 0.000234711863267                                       // A, B, and C are constants used for a 10k resistor and 10k thermistor for the steinhart-hart equation
#define CONST_C 0.000000085663516                                       // NOTE: These values change when the thermistor and/or resistor change value, so if that happens, more research needs to be done on those constants
#define CONST_R 10000       
#define THERMISTOR_A A1
#define THERMISTOR_B A2    
float t1 = -127.00;                                                    //Temperature initialization values
float t2 = -127.00;

// Time Stamps
unsigned long updateStamp = 0;
unsigned long cutStampA = 0,  cutStampB = 0;  
unsigned long LEDStamp = 0;

// State Machine
uint8_t state; 
bool stateSwitched;
bool maxAltReached = false;
bool cutStatusA = false;
bool cutterOnA = false;
String cutReasonA;
String stateString;

// GPS Variables
UbloxGPS gps(&Serial);
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
bool LEDOn = false;

// active heating variables
float sensTemp;
bool coldSensor = false;
LatchRelay sensorHeatRelay(HEAT_ON,HEAT_OFF);        //Declare latching relay objects and related logging variables
String sensorHeat_Status = "";

// Bluetooth comms variables
SoftwareSerial blueSerial(BLUE_RX, BLUE_TX); // initialize bluetooth serial
struct data{
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
}dataPacket;                                 // shortcut to create data object dataPacket

struct input{
  uint8_t startByte;
  uint8_t cutterTag;
  uint8_t command;
  // float pressure;
  uint16_t checksum;
  uint8_t stopByte;
}inputPacket;

// Autonomous operation variables
long timeOut;
bool autonomousNow = false;


void setup() {
  Serial.begin(9600);   // initialize serial monitor
  
  blueSerial.begin(9600); // initialize bluetooth serial communication
    
  initGPS();            // initialze GPS

  initRelays();        //Initialize Relays

  pinMode(LED,OUTPUT);

  pinMode(CUTTER_PIN1,OUTPUT);
  // pinMode(CUTTER_PIN2,OUTPUT);

}

void loop() {

  gps.update();

  if(millis() - updateStamp > UPDATE_INTERVAL) {   
    updateStamp = millis();

    updateTemperatures();
    
    actHeat();          //Controls active heating

    updateTelemetry();  // update GPS data
    
    stateMachine();     // update the state machine

    sendData();         // send current data to main

    readInstruction();  // read commands from main, cuts if instructed

    if( !readInstruction()) timeOut+= UPDATE_INTERVAL;
    if( timeOut > 2*M2MS) autonomousNow = true;
  }

  // cut balloon if the master timer expires
  if(millis() > MASTER_INTERVAL*M2MS) {
    cutResistorOnA();
    cutReasonA = F("master timer expired");
  }

  if(millis() - cutStampA > CUT_INTERVAL && cutterOnA) cutResistorOffA();

  fixLEDSchema();

}
