#include <Wire.h>
#include <Adafruit_VL6180X.h>
#include <Ultrasonic.h>
#include <NewPing.h>

#define TrigPin 5
#define EchoPin 6
#define MAX_DISTANCE 46

NewPing sonar(TRIGGER_PIN, ECHO_PIN, MAX_DISTANCE);

float distance;
int actuator;

void setup() {
  Serial.begin(9600);
}

void loop() {
  distance = sonar.pin_cm();

  Serial.print("Distance: ");
  if (distance >= 46) {
    Serial.println("Nada Mucho")
    }
  else {
    Serial.print(distance)
    Serial.println("SOMEONE IS WAITING TO DRINK!")
    delay(1000);
    }
   delay(1000);
   }
