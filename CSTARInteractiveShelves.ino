#include <Wire.h>
#include "Adafruit_VL6180X.h"

Adafruit_VL6180X vl = Adafruit_VL6180X();
int ledPin = 23;

void setup() {
  Serial.begin(115200);
  pinMode(ledPin, OUTPUT);

  vl.begin();
}

void loop() {
  float range = vl.readLux(VL6180X_ALS_GAIN_5);
  int Light = map(range, 0, 50, 250, 0);
  analogWrite(ledPin, Light);
  Serial.print("Lux: ");
  Serial.println(range);

    delay(50);
}
