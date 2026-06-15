#include <ESP32Servo.h>
#include <BluetoothSerial.h>

BluetoothSerial SerialBT;

// MOTORS
int motorPinL1 = 25;
int motorPinL2 = 26;
int motorLEna = 27;

int motorPinR1 = 21;
int motorPinR2 = 22;
int motorREna = 23;

int speed = 255; // the base speed of the motors
int lowSpeed = 110;

// SERVOS
Servo servo1, servo2;
int servoPin1 = 16;
int servoPin2 = 17;
int baseAngle = 0;
int moistureServoAngle = 105;
int turbidityServoAngle = 60;

// SENSORS
int moisturePin = 34;
int turbidityPin = 35;

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
  // SerialBT.setPin("1234");

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
  SerialBT.println("FORWARD");
}

/*
Move the robot backward.
Accepts speed (0-255) to set motor speed.
*/
void moveBackward(int speed) {
  moveMotor(motorPinR1, motorPinR2, motorREna, -1, speed);  // Move right motor backward
  moveMotor(motorPinL1, motorPinL2, motorLEna, -1, speed);  // Move left motor backward
  SerialBT.println("BACKWARD");
}

/*
Rotate the robot to the left.
Right motor moves forward, left motor moves backward.
*/
void turnLeft(int speed) {
  moveMotor(motorPinR1, motorPinR2, motorREna, 1, speed);   // Right motor forward
  moveMotor(motorPinL1, motorPinL2, motorLEna, 1, lowSpeed);  // Left motor backward
  SerialBT.println("LEFT");
}

/*
Rotate the robot to the left.
Right motor moves backward, left motor moves forward.
*/
void turnRight(int speed) {
  moveMotor(motorPinR1, motorPinR2, motorREna, 1, lowSpeed);  // Right motor backward
  moveMotor(motorPinL1, motorPinL2, motorLEna, 1, speed);   // Left motor forward
  SerialBT.println("RIGHT");
}

/*
Stop both motors.
*/
void stopMotors() {
  moveMotor(motorPinR1, motorPinR2, motorREna, 0, 0);  // Stop right motor (set direction to stop and speed to 0)
  moveMotor(motorPinL1, motorPinL2, motorLEna, 0, 0);  // Stop left motor (set direction to stop and speed to 0)
  SerialBT.println("STOP");
}


/*
Rotates servo based on position and command pressed.
*/
void rotateServo(Servo &servo, int endPos) {
  endPos = constrain(endPos, 0, 180);
  servo.write(endPos);

  // int pos = servo.read();

  // if (pos < endPos) {
  //   for (; pos <= endPos; pos++) {
  //       servo.write(pos);
  //       delay(10);
  //     }
  // }

  // else if (pos > endPos) {
  //   for (; pos >= endPos; pos--) {
  //       servo.write(pos);
  //       delay(10);
  //     }
  // }
}

/*
Gets the water turbidity
*/
void getTurbidity() {
  int sensorValue = analogRead(turbidityPin);

  float voltage = (sensorValue * (5.0 / 1024.0)) / 2;

  // Convert voltage to NTU
  float ntu = -1120.4 * voltage * voltage + 5742.3 * voltage - 4352.9;

  // Prevent negative readings
  if (ntu < 0) {
    ntu = 0;
  }

  Serial.print("Voltage: ");
  Serial.print(voltage, 2);
  Serial.println(" V");

  SerialBT.print("Voltage: ");
  SerialBT.print(voltage, 2);
  SerialBT.println(" V");

  Serial.print("Turbidity: ");
  Serial.print(ntu, 2);
  Serial.println(" NTU");

  SerialBT.print("Turbidity: ");
  SerialBT.print(ntu, 2);
  SerialBT.println(" NTU");
}


/*
Gets the soil moisture
*/
void getSoilMoisture() {
  int sensorValue = analogRead(moisturePin);

  // These values need to be tested first
  int dryValue = 3200;
  int wetValue = 1300;

  int moisturePercent = map(sensorValue, dryValue, wetValue, 0, 100);

  moisturePercent = constrain(moisturePercent, 0, 100);

  Serial.print("Raw Value: ");
  Serial.println(sensorValue);

  SerialBT.print("Raw Value: ");
  SerialBT.println(sensorValue);

  Serial.print("Moisture: ");
  Serial.print(moisturePercent);
  Serial.println("%");

  SerialBT.print("Moisture: ");
  SerialBT.print(moisturePercent);
  SerialBT.println("%");
}


void loop() {
  // put your main code here, to run repeatedly:

  // Commands for Controlling EnviroBot With Bluetooth Device
  if (SerialBT.available()) {

    char command = SerialBT.read();

    switch(command) {
      case 'f': {
        // int commandSpeed = SerialBT.parseInt();
        // if (commandSpeed == 0) {
        //   commandSpeed = speed;  // use stored default
        // }
        // moveForward(commandSpeed);
        // SerialBT.print("Forwards speed set to ");
        // SerialBT.println(commandSpeed);
        moveForward(speed);
        break;
      }
      case 'b': {
        // int commandSpeed = SerialBT.parseInt();
        // if (commandSpeed == 0) {
        //   commandSpeed = speed;  // use stored default
        // }
        // moveBackward(commandSpeed);
        // SerialBT.print("Backwards speed set to ");
        // SerialBT.println(commandSpeed);
        moveBackward(speed);
        break;
      }
      case 'l': {
        turnLeft(speed);
        break;
      }
      case 'r': {
        turnRight(speed);
        break;
      }
      case 's': {
        stopMotors();
        break;
      }
      // MOISTURE SERVO
      case 'x': {
        // int angle = SerialBT.parseInt();
        // SerialBT.print("Moving to ");
        // SerialBT.println(angle);
        int option = SerialBT.parseInt();
        if (option == 1) {
          rotateServo(servo1, moistureServoAngle);
        }
        else if (option == 2) {
          rotateServo(servo1, 0);
        }
        break;
      }
      // TURBIDITY SERVO
      case 'y': {
        // int angle = SerialBT.parseInt();
        // SerialBT.print("Moving to ");
        // SerialBT.println(angle);
        int option = SerialBT.parseInt();
        if (option == 1) {
          rotateServo(servo2, turbidityServoAngle);
        }
        else if (option == 2) {
          rotateServo(servo2, 0);
        }
        break;
      }
      case 't': {
        getTurbidity();
        break;
      }
      case 'm': {
        getSoilMoisture();
        break;
      }
    }
  }
}
