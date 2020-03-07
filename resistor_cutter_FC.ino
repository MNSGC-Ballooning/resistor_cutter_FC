// Resistor Cutaway Standalone System

// OTHERWISE KNOWN AS BORIS

// Libraries
#include <SD.h>
#include <SoftwareSerial.h>
#include <UbloxGPS.h>
#include <Arduino.h>

// Pin Definitions
#define UBLOX_RX 2
#define UBLOX_TX 3
#define CUTTER_PIN 5
#define CHIP_SELECT 8
#define PRESSURE_ANALOG_PIN A0

// Intervals
#define UPDATE_INTERVAL 4000

// Constants
#define PSI_TO_ATM 0.068046

// Time Stamps
long updateStamp = 0;


// SD Variables
File dataLog;
char fileName[] = "rcut00.csv";
bool sdActive = false;

// GPS Variables
SoftwareSerial ubloxSerial(UBLOX_RX,UBLOX_TX);
UbloxGPS gps(&ubloxSerial);
float alt[10];        // altitude in feet, also there exists a queue library we can use instead
long timeStamp[10];
float latitude;
float longitude;
float ascentRate;
int sats;

// Pressure Sensor Variables
int pressureAnalog;
float pressureVoltage;
float pressurePSI;


boolean cutterOn = false;

void setup() {
  Serial.begin(9600);
  
  initGPS();

  initSD();

  pinMode(CUTTER_PIN,OUTPUT);

}

void loop() {

  if(millis() - updateStamp > UPDATE_INTERVAL) {
    updateGPS();
    updatePressure();
    logData();
  }

}
