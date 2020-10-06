/********************************************************
 * PID RelayOutput Example
 * Same as basic example, except that this time, the output
 * is going to a digital pin which (we presume) is controlling
 * a relay.  the pid is designed to Output an analog value,
 * but the relay can only be On/Off.
 *
 *   to connect them together we use "time proportioning
 * control"  it's essentially a really slow version of PWM.
 * first we decide on a window size (5000mS say.) we then
 * set the pid to adjust its output between 0 and that window
 * size.  lastly, we add some logic that translates the PID
 * output into "Relay On Time" with the remainder of the
 * window being "Relay Off Time"
 ********************************************************/

#include <PID_v1.h>
#include "pins.h"
#include <SPI.h>
#include "Adafruit_MAX31855.h"
#include <ClearPathMotorSD.h>
#include <ClearPathStepGen.h>

/********************************************************
 * PID
 ********************************************************/
//Define Variables we'll be connecting to
double Setpoint, Input, Output;
//Specify the links and initial tuning parameters
double Kp=2, Ki=5, Kd=1;
PID myPID(&Input, &Output, &Setpoint, Kp, Ki, Kd, DIRECT);

const int WindowSize = 5000;
unsigned long windowStartTime;
unsigned long time_1 = 0;
char buffer[64];
int sofar;
float mm, fr, flow;
bool heaterOn = false;

/********************************************************
 * THERMOCOUPLE
 ********************************************************/
Adafruit_MAX31855 t1 (SCK, T1_CS, MISO);

byte BSPEED = 0b00000000;
int IN_PINS[] = {IN_b0, IN_b1, IN_b2, IN_b3, IN_b4, IN_b5, IN_b6, IN_b7};
unsigned int TOOL_SPEED = 0;

void updateSpeed (){
  for(int i=0; i<7; i++){
    bitWrite(BSPEED,i, digitalRead(IN_PINS[i]));
  }
  TOOL_SPEED = (unsigned int)BSPEED;
}


/********************************************************
 * MOTOR
 ********************************************************/
ClearPathMotorSD Extruder;
ClearPathStepGen machine(&Extruder);

void help(){
  Serial.print("G1 [E(mm)] [F(mm/s)]; - linear move");
  Serial.println("G6 [F(mm/s)]; - move w/ speed");
  Serial.println("M18; - disable motors");
  Serial.println("M104 [R(temperature)]; - set extruder temperature");
  Serial.println("M108; - cancel heating");
  Serial.println("M111; [R(flow)]- flow multiplier *1%");
  Serial.println("M100; -help");
}

void ready() {
  sofar=0; // clear input buffer
  //Serial.print(F("> ")); // signal ready to receive input
}

float parseNumber(char code,float val) {
  char *ptr=buffer;  // start at the beginning of buffer
  while((long)ptr > 1 && (*ptr) && (long)ptr < (long)buffer+sofar) {  // walk to the end
    if(*ptr==code) {  // if you find code on your walk,
      return atof(ptr+1);  // convert the digits that follow into a float and return it
    }
    ptr=strchr(ptr,' ')+1;  // take a step from here to the letter after the next space
  }
  return val;  // end reached, nothing found, return default val.
}

void processCommand(){
  // look for commands that start with 'G'
  int cmd=parseNumber('G',-1);
  switch(cmd) {
  case 1: // move to distance
    if(Input > Setpoint-10){
      Extruder.setMaxVel(parseNumber('F', fr));
      Extruder.move(parseNumber('E', mm));
    }else{
      Serial.println("COLD EXTRUSION PREVENTED");
    }
    break;
  case 4: // move with speed
    if(Input > Setpoint-10){
    Extruder.moveWithVelocity(parseNumber('F', fr));
    }else{
      Serial.println("COLD EXTRUSION PREVENTED");      
    }
    break; 
    case 69:
      TOOL_SPEED = TOOL_SPEED + 10;
    break;
  default: break;
  }

  // look for commands that start with 'M'
  cmd=parseNumber('M',-1);
  switch(cmd) {
  case 18: // turns off power to steppers (releases the grip)
    Extruder.stopMove();
    break;
  case 100: help(); break;
  case 104:
    Setpoint = parseNumber('R', Setpoint); 
    break;
  case 105:
    heaterOn = true; break;
  case 108:
    heaterOn = false; break;
  case 111:
    flow = parseNumber('R', flow);
  default: break;
  } 
}

void print_stat(){
    // T = temp , S = speed , H = heateron , 
    Serial.print("T");
    Serial.println(Input);

    Serial.print("S");
    Serial.println(TOOL_SPEED);

    Serial.print("H");
    Serial.println(heaterOn);

    Serial.println("");
    //Serial.println();

}

void setup()
{
  pinMode(SSR_0_PIN, OUTPUT);
  pinMode(MOSFET_0_PIN, OUTPUT);
  pinMode(IN_b0, INPUT);
  pinMode(IN_b1, INPUT);
  pinMode(IN_b2, INPUT);
  pinMode(IN_b3, INPUT);
  pinMode(IN_b4, INPUT);
  pinMode(IN_b5, INPUT);
  pinMode(IN_b6, INPUT);
  pinMode(IN_b7, INPUT);

  digitalWrite(MOSFET_0_PIN, HIGH);

  Serial.begin(9600);

  help();
  ready();

  Extruder.attach(MOT_DIR_PIN, MOT_STEP_PIN, MOT_ENBL_PIN);
  Extruder.setMaxVel(100000);
  Extruder.setMaxAccel(2000000);
  Extruder.enable();
  machine.Start();

  windowStartTime = millis();
  //initialize the variables we're linked to
  Setpoint = 220;

  //tell the PID to range between 0 and the full window size
  myPID.SetOutputLimits(0, WindowSize);

  //turn the PID on
  myPID.SetMode(AUTOMATIC);

  Input = 200;
  flow = 1.0;
  /*
  Serial.print("Initializing sensor...");
  if (!t1.begin()) {
    Serial.println("ERROR.");
  }*/
}

void loop()
{

  if(Serial.available()){
    char c = Serial.read();

    if(sofar < 64) buffer[sofar++] = c;
    if((c=='\n') || (c == '\r')){
      buffer[sofar] = 0;
      Serial.print(F("\r\n"));
      processCommand();
      ready();
    }
  }
  
  if(heaterOn){
     myPID.Compute();
    /************************************************
    * turn the output pin on/off based on pid output
    ************************************************/
    if (millis() - windowStartTime > WindowSize)
    { //time to shift the Relay Window
      //Input = t1.readCelsius();
      if(Input < Setpoint) {
        Input ++;}else{
        Input--;
        }
      windowStartTime += WindowSize;
    }
    if (Output < millis() - windowStartTime) digitalWrite(SSR_0_PIN, HIGH);
    else digitalWrite(SSR_0_PIN, LOW);
  }
  
  if(millis() > time_1 + 2000){
    time_1 = millis();

    print_stat();
  }
    
  
}
