// Script to test telemetry from a UBLOX GPS and a Honeywell pressure sensor.
// Currently written for a teensy 3.5 for ease of testing.
// This test also includes propogation for latitude and longitude values, and should be analyzed to see if such progation measures work.

#include <UbloxGPS.h>
#include <SoftwareSerial.h>
#include <SD.h>

// Pins
#define PRESSURE_ANALOG_PIN A1

// Communication
#define UBLOX_SERIAL Serial1
#define UBLOX_BAUD 9600

// Intervals and Timers
#define LOOP_INTERVAL 2000

// Array Sizes
#define SIZE 10

unsigned long loopTimer = 0;

// Constants
#define PSI_TO_ATM 0.068046             // PSI to ATM conversion ratio
#define SEA_LEVEL_PSI 14.7              // average sea level pressure in PSI
#define D2R PI/180                      // radians per degree
#define R2D 180/PI                      // degrees per radian
#define SECONDS_PER_HOUR 3600           // seconds per hour
#define FPM_PER_MPH 88                  // feet per minute per mile per hour


// Fix statuses
#define NOFIX 0x00
#define FIX 0x01

// Control Telemetry Variables//

// GPS
UbloxGPS gps(&UBLOX_SERIAL);
float alt[SIZE];                  // altitude in feet, also there exists a queue library we can use instead
unsigned long timeStamp[SIZE];    // time stamp array that can be used with alt array to return a velocity
float latitude[SIZE];
float longitude[SIZE];
float nextLat = 0;
float nextLong = 0;
float nextAlt = 0;
float dt = 0;
float heading;
float ascentRate;
float groundSpeed;
uint8_t sats;
uint8_t fixStatus = NOFIX;

// Pressure Sensor
int pressureAnalog;
float pressurePSI;
float pressureAltitude = 0;         // back up altitude to be used when things get desperate

// SD Variables
const int chipSelect = BUILTIN_SDCARD;
File dataLog;
char fileName[] = "tele00.csv";
bool sdActive = false;

void setup() {
  Serial.begin(9600);
  initGPS();

  initSD();

}

void loop() {
  gps.update();   // update the GPS
  
  if (millis() - loopTimer > LOOP_INTERVAL) {
    loopTimer = millis();
    updateGPS();

    logData();
 
  }

}
