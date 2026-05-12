#include <ESP32Servo.h>
#include <BluetoothSerial.h>

BluetoothSerial SerialBT;

// MOTORS
int motorPinL1 = 2;
int motorPinL2 = 3;
int motorLEna = 6;

int motorPinR1 = 4;
int motorPinR2 = 5;
int motorREna = 11;

int speed = 100; // the base speed of the motors

// SERVOS
Servo servo1, servo2;
int servoPin1 = 7;
int servoPin2 = 8;
int baseAngle = 90;

// SENSORS
int turbidity = 9;
int moisture = 12;

void setup() {
  // Motor Pins Setup
  pinMode(motorPinL1, OUTPUT);
  pinMode(motorPinR1, OUTPUT);
  pinMode(motorPinL2, OUTPUT);
  pinMode(motorPinR2, OUTPUT);

  pinMode(motorLEna, OUTPUT);
  pinMode(motorREna, OUTPUT);

  // Servos Setup
  servo1.attach(servoPin1);
  servo2.attach(servoPin2);

  // sets base position each time
  servo1.write(baseAngle);
  servo2.write(baseAngle);


  // Initialise Serial Monitor
  Serial.begin(115200);

  // Initialise Blutooth
  SerialBT.begin("EnviroBot");
  SerialBT.setPin("1234");

  stopMotors();
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
  moveMotor(motorPinR1, motorPinR2, motorREna, 1, speed);  // Move right motor forward
  moveMotor(motorPinL1, motorPinL2, motorLEna, 1, speed);  // Move left motor forward
}

/*
Move the robot backward.
Accepts speed (0-255) to set motor speed.
*/
void moveBackward(int speed) {
  moveMotor(motorPinR1, motorPinR2, motorREna, -1, speed);  // Move right motor backward
  moveMotor(motorPinL1, motorPinL2, motorLEna, -1, speed);  // Move left motor backward
}

/*
Rotate the robot to the right.
Right motor moves forward, left motor moves backward.
*/
void turnRight(int speed) {
  moveMotor(motorPinR1, motorPinR2, motorREna, 1, speed);   // Right motor forward
  moveMotor(motorPinL1, motorPinL2, motorLEna, -1, speed);  // Left motor backward
}

/*
Rotate the robot to the left.
Right motor moves backward, left motor moves forward.
*/
void turnLeft(int speed) {
  moveMotor(motorPinR1, motorPinR2, motorREna, -1, speed);  // Right motor backward
  moveMotor(motorPinL1, motorPinL2, motorLEna, 1, speed);   // Left motor forward
}

/*
Stop both motors.
*/
void stopMotors() {
  moveMotor(motorPinR1, motorPinR2, motorREna, 0, 0);  // Stop right motor (set direction to stop and speed to 0)
  moveMotor(motorPinL1, motorPinL2, motorLEna, 0, 0);  // Stop left motor (set direction to stop and speed to 0)
}


/*
Rotates servo based on position and command pressed.
*/
void rotateServo(Servo &servo, int endPos) {
  endPos = constrain(endPos, 0, 180);

  int pos = servo.read();

  if (pos < endPos) {
    for (; pos <= endPos; pos++) {
        servo.write(pos);
        delay(15);
      }
  }

  else if (pos > endPos) {
    for (; pos >= endPos; pos--) {
        servo.write(pos);
        delay(15);
      }
  }

}

/*
Gets the water turbidity
*/

void getTurbidity() {
  int sensorValue = analogRead(turbidity);

  float voltage = sensorValue * (5.0 / 1024.0);

  // Convert voltage to NTU
  float ntu = -1120.4 * voltage * voltage + 5742.3 * voltage - 4352.9;

  // Prevent negative readings
  if (ntu < 0) {
    ntu = 0;
  }

  // Prints the value in Volts
  Serial.println("Voltage: %.2fV", voltage);
  SerialBT.println("Voltage: %.2fV", voltage);

  // Prints the value in NTU
  Serial.println("Turbidity: %.2f NTU", ntu);
  SerialBT.println("Turbidity: %.2f NTU", ntu);

}


/*
Gets the soil moisture
*/

void getSoilMoisture() {
  int sensorValue = analogRead(moisture);

  // These values need to be tested first
  int dryValue = 3500;
  int wetValue = 1200;

  int moisturePercent = map(sensorValue, dryValue, wetValue, 0, 100);

  moisturePercent = constrain(moisturePercent, 0, 100);

  // Prints the raw analogue value
  Serial.println(".2f%", sensorValue);
  SerialBT.println(".2f%", sensorValue);

  // Prints the value in perecentage
  Serial.println(".2f%", moisturePercent);
  SerialBT.println(".2f%", moisturePercent);
}


void loop() {
  // put your main code here, to run repeatedly:

  // Commands for Controlling EnviroBot With Bluetooth Device
  if (SerialBT.available()) {

    char command = SerialBT.read();

    switch(command) {
      case 'F':
        moveForward(speed);
        break;
      case 'B':
        moveBackward(speed);
        break;
      case 'L':
        turnLeft(speed);
        break;
      case 'R':
        turnRight(speed);
        break;
      case 'S':
        stopMotors();
        break;
      // RIGHT SERVO
      case 'X': {
        int angle = SerialBT.parseInt();
        rotateServo(servo1, angle);
        break;
      }
      // LEFT SERVO
      case 'Y': {
        int angle = SerialBT.parseInt();
        rotateServo(servo2, angle);
        break;
      }
      case 'T': {
        getTurbidity();
        break;
      }
      case 'M': {
        getSoilMoisture();
        break;
      }
    }
  }

}
