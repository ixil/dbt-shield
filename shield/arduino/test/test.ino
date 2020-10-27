#include <SPI.h>
#include "Adafruit_MAX31855.h"
#include <PID_v1.h>
#include <AutoPID.h>
#include <ClearPathMotorSD.h>
#include <ClearPathStepGen.h>

#define RELAY_PIN A3
#define LED_PIN 6
#define TEMP_READ_DELAY 800
#define OUTPUT_MIN 0
#define OUTPUT_MAX 255
#define KP .12
#define KI .0003
#define KD 0
#define MOT_STEP_PIN                          22
#define MOT_DIR_PIN                           24
#define MOT_ENBL_PIN                          23

// Default connection is using software SPI, but comment and uncomment one of
// the two examples below to switch between software SPI and hardware SPI:

// Example creating a thermocouple instance with software SPI on any three
// digital IO pins.
#define MAXDO   50
#define MAXCS   18
#define MAXCLK  52


double temperature, setPoint, outputVal;
AutoPID myPID(&temperature, &setPoint, &outputVal, OUTPUT_MIN, OUTPUT_MAX, KP, KI, KD);
unsigned long lastTempUpdate;

// initialize the Thermocouple
Adafruit_MAX31855 thermocouple(MAXCLK, MAXCS, MAXDO);

ClearPathMotorSD Extruder;
ClearPathStepGen machine(&Extruder);

bool updateTemperature() {
  if ((millis() - lastTempUpdate) > TEMP_READ_DELAY) {
    temperature = thermocouple.readCelsius(); //get temp reading
    lastTempUpdate = millis();
    return true;
  }
  return false;
}//void updateTemperature

void setup() {
  Serial.begin(9600);


  Serial.println("MAX31855 test");
  // wait for MAX chip to stabilize
  delay(500);
  Serial.print("Initializing sensor...");
  if (!thermocouple.begin()) {
    Serial.println("ERROR.");
    while (1) delay(10);
  }
  Serial.println("DONE.");
  
  while (!updateTemperature()) {}
  
  //if temperature is more than 4 degrees below or above setpoint, OUTPUT will be set to min or max respectively
  myPID.setBangBang(4);
  //set PID update interval to 4000ms
  myPID.setTimeStep(4000);
  setPoint = 60;

  Extruder.attach(MOT_DIR_PIN, MOT_STEP_PIN, MOT_ENBL_PIN);
  Extruder.setMaxVel(100000);
  Extruder.setMaxAccel(2000000);
  //Extruder.enable();
  delay(100);
  machine.Start();
  
  pinMode(13, OUTPUT);
  digitalWrite(13, HIGH);

  //pinMode(23, OUTPUT);
  //digitalWrite(23, HIGH);

}

void loop() {
   double c = thermocouple.readCelsius();
   if (isnan(c)) {
     //Serial.println("Something wrong with thermocouple!");
   } else {
     Serial.print("C = ");
     Serial.println(c);

     
   }

  updateTemperature();
 // setPoint = analogRead(POT_PIN);
  myPID.run(); //call every loop, updates automatically at certain time interval
  analogWrite(RELAY_PIN, outputVal);
  digitalWrite(LED_PIN, myPID.atSetPoint(1));
  delay(250);


}