/*
 April 2, 2021  -> Bobby will bring latest code he has on Monday
                -> Do not upload this code into bar shelves until confirmation is made about being the latest code made
                -> Toggle Switch integration when MOVING DOWN & ANIMATE
*/

#include <Arduino.h>
#include <Ultrasonic.h>

//Object delare and pin Setting
Ultrasonic ultrasonic(3, 2); // (trig, echo)

void Move_Forward();

//--pin setup--//
//Motor Driver
const int MotorInA = 14, MotorInB = 15;
const int MotorEnA = 7, MotorEnB = 8;
const int MotorPWM = 23;
const int MotorCS = A5;

//Toggle Switch Configuration
const int TogglePin1 = 9;
const int TogglePin2 = 11;

//--global variables--//
//Motor Controller
float Motor_Speed;
float NewSpeed;
//float sensorValue, current; //
//int waitingTime = 1000;     //
//int maxCurrentLimit = 300;  // if Current sensor is used
//bool ShelfJammed = false;   //
//bool Jamming = false;       //

//ultrasonic sensor
int distance;            // Ultrasonic reading
int distHOLD = 40;       // Last Ultrasonic reading
float speedRate = 15;    // PWM speed rate increment
int DELAY = 150;         // Delay between increase
float Rate = 1.03;       // Rate for ramp up
int countUP = 0;         // Shelves go UP
int countDOWN = 0;       // Shelves go DOWN
int countCYCLE = 0;      // Shelves Cycle
int distance_limit = 30; // limit for reading values

void setup()
{
  Serial.begin(9600);
  //Pin calling for Motor Controller
  pinMode(MotorInA, OUTPUT);
  pinMode(MotorInB, OUTPUT);
  pinMode(MotorEnA, OUTPUT);
  pinMode(MotorEnB, OUTPUT);
  pinMode(MotorPWM, OUTPUT);
  pinMode(MotorCS, INPUT);

  //Pin calling for Toggle Switch
  pinMode(TogglePin1, OUTPUT);
  pinMode(TogglePin2, OUTPUT);

  //Motor Controller
  digitalWrite(MotorInA, LOW);
  digitalWrite(MotorInB, LOW);
  digitalWrite(MotorEnA, LOW);
  digitalWrite(MotorEnB, LOW);
}

void loop()
{
  distance = ultrasonic.read();

  // Hand moving CLOSER to sensor = [CLOSE shelves]
  if ((distance > 7) && (distance < distance_limit) && (distance <= distHOLD))
  {
    countDOWN++;
    countUP = 0;
    countCYCLE = 0;
    delay(100);
  }

  // Hand moving AWAY from sensor = [OPEN shelves]
  if ((distance > 7) && (distance < distance_limit) && (distance >= distHOLD))
  {
    countUP++;
    countDOWN = 0;
    countCYCLE = 0;
    delay(100);
  }

  // Hand close to sensor = [CYCLE shelves]
  if (distance <= 6)
  {
    countCYCLE++;
    countDOWN = 0;
    countUP = 0;
    delay(100);
  }

  //--READ COUNTS--//
  if (countDOWN == 4)
  {
    PWM_Increase_Forward();
    countDOWN = 0;
    countUP = 0;
    countCYCLE = 0;
    speedRate = 15;
    delay(1000); // wait 1 sec
  }

  if (countUP == 4)
  {
    PWM_Increase_Reverse();
    countDOWN = 0;
    countUP = 0;
    countCYCLE = 0;
    speedRate = 15;
    delay(1000); // wait 1 sec
  }

  if (countCYCLE == 6)
  {
    PWM_Increase_Reverse();
    delay(20000); // Shelf stays down for 20s
    PWM_Increase_Forward();
    countDOWN = 0;
    countUP = 0;
    countCYCLE = 0;
    speedRate = 15;
    delay(1000); // wait 1 sec
  }

  //RESET COUNTS//

  if (distance > 40)
  { // If no hand sensed, reset.
    countDOWN = 0;
    countUP = 0;
    countCYCLE = 0;
  }

  // Hold last distance reading

  Serial.print("Distance in CM: ");
  Serial.println(distance);
  Serial.print("HOLDING DiST: ");
  Serial.println(distHOLD);
  Serial.print("Down Count: ");
  Serial.println(countDOWN);
  Serial.print("UP Count: ");
  Serial.println(countUP);
  Serial.print("Cycle Count: ");
  Serial.println(countCYCLE);
  Serial.print("Speed: ");
  Serial.println(Motor_Speed);

  distHOLD = distance;
  delay(50); // overall delay
}

float PWM_Increase_Forward()
{ // Shelves gradually speed up then slow down at end.
  for (float i = 50; i < 256; i = i + speedRate)
  {
    Motor_Speed = i;
    Move_Forward();
    speedRate = speedRate * Rate;
    Serial.println(Motor_Speed);
    delay(DELAY);
  }
  delay(15500);
  for (float i = 255; i > 100; i = i - speedRate)
  {
    Motor_Speed = i;
    Move_Forward();
    speedRate = speedRate * Rate;
    Serial.println(Motor_Speed);
    delay(DELAY);
  }
}

float PWM_Increase_Reverse()
{ // Shelves gradually speed up then slow down at end.
  for (float i = 50; i < 256; i = i + speedRate)
  {
    Motor_Speed = i;
    Move_Reverse();
    speedRate = speedRate * Rate;
    Serial.println(Motor_Speed);
    delay(DELAY);
  }
  delay(12000);
  for (float i = 255; i > 75; i = i - speedRate)
  {
    Motor_Speed = i;
    Move_Reverse();
    speedRate = speedRate * Rate;
    Serial.println(Motor_Speed);
    delay(DELAY);
  }
}

//----Motor Functions----//
void Move_Forward()
{ // Lower Shelf
  NewSpeed = Motor_Speed;
  digitalWrite(MotorEnA, HIGH);
  digitalWrite(MotorEnB, HIGH);
  digitalWrite(MotorInA, HIGH);
  digitalWrite(MotorInB, LOW);
  analogWrite(MotorPWM, NewSpeed);
  //Serial.println("avanzando");
}
void Move_Reverse()
{ // Retract Shelf
  NewSpeed = Motor_Speed;
  digitalWrite(MotorEnA, HIGH);
  digitalWrite(MotorEnB, HIGH);
  digitalWrite(MotorInA, LOW);
  digitalWrite(MotorInB, HIGH);
  analogWrite(MotorPWM, NewSpeed);
  //Serial.println("retrocediendo");
}
void Motor_Stop()
{ // Motor stops no matter what direction its going
  digitalWrite(MotorEnA, HIGH);
  digitalWrite(MotorEnB, HIGH);
  digitalWrite(MotorInA, LOW);
  digitalWrite(MotorInB, LOW);
  digitalWrite(MotorPWM, HIGH);
  //Serial.println("estoy parando");
}