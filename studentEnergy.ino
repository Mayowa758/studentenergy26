#include <Servo.h>

// MOTORS
int motorPinL1 = 2;
int motorPinL2 = 3;
int motorEnable = 6;

int motorPinR1 = 4;
int motorPinR2 = 5;
int motor2enable = 11;

int speed = 100; // the speed of the motors

// SERVOS
Servo servo1 servo2;
int servoPin1 = 7;
int servoPin2 = 8;

int baseAngle = 90;

void setup() {
  // Motor Pins Setup
  pinMode(motorPinL1, OUTPUT);
  pinMode(motorPinR1, OUTPUT);
  pinMode(motorPinL2, OUTPUT);
  pinMode(motorPinR2, OUTPUT);

  // Servos Setup
  servo1.attach(servoPin1);
  servo2.attach(servoPin2);

  // sets base position each time
  servo1.write(baseAngle);
  servo2.write(baseAngle);
}

// motor helper function
void moveMotor(int motorPin1, int motorPin2, int motorPinEna, int direction, int speed) {
  if (direction == 1) {  // Move forward
    digitalWrite(motorPin1, HIGH);  // Set IN1 HIGH
    digitalWrite(motorPin2, LOW);   // Set IN2 LOW
  } else if (direction == -1) {  // Move backward
    digitalWrite(motorPin1, LOW);   // Set IN1 LOW
    digitalWrite(motorPin2, HIGH);  // Set IN2 HIGH
  } else {  // Stop (direction == 0)
    digitalWrite(motorPin1, LOW);   // Set both IN1 and IN2 to LOW
    digitalWrite(motorPin2, LOW);
  }

  // Control motor speed using PWM (analogWrite)
  analogWrite(motorPinEna, speed);
}

/*
Move the robot forward.
Accepts speed (0-255) to set motor speed.
*/
void moveForward(int speed) {
  moveMotor(motorPinR1, motorPinR2, motorPinREna, 1, speed);  // Move right motor forward
  moveMotor(motorPinL1, motorPinL2, motorPinLEna, 1, speed);  // Move left motor forward
}

/*
Move the robot backward.
Accepts speed (0-255) to set motor speed.
*/
void moveBackward(int speed) {
  moveMotor(motorPinR1, motorPinR2, motorPinREna, -1, speed);  // Move right motor backward
  moveMotor(motorPinL1, motorPinL2, motorPinLEna, -1, speed);  // Move left motor backward
}

/*
Rotate the robot to the right.
Right motor moves forward, left motor moves backward.
*/
void turnRight(int speed) {
  moveMotor(motorPinR1, motorPinR2, motorPinREna, 1, speed);   // Right motor forward
  moveMotor(motorPinL1, motorPinL2, motorPinLEna, -1, speed);  // Left motor backward
}

/*
Rotate the robot to the left.
Right motor moves backward, left motor moves forward.
*/
void turnLeft(int speed) {
  moveMotor(motorPinR1, motorPinR2, motorPinREna, -1, speed);  // Right motor backward
  moveMotor(motorPinL1, motorPinL2, motorPinLEna, 1, speed);   // Left motor forward
}

/*
Stop both motors.
*/
void stopMotors() {
  moveMotor(motorPinR1, motorPinR2, motorPinREna, 0, 0);  // Stop right motor (set direction to stop and speed to 0)
  moveMotor(motorPinL1, motorPinL2, motorPinLEna, 0, 0);  // Stop left motor (set direction to stop and speed to 0)
}


/*
Rotates servo based on position and command pressed.
*/
void rotateServo(int servo, int direction, int endPos, int command) {
  int pos = servo.read();
  // RIGHT SERVO (direction = 1)
  if (direction == 1) {
    if (command == 'u') {
      for (; pos <= endPos; pos++) {
        servo.write(pos);
        delay(15);
      }
    } 
    else if (command == 'd') {
      for (; pos >= baseAngle; pos--) {
        servo.write(pos);
        delay(15);
      }
    }
  }

  // LEFT SERVO (direction = -1)
  else if (direction == -1) {
    if (command == 'u') {
      for (; pos >= endPos; pos--) {
        servo.write(pos);
        delay(15);
      }
    } 
    else if (command == 'd') {
      for (; pos <= baseAngle; pos++) {
        servo.write(pos);
        delay(15);
      }
    }
  }
}


void loop() {
  // put your main code here, to run repeatedly:

}
