#include <Ultrasonic.h>

//Object delare and pin Setting
Ultrasonic ultrasonic(11, 12);

//--pin setup--//
//Motor Driver
const int MotorInA = 14, MotorInB = 15;
const int MotorEnA = 7, MotorEnB = 8;
const int MotorPWM = 23;
const int MotorCS = A5;

//--global variables--//
//Motor Controller
int Motor_Speed;
//float sensorValue, current; // if Current sensor is used
//int waitingTime = 1000;     // if Current sensor is used
//int maxCurrentLimit = 300;  // if Current sensor is used
//bool ShelfJammed = false;   // if Current sensor is used
//bool Jamming = false;       // if Current sensor is used

//ultrasonic sensor
int distance;                // Ultrasonic reading
int speedRate = 15;          // PWM speed rate increment
int DELAY = 100;             // Delay between increase
float Rate = 1.05;           // Rate for ramp up
int counta = 0;              // Shelves go down & up
int countb = 0;              // Shelves go down or up
int distance_limit = 30;     // limit for reading values

int NewSpeed;


void setup() {
  Serial.begin(9600);
  //Pin calling for Motor Controller
  pinMode(MotorInA, OUTPUT);
  pinMode(MotorInB, OUTPUT);
  pinMode(MotorEnA, OUTPUT);
  pinMode(MotorEnB, OUTPUT);
  pinMode(MotorPWM, OUTPUT);
  pinMode(MotorCS,   INPUT);

  //Motor Controller
  digitalWrite(MotorInA,  LOW);
  digitalWrite(MotorInB,  LOW);
  digitalWrite(MotorEnA,  LOW);
  digitalWrite(MotorEnB,  LOW);
}

void loop() {
  distance = ultrasonic.read();

  if ((distance > 5) && (distance < distance_limit)) {  // range 6 - distance_limit
    counta++;
    countb = 0;
    delay(1000); // wait 1 sec
  }
  if (distance <= 5) {  // range 0-5 for reading
    countb++;
    counta = 0;
    delay(1000); //wait 1 sec
  }
  if (counta == 3) {
    Move_Forward();
    counta = 0;
    countb = 0;
    speedRate = 15;
    delay(1000); // wait 1 sec
  }
  if (countb == 3) {
    Move_Reverse();
    counta = 0;
    countb = 0;
    speedRate = 15;
    delay(1000); // wait 1 sec
  }
  if (distance > 40) {
    counta = 0;
    countb = 0;
  }

  Serial.print("Distance in CM: ");
  Serial.println(distance);
  Serial.print("A Counts: ");
  Serial.println(counta);
  Serial.print("B Counts: ");
  Serial.println(countb);
  Serial.print("Speed: ");
  Serial.println(Motor_Speed);

  delay(700); // overall delay
}

int PWM_Increase() {
  for (int i = 50; i < 256; i = i + speedRate) {
    Motor_Speed = i;
    speedRate = speedRate * Rate;
    delay(DELAY);
    Serial.println(Motor_Speed);
  }
}

//----Motor Functions----//
void Move_Forward() {                  // Clockwise Motion
  NewSpeed = PWM_Increase();
  digitalWrite(MotorEnA, HIGH);
  digitalWrite(MotorEnB, HIGH);
  digitalWrite(MotorInA, HIGH);
  digitalWrite(MotorInB, LOW);
  analogWrite(MotorPWM, NewSpeed);
  //Serial.println("avanzando");
}
void Move_Reverse() {                 // Counter Clockwise Motion
  //NewSpeed = PWM_Increase();
  digitalWrite(MotorEnA, HIGH);
  digitalWrite(MotorEnB, HIGH);
  digitalWrite(MotorInA, LOW);
  digitalWrite(MotorInB, HIGH);
  digitalWrite(MotorPWM, HIGH);
  //Serial.println("retrocediendo");
}
void Motor_Stop() {  // Motor stops no matter what direction its going
  digitalWrite(MotorEnA, HIGH);
  digitalWrite(MotorEnB, HIGH);
  digitalWrite(MotorInA, LOW);
  digitalWrite(MotorInB, LOW);
  digitalWrite(MotorPWM, HIGH);
  //Serial.println("estoy parando");
}

