// Resistor Cutaway Standalone System

// This is the code for the main flight computer, presumably flown with a Teensy 3.5 and an SD logger
// Communications devices still need to be decided and implemented

#define SERIAL_BUFFER_SIZE 32

// Libraries
#include <SPI.h>
#include <SoftwareSerial.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <UbloxGPS.h>
#include <RFM69.h>
#include <MS5611.h>
#include <Arduino.h>


// Pin Definitions
#define UBLOX_RX 2
#define UBLOX_TX 3
#define CUTTER_PIN 5
#define LED_SD 6
#define LED_GPS 7
#define CHIP_SELECT 8
#define ONE_WIRE_BUS 9
#define TWO_WIRE_BUS 10
#define PRESSURE_ANALOG_PIN A0

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

// Comms Addresses
#define NETWORKID 1                     // network within which all comms devices will operate
#define LOCAL_ADDRESS 3                 // internal network address of the main computer
#define MAINCOMP_ADDRESS 1              // network address for cutter A
#define CUTTERA_ADDRESS 2               // network address for cutter B
#define BROADCAST 255                   // network address to broadcast to all nodes

// Comms definitions
#define FREQUENCY  RF69_915MHZ           // change to RF69_433MHZ if using 433MHz devices
#define ENCRYPT   false                 // if set to true, all node will require an encrytion key to talk within the network
#define ENCRYPT_KEY "PAPAJOEKNOWSBEST"  // 16 byte key used for all nodes within the network
#define USEACK    false                 // if set to true, node will request acknowledgements when sending messages

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

// Velocity Boundaries
#define MAX_SA_RATE 375                 // maximum velocity (ft/min) that corresponds to a slow ascent state
#define MAX_FLOAT_RATE 100              // maximum velocity that corresponds to a float state, or minimum for a slow ascent state
#define MIN_FLOAT_RATE -100             // minimum velocity that corresponds to a float state, or maximum for a slow descent state
#define MIN_SD_RATE -600                // minimum velocity that corresponds to a slow desent state

#define PRESSURE_TIMER_ALTITUDE 70000   // altitude at which the pressure timer begins

// Time Stamps
unsigned long updateStamp = 0;
unsigned long cutStampB = 0;  
unsigned long gpsLEDStamp = 0;

// State Machine
uint8_t state; 
bool stateSwitched;
bool maxAltReached = false;
bool cutStatusB = false;
boolean cutterOnB = false;
String cutReasonB;
String stateString;

// GPS Variables
SoftwareSerial ubloxSerial(UBLOX_RX,UBLOX_TX);
UbloxGPS gps(&ubloxSerial);
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

//Dallas Digital Temp Sensors
OneWire oneWire1(ONE_WIRE_BUS);                 //Temperature sensor wire busses
OneWire oneWire2(TWO_WIRE_BUS);
DallasTemperature sensor1(&oneWire1);           //Temperature sensors
DallasTemperature sensor2(&oneWire2);
float t1,t2 = -127.00;                          //Temperature values

// RFM69 Comms Device
RFM69 radio;


void setup() {
  Serial.begin(9600);   // initialize serial monitor
  
  initGPS();            // initialze GPS

  initPressure();       // initialize pressure sensor

  initTemperatures();   // initialize temp sensoes

  pinMode(LED_SD,OUTPUT);
  pinMode(LED_GPS,OUTPUT);

  pinMode(CUTTER_PIN,OUTPUT);

}

void loop() {

  gps.update();

  if(millis() - updateStamp > UPDATE_INTERVAL) {  
    updateStamp = millis();
       
    updatePressure();   // update pressure data

    updateTemperatures(); // update temp data

    updateTelemetry();  // update GPS data

    //NEEDED: Function to read data from comms from cutter boxes
    
    stateMachine();     // update the state machine
  }

  // cut balloon if the master timer expires
  if(millis() > MASTER_INTERVAL*M2MS) {
    cutResistorOnB;
    cutReasonB = F("master timer expired");
  }

  if(millis() - cutStampB > CUT_INTERVAL && cutterOnB) cutResistorOffB();

  fixLEDSchema();

}
