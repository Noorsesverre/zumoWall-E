//Libraries
#include <ZumoMotors.h>
#include <Pushbutton.h>
#include <QTRSensors.h>
#include <ZumoReflectanceSensorArray.h>
#include <NewPing.h>
#include <PLab_ZumoMotors.h>
 
//QTR greier
#define QTR_THRESHOLD  1200
#define NUM_SENSORS 6
unsigned int sensor_values[NUM_SENSORS];
ZumoReflectanceSensorArray sensors;
  
//Farter og turn durations
#define REVERSE_SPEED     400 // 0 is stopped, 400 is full speed
#define TURN_SPEED        400
#define FORWARD_SPEED     300
#define REVERSE_DURATION  200 // ms
#define TURN_DURATION     150 // ms

//Definerer motorene
ZumoMotors motors;
PLab_ZumoMotors plab_Motors;
 
//Variabler som blir brukt til å bestemme oppførsel
unsigned long time = 0;
unsigned long time2 = 0;
unsigned long difference = 0;

//Sonar-ting
const int triggerPin = A5;
const int echoPin = A4;
const int maxDistance = 50;
NewPing sonar(triggerPin, echoPin, maxDistance);

//Definer på-knappen
Pushbutton button(ZUMO_BUTTON); 


//Definerer hvilken modus vi har valgt.
boolean mode = true; //tru for sensorMode, false for randomMode.

const int buttonPin = 2;

void setup() {
  Serial.begin(9600);
  pinMode(buttonPin, INPUT);
  sensors.init(); //Initialiserer QTR sensorer
  button.waitForButton(); // start when button pressed
}

boolean gotBluetooth(boolean selectedMode){
 mode = selectedMode;
}

void turn(boolean left, float x) {   
  //Kjører litt bakover
  motors.setSpeeds(-REVERSE_SPEED, -REVERSE_SPEED);
  delay(REVERSE_DURATION);

  //Svinger enten til venstre eller høyre
  if (left) {
    motors.setSpeeds(-TURN_SPEED, TURN_SPEED);
    delay(TURN_DURATION*x);
  } else {
    motors.setSpeeds(TURN_SPEED, -TURN_SPEED);
    delay(TURN_DURATION*x);
  }
  motors.setSpeeds(FORWARD_SPEED, FORWARD_SPEED); //Kjører framover
}

float sonarDistance() {
  // Gjør ett ping, og beregn avstanden
  unsigned int df = sonar.ping();
  delay(10);
  float distance = sonar.convert_cm(df);
  return distance;
}

void search(boolean left) {
  //Kjører litt bakover
  motors.setSpeeds(-REVERSE_SPEED, -REVERSE_SPEED);
  delay(REVERSE_DURATION * 1.5);
  bool done = false;
  
  if (left) {   //Hvis den skal spinne mot venstre
    for (int i = 0; i < 3; i++) {
      motors.setSpeeds(-TURN_SPEED, TURN_SPEED);
      delay(TURN_DURATION * 0.75);
      motors.setSpeeds(0, 0);
      time = 0; 
      while (time < 10) {
        if(sonarDistance() > 0) {
          motors.setSpeeds(FORWARD_SPEED+100, FORWARD_SPEED+50);
          done = true;
          break;
        }
        time++;
      }
      if (done){
        break;
      }
    }
  } else {  //Hvis den skal spinne mot høyre
    for (int i = 0; i < 3; i++) {
      motors.setSpeeds(TURN_SPEED, -TURN_SPEED);
      delay(TURN_DURATION * 0.75);
      motors.setSpeeds(0, 0);
      time = 0; 
      while (time < 10) {
        if(sonarDistance() > 0) {
          motors.setSpeeds(FORWARD_SPEED+100, FORWARD_SPEED+50);
          done = true;
          break;
        }
        time++;
      }
      if (done){
        break;
      }
    }
  }
}

void loop() {
  if (mode) { //digitalRead(buttonPin) == LOW
    sensorMode();
  }
  randomMode();
}

void sensorMode(){
  motors.setSpeeds(-TURN_SPEED, TURN_SPEED);
  delay(TURN_DURATION);
  motors.setSpeeds(FORWARD_SPEED, FORWARD_SPEED);
  
  while(true){
    sensors.read(sensor_values);
    int distance = sonarDistance();
    if (sensor_values[0] < QTR_THRESHOLD || sensor_values[1] < QTR_THRESHOLD) {
      search(false);
    }
    else if (sensor_values[5] < QTR_THRESHOLD || sensor_values[4] < QTR_THRESHOLD) {
      search(true); 
    }
    else {
      motors.setSpeeds(FORWARD_SPEED, FORWARD_SPEED);
    }
  }
}

void randomMode() {
  motors.setSpeeds(-TURN_SPEED, TURN_SPEED);
  delay(TURN_DURATION);
  motors.setSpeeds(FORWARD_SPEED, FORWARD_SPEED);
  
  while (true) {
    float x = random(1,2);
    sensors.read(sensor_values);
    int distance = sonarDistance();
    if (sensor_values[0] < QTR_THRESHOLD || sensor_values[1] < QTR_THRESHOLD) {
      turn(false, x); 
    }
    else if (sensor_values[5] < QTR_THRESHOLD || sensor_values[4] < QTR_THRESHOLD) {
      turn(true, x); 
    }
    else {   
      motors.setSpeeds(FORWARD_SPEED, FORWARD_SPEED);
    }
  }
}
