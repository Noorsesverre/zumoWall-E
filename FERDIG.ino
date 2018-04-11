//Libraries
#include <ZumoMotors.h>
#include <Pushbutton.h>
#include <QTRSensors.h>
#include <ZumoReflectanceSensorArray.h>
#include <NewPing.h>
#include <PLab_ZumoMotors.h>
 
//QTR greier
#define QTR_THRESHOLD  200
#define NUM_SENSORS 6
unsigned int sensor_values[NUM_SENSORS];
ZumoReflectanceSensorArray sensors;
  
//Farter og turn durations
#define REVERSE_SPEED     400 // 0 is stopped, 400 is full speed
#define TURN_SPEED        400
#define FORWARD_SPEED     400
#define REVERSE_DURATION  200 // ms
#define TURN_DURATION     150 // ms

//Definerer motorene
ZumoMotors motors;
PLab_ZumoMotors plab_Motors;
 
//Variabler som blir brukt til å bestemme oppførsel
unsigned long time = 0;

//Sonar-ting
const int triggerPin = A5;
const int echoPin = A4;
const int maxDistance = 300;
NewPing sonar(triggerPin, echoPin, maxDistance);

//Definer på-knappen
Pushbutton button(ZUMO_BUTTON); 


//Definerer hvilken modus vi har valgt.
boolean mode = true; //tru for sensorMode, false for randomMode.

void setup() {
  Serial.begin(9600);
  sensors.init(); //Initialiserer QTR sensorer
  button.waitForButton(); // start when button pressed
}

boolean gotBluetooth(boolean selectedMode){
 mode = selectedMode;
}

void turn(boolean left, float x) {   
  time = millis();
      
  //Kjører litt bakover
  motors.setSpeeds(-REVERSE_SPEED, -REVERSE_SPEED);
  delay(REVERSE_DURATION);

  //Svinger enten til venstre eller høyre
  if (left) {
    motors.setSpeeds(TURN_SPEED, -TURN_SPEED);
  } else {
    motors.setSpeeds(-TURN_SPEED, TURN_SPEED);
  }
  delay(TURN_DURATION*x);

  motors.setSpeeds(FORWARD_SPEED, FORWARD_SPEED); //Kjører framover
}

float sonarDistance() {
  // Gjør ett ping, og beregn avstanden
  unsigned int df = sonar.ping();
  float distance = sonar.convert_cm(df);
  return distance;
}

void search(boolean left) {
   boolean done = false;
  //Kjører litt bakover
  motors.setSpeeds(-REVERSE_SPEED, -REVERSE_SPEED);
  delay(REVERSE_DURATION);
  
  while (true) {
    if (left) {
      for (int i = 0; i > 3; i++) {
        motors.setSpeeds(TURN_SPEED, -TURN_SPEED);
        if(sonarDistance() > 0) {
          motors.setSpeeds(FORWARD_SPEED, FORWARD_SPEED);
          done = true;
          break;
        }
      }
    } else {
      for (int i = 0; i > 3; i++) {
        motors.setSpeeds(TURN_SPEED, -TURN_SPEED);
        if(sonarDistance() > 0) {
          motors.setSpeeds(FORWARD_SPEED, FORWARD_SPEED);
          done = true;
          break;
        }
      }
    }
    if (done) {
      break;
    }
  }
}

void loop() {
  if (mode) {
    sensorMode();
  }
  randomMode();
}

void sensorMode(){
  turn(true, 1);
  motors.setSpeeds(FORWARD_SPEED, FORWARD_SPEED);
  
  while(true){
    sensors.read(sensor_values);
    int distance = sonarDistance();
    if (sensor_values[0] < QTR_THRESHOLD || sensor_values[1] < QTR_THRESHOLD) {
      search(true);
    }
    else if (sensor_values[5] < QTR_THRESHOLD || sensor_values[4] < QTR_THRESHOLD) {
      search(false); 
    }
    else {
      motors.setSpeeds(FORWARD_SPEED, FORWARD_SPEED);
    }
  }
}

void randomMode() {
  turn(true, 1);
  motors.setSpeeds(FORWARD_SPEED, FORWARD_SPEED);
  
  while (true) {
    float x = random(1,2);
    sensors.read(sensor_values);
    int distance = sonarDistance();
    if (sensor_values[0] < QTR_THRESHOLD || sensor_values[1] < QTR_THRESHOLD) {
      turn(true, x); 
    }
    else if (sensor_values[5] < QTR_THRESHOLD || sensor_values[4] < QTR_THRESHOLD) {
      turn(false, x); 
    }
    else {   
      motors.setSpeeds(FORWARD_SPEED, FORWARD_SPEED);
    }
  }
}
