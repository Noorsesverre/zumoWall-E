
#include <ZumoMotors.h>
#include <Pushbutton.h>
#include <QTRSensors.h>
#include <ZumoReflectanceSensorArray.h>
#include <NewPing.h>
#include <NewServo.h>
#include <Pushbutton.h>
#include <PLab_ZumoMotors.h>
 
#define LED 13
 
// this might need to be tuor different lighting conditions, surfaces, etc.
#define QTR_THRESHOLD  200 // ned f
  
// these might need to be tuned for different motor types
#define REVERSE_SPEED     400 // 0 is stopped, 400 is full speed
#define TURN_SPEED        400
#define FORWARD_SPEED     400
#define REVERSE_DURATION  200 // ms
#define TURN_DURATION     150 // ms
 
ZumoMotors motors;
 
#define NUM_SENSORS 6
unsigned int sensor_values[NUM_SENSORS];
 
ZumoReflectanceSensorArray sensors;

int x = 0;

long timeM;

unsigned int time;

const int ledPin = 6;

const int echoPin = 2;
const int triggerPin = 3;
const int maxDistance = 80;

NewPing sonar(triggerPin, echoPin, maxDistance);

Pushbutton button(ZUMO_BUTTON); 

PLab_ZumoMotors plab_Motors;

int prevDistance = 0;

void setup()
{
   pinMode(ledPin, OUTPUT);
   Serial.begin(9600);
   sensors.init();
   button.waitForButton(); // start when button pressed
   
}
void turnLeft () {   
      timeM = millis();
      // if leftmost sensor detects line, reverse and turn to the right
      motors.setSpeeds(-REVERSE_SPEED, -REVERSE_SPEED);
      delay(REVERSE_DURATION);
      motors.setSpeeds(TURN_SPEED, -TURN_SPEED);
      delay(TURN_DURATION*x);
      motors.setSpeeds(FORWARD_SPEED, FORWARD_SPEED);

}

void turnRight ()  {
    timeM = millis();
    // if rightmost sensor detects line, reverse and turn to the left
    motors.setSpeeds(-REVERSE_SPEED, -REVERSE_SPEED);
    delay(REVERSE_DURATION);
    motors.setSpeeds(-TURN_SPEED, TURN_SPEED);
    delay(TURN_DURATION*x);
    motors.setSpeeds(FORWARD_SPEED, FORWARD_SPEED);
}


void loop() {
  
  time = sonar.ping();
  x = random(1,2);
  sensors.read(sensor_values);
  
  float distance = sonar.convert_cm(time);
 
  Serial.println(distance);
  

  if (sensor_values[0] < QTR_THRESHOLD || sensor_values[1] < QTR_THRESHOLD)
  {
    turnLeft(); 
  }
  else if (sensor_values[5] < QTR_THRESHOLD || sensor_values[4] < QTR_THRESHOLD)
  {
    turnRight(); 
  }
  else if (distance > 0 && distance < 40) {
    digitalWrite(ledPin, HIGH);
    motors.setSpeeds(FORWARD_SPEED, FORWARD_SPEED);
    prevDistance = distance;
    if (prevDistance = 0) {
      turnLeft();
    }
    digitalWrite(ledPin, LOW);
  }
  else {
    if (millis() > timeM + 750) {
       motors.setSpeeds(FORWARD_SPEED/2, FORWARD_SPEED/2);
    } else {
       motors.setSpeeds(FORWARD_SPEED, FORWARD_SPEED);
    }   
  }
}



