#include <Wire.h>
#include "Adafruit_VL53L0X.h"

// ZAMIENIONE PINS – NIE używamy GPIO8 (bo to LED)
#define SDA_PIN 3
#define SCL_PIN 4

Adafruit_VL53L0X lox = Adafruit_VL53L0X();

void setup() {
  Serial.begin(115200);
  delay(2000);
  pinMode(8, OUTPUT);

  Wire.begin(SDA_PIN, SCL_PIN);
  Wire.setClock(100000);

  Serial.println("Adafruit VL53L0X test");

  if (!lox.begin(0x29, &Wire)) {
    Serial.println(F("Failed to boot VL53L0X"));
    while (1);
  }
  Serial.println(F("VL53L0X API Simple Ranging example\n\n"));
}

void loop() {
  VL53L0X_RangingMeasurementData_t measure;

  lox.rangingTest(&measure, false);

  if (measure.RangeStatus != 4) {
    Serial.print("Distance (mm): ");
    Serial.println(measure.RangeMilliMeter);
  } else {
    Serial.println("Out of range");
  }

  if(measure.RangeMilliMeter < 200){
    digitalWrite(8, HIGH);
  } else {
    digitalWrite(8, LOW);
  }
  delay(50);
}
