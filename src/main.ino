#include <Servo.h>

// motor driver 
#define PWMA 5
#define AIN1 8
#define AIN2 9

#define PWMB 3
#define BIN1 10
#define BIN2 11

#define STBY 7

// ultrasonic
#define TRIG 12
#define ECHO 13


// servo
#define SERVO_PIN 4

Servo sensorServo;

// settings
#define MOTOR_SPEED 200
#define TURN_SPEED 200

#define SAFE_DISTANCE 20

#define MIN_DIFFERENCE 10

#define CENTER_ANGLE 90
#define LEFT_ANGLE 45
#define RIGHT_ANGLE 135


void setup() {

  pinMode(PWMA, OUTPUT);
  pinMode(AIN1, OUTPUT);
  pinMode(AIN2, OUTPUT);

  pinMode(PWMB, OUTPUT);
  pinMode(BIN1, OUTPUT);
  pinMode(BIN2, OUTPUT);

  pinMode(STBY, OUTPUT);

  pinMode(TRIG, OUTPUT);
  pinMode(ECHO, INPUT);

  sensorServo.attach(SERVO_PIN);
  sensorServo.write(CENTER_ANGLE);

  digitalWrite(STBY, HIGH);

  delay(1000);
}


// distance
float getDistance() {

  digitalWrite(TRIG, LOW);
  delayMicroseconds(2);

  digitalWrite(TRIG, HIGH);
  delayMicroseconds(10);

  digitalWrite(TRIG, LOW);

  long duration = pulseIn(ECHO, HIGH, 30000);

  if (duration == 0) {return 999;}

  float distance = (duration * 0.0343) / 2;

  return distance;
}


// move forward
void forward() {
  digitalWrite(AIN1, HIGH);
  digitalWrite(AIN2, LOW);

  digitalWrite(BIN1, HIGH);
  digitalWrite(BIN2, LOW);

  analogWrite(PWMA, MOTOR_SPEED);
  analogWrite(PWMB, MOTOR_SPEED);
}


// move backward
void backward() {

  digitalWrite(AIN1, LOW);
  digitalWrite(AIN2, HIGH);

  digitalWrite(BIN1, LOW);
  digitalWrite(BIN2, HIGH);

  analogWrite(PWMA, MOTOR_SPEED);
  analogWrite(PWMB, MOTOR_SPEED);
}


// stop
void stopMotors() {

  analogWrite(PWMA, 0);
  analogWrite(PWMB, 0);
}


// turning left
void turnLeft() {

  digitalWrite(AIN1, LOW);
  digitalWrite(AIN2, HIGH);

  digitalWrite(BIN1, HIGH);
  digitalWrite(BIN2, LOW);

  analogWrite(PWMA, TURN_SPEED);
  analogWrite(PWMB, TURN_SPEED);

  delay(500);

  stopMotors();
}


// turning right
void turnRight() {

  digitalWrite(AIN1, HIGH);
  digitalWrite(AIN2, LOW);

  digitalWrite(BIN1, LOW);
  digitalWrite(BIN2, HIGH);

  analogWrite(PWMA, TURN_SPEED);
  analogWrite(PWMB, TURN_SPEED);

  delay(500);

  stopMotors();
}


// scanning left
float scanLeft() {
  sensorServo.write(LEFT_ANGLE);

  delay(500);

  float distance = getDistance();

  return distance;
}


// scanning right
float scanRight() {

  sensorServo.write(RIGHT_ANGLE);

  delay(500);

  float distance = getDistance();

  return distance;
}


// choosing direction
void chooseDirection() {

  stopMotors();

  delay(200);

  // Scan left
  float leftDistance = scanLeft();

  // Scan right
  float rightDistance = scanRight();

  // Return to center
  sensorServo.write(CENTER_ANGLE);

  delay(300);

  // if Left is clearly a better choice
  if (leftDistance > rightDistance + MIN_DIFFERENCE) {turnLeft();}

  // if right is clearly a better choice
  else if (rightDistance > leftDistance + MIN_DIFFERENCE) {turnRight();}

  // if Almost equal, move back slightly and scan again
  else {

    backward();

    delay(300);

    float leftAgain = scanLeft();
    float rightAgain = scanRight();

    sensorServo.write(CENTER_ANGLE);

    delay(300);

    if (leftAgain > rightAgain) {turnLeft();}
    else {turnRight();}
  }
}


void loop() {

  sensorServo.write(CENTER_ANGLE);

  delay(150);

  float frontDistance = getDistance();

  if (frontDistance < SAFE_DISTANCE) {

    stopMotors();
    delay(200);

    backward();
    delay(400);

    stopMotors();
    delay(200);

    chooseDirection();

  }

  else {forward();}

  delay(100);
}
