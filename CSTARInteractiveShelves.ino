/*
 April 2, 2021  -> Bobby will bring latest code he has on Monday
                -> Do not upload this code into bar shelves until confirmation is made about being the latest code made
                -> Toggle Switch integration when MOVING DOWN & ANIMATE

 April 14, 2021 -> Below is latest code as per Bobby and Edgar
                -> Adding Toggle Switch functionality
                          ----
                          | 1 |-- Animate(go down,wait, go up)
                          | 2 |---- OFF
                          | 3 |-- go Down
                          ----
  April 30, 2021 -> Integrating Debounce funtion to switch

  July 19, 2021 -> Testing Control switch box code with main code for compability

July 20, 2021 -> Site installation for new functionality
                          code seems to be a different one that previous uploaded
                          motions are:
                              1. "close to sensor" = UP/DOWN
                              2. "middle of shelf" = ANIMATION
                          reverse = retract / going down
                          forward = expand / going up
*/

#include <Arduino.h>
#include <Ultrasonic.h>
#include <Bounce2.h>

//Object delare and pin Setting
Ultrasonic ultrasonic(3, 2);  // (trig, echo)

Bounce DebounceUP = Bounce();
Bounce DebounceDOWN = Bounce();

//Functions calling
void print_status();
float PWM_Increase_Forward();
float PWM_Increase_Reverse();
void Move_Forward();
void Move_Reverse();
void Motor_Stop();

//--pin setup--//
//Motor Driver
const int MotorInA = 14, MotorInB = 15;
const int MotorEnA = 7, MotorEnB = 8;
const int MotorPWM = 23;
const int MotorCS = A5;

//Toggle Switch Configuration
const int TogglePin1 = 9;
const int TogglePin2 = 11;
int ToggleUP;
int ToggleDOWN;

//--global variables--//
//Motor Controller
float Motor_Speed;
float NewSpeed;
bool shelfDown = false;  //false
/*
Shelf # 5:
1. right now shelf got stuck on going down and shelf is physically down
2. changing value of shelfdown to true helps with shelf go up, but gets the shelf stuck on going up and shelf is physically up
3. changing the value back to false, makes the shelf go to animation after bootup but doesnt go up. sensor activates going down when triggered but gets stuck on that function while shelf is already down.
4. cycling through step 1 and 2 but this time with cycle/animation commented out
5. after shelfdown value is changed back to false, shelf going down motion does get activated, but it gets stuck on the function and doesnt allow the sensor to read other functions (13sec between each serial.print)

Shelf # 6:
(shelf 1 has different code, serial.prints are in spanish)
1. right now shelf got stuck on going down and shelf is physically down 
2. first upload went into going down after bootup and got stuck, shelf is already down
3. second upload made the shelf go up, but motor got stuck midway. (motor seems to be not sufficient)
4. switching between false and tre makes the shelf move either up or down.
5. motor is giving out going up similar as the previous shelf (even with help it doesnt go all the way up)



*/
//float sensorValue, current; //
//int waitingTime = 1000;     //
//int maxCurrentLimit = 300;  // if Current sensor is used
//bool ShelfJammed = false;   //
//bool Jamming = false;       //

//ultrasonic sensor
int distance;  // Ultrasonic reading
//int distHOLD = 40;       // Last Ultrasonic reading
float speedRate = 15;     // PWM speed rate increment
int DELAY = 250;          // Delay between increase (150)
float Rate = 1.07;        // Rate for ramp up (1.03)
int countUP = 0;          // Shelves go UP
int countDOWN = 0;        // Shelves go DOWN
int countCYCLE = 0;       // Shelves Cycle
int distance_limit = 30;  // limit for reading values

void setup() {
  Serial.begin(9600);
  Serial.println("I'm waking up");

  //Pin calling for Motor Controller
  pinMode(MotorInA, OUTPUT);
  pinMode(MotorInB, OUTPUT);
  pinMode(MotorEnA, OUTPUT);
  pinMode(MotorEnB, OUTPUT);
  pinMode(MotorPWM, OUTPUT);
  pinMode(MotorCS, INPUT);

  //Pin calling for Toggle Switch
  DebounceUP.attach(TogglePin1, INPUT_PULLUP);
  DebounceUP.interval(5);
  DebounceDOWN.attach(TogglePin2, INPUT_PULLUP);
  DebounceDOWN.interval(5);

  //Motor Controller
  digitalWrite(MotorInA, LOW);
  digitalWrite(MotorInB, LOW);
  digitalWrite(MotorEnA, LOW);
  digitalWrite(MotorEnB, LOW);

  Serial.println("Initialization successful");
}

void loop() {
  distance = ultrasonic.read();
  DebounceUP.update();
  DebounceDOWN.update();
  ToggleUP = DebounceUP.read();
  ToggleDOWN = DebounceDOWN.read();

   print_status();
  // Hand moving CLOSER to sensor = [CLOSE shelves]  ///OCT12/23 -> hand less then 6 will go down if shelfdown = false
  if ((distance <= 6) && (shelfDown != true))  //(distance > 7) && (distance < distance_limit)
  {
    countDOWN++;
    countUP = 0;
    countCYCLE = 0;
    delay(100);
  }

  // // Hand moving AWAY from sensor = [OPEN shelves] ///OCT12/23 -> hand less then 6 will go up if shelfdown = true
  if ((distance <= 6) && (shelfDown == true))  //(distance > 7) && (distance < distance_limit)
  {
    countUP++;
    countDOWN = 0;
    countCYCLE = 0;
    delay(100);
  }

  // Hand close to sensor = [CYCLE shelves]
  if ((distance > 7) && (distance < distance_limit))  //(distance <= 6)
  {
    countCYCLE++;
    countDOWN = 0;
    countUP = 0;
    delay(100);
  }

  ///////////////////
  //  READ COUNTS  //
  ///////////////////

  // Shelves go Down[OPEN]
  if ((countDOWN == 4) || (ToggleDOWN == 0)) {
    Serial.println("Going DOWN");
    Move_Reverse();
    //PWM_Increase_Reverse();
    countDOWN = 0;
    countUP = 0;
    countCYCLE = 0;
    speedRate = 15;
    shelfDown = true;
    // DebounceUP.update();
    // DebounceDOWN.update();
    delay(1000);  // wait 1 sec
  }

  //Shelves go Up[CLOSE] or Toggle switch is in position 3
  if (countUP == 4) {
    Serial.println("Going UP");
    Move_Forward();
    // PWM_Increase_Forward();
    countDOWN = 0;
    countUP = 0;
    countCYCLE = 0;
    speedRate = 15;
    shelfDown = false;
    delay(1000);  // wait 1 sec
  }

  //Shelves Animate(go Down, wait, go Up) or Toggle Switch is in Position 1
  if ((countCYCLE == 4) || (ToggleUP == 0))  //countCYCLE ==6
  {
    Serial.println("Animation mode");
    Move_Reverse(); // 13 sec to go all down
    //PWM_Increase_Reverse();
    delay(33000);  //going down time + waiting time 13+20 = 33sec delay. Shelf stays down for 15s (Shelf stays down for 20s -> 5000)
    Move_Forward();// 17-18 sec to go all the way up
    //PWM_Increase_Forward();
    countDOWN = 0;
    countUP = 0;
    countCYCLE = 0;
    speedRate = 15;
    DebounceUP.update();
    DebounceDOWN.update();
    delay(1000);  // wait 1 sec
  }

  //RESET COUNTS//
  if ((distance > 40) && (ToggleUP && ToggleDOWN == 1)) {  // If no hand sensed, reset.
    Serial.println("Waiting for command");
    countDOWN = 0;
    countUP = 0;
    countCYCLE = 0;
  }

  // // Hold last distance reading
  // // distHOLD = distance;
  delay(50);  // overall delay
}

void print_status() {
  Serial.print("Distance in CM: ");
  Serial.print(distance);
  Serial.print("\t");
  // Serial.print("HOLDING DiST: ");
  // Serial.print(distHOLD);
  // Serial.print("\t");
  Serial.print("Down Count: ");
  Serial.print(countDOWN);
  Serial.print("\t");
  Serial.print("UP Count: ");
  Serial.print(countUP);
  Serial.print("\t");
  Serial.print("Cycle Count: ");
  Serial.print(countCYCLE);
  Serial.print("\t");
  Serial.print("bool_shelfDown: ");
  Serial.print(shelfDown);
  Serial.print("\t");
  Serial.print("Speed: ");
  Serial.print(Motor_Speed);
  Serial.print("\t");
  Serial.print("Down Toggle Pos: ");
  Serial.print(ToggleDOWN);
  Serial.print("\t");
  Serial.print("Up Toogle Pos: ");
  Serial.println(ToggleUP);
  return;
}

// Shelves gradually speed up then slow down at end in the "up" direction.
float PWM_Increase_Forward() {
  for (float i = 255; i < 256; i = i + speedRate)  //i=50
  {
    Motor_Speed = i;

    Move_Forward();
    speedRate = speedRate * Rate;
    Serial.print(Motor_Speed);
    Serial.print("\t");
    Serial.print(speedRate);
    delay(DELAY);
  }
  delay(14750);                                    // 15500
  for (float i = 255; i > 200; i = i - speedRate)  //i>100
  {
    Motor_Speed = i;
    Move_Forward();
    speedRate = speedRate * Rate;
    Serial.print("\t");
    Serial.print(Motor_Speed);
    Serial.print("\t");
    Serial.println(speedRate);
    //Serial.println(Motor_Speed);
    delay(DELAY);
  }
}

// Shelves gradually speed up then slow down at end in the "down" direction.
float PWM_Increase_Reverse() {
  for (float i = 175; i < 256; i = i + speedRate)  //i=50
  {
    Motor_Speed = i;
    Move_Reverse();
    speedRate = speedRate * Rate;
    //Serial.println(Motor_Speed);
    delay(DELAY);
  }
  // delay(12000);
  for (float i = 255; i > 256; i = i - speedRate)  //i>75
  {
    Motor_Speed = i;
    Move_Reverse();
    speedRate = speedRate * Rate;
    //Serial.println(Motor_Speed);
    delay(DELAY);
  }
}

//----Motor Functions----//
void Move_Forward() {  // Lower Shelf
  //NewSpeed = Motor_Speed;
  digitalWrite(MotorEnA, HIGH);
  digitalWrite(MotorEnB, HIGH);
  digitalWrite(MotorInA, HIGH);
  digitalWrite(MotorInB, LOW);
  analogWrite(MotorPWM, 255);
  //Serial.println("avanzando");
}

void Move_Reverse() {  // Retract Shelf
  //NewSpeed = Motor_Speed;
  digitalWrite(MotorEnA, HIGH);
  digitalWrite(MotorEnB, HIGH);
  digitalWrite(MotorInA, LOW);
  digitalWrite(MotorInB, HIGH);
  analogWrite(MotorPWM, 255);
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
