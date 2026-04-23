#include <Servo.h>
#include <Wire.h>
#include <VL53L0X.h>

#define PIN_SERVO_PHI   6
#define PIN_SERVO_TETTA 5

Servo servoPhi;
Servo serveTetta;
VL53L0X sensor;

void smooth_move(Servo &servo, int angle, int spd) {
  int curr = servo.read();
  int diff = angle - curr;
  int steps = abs(diff);
  for (int i = 1; i <= steps; i++) {
    int next = (diff > 0) ? curr + i : curr - i;
    servo.write(next);
    delay((10 - spd) * 10);
  }
}

void setup() {
  Serial.begin(115200);
  servoPhi.attach(PIN_SERVO_PHI);
  serveTetta.attach(PIN_SERVO_TETTA);
  servoPhi.write(90);
  serveTetta.write(100);
  delay(1000);
  Wire.begin();
  sensor.init();
  sensor.setTimeout(500);
  sensor.setMeasurementTimingBudget(200000);
  Serial.println("READY");
}

void loop() {
  if (Serial.available() > 0) {
    String s = Serial.readString();
    s.trim();
    int phi_min, phi_max, tetta_min, tetta_max, steps, v;
    int parsed = sscanf(s.c_str(), "%d %d %d %d %d %d",
                        &phi_min, &phi_max,
                        &tetta_min, &tetta_max,
                        &steps, &v);
    if (parsed != 6) {
      Serial.println("ERROR");
      return;
    }
    if (steps < 2)  steps = 2;
    if (steps > 50) steps = 50;
    if (v < 1) v = 1;
    if (v > 10) v = 10;

    Serial.println("OK");

    float phi_step   = (float)(phi_max - phi_min) / (steps - 1);
    float tetta_step = (float)(tetta_max - tetta_min) / (steps - 1);

    // ВНЕШНИЙ — азимут
    for (int i = 0; i < steps; i++) {
      int phi = phi_min + (int)(phi_step * i);
      smooth_move(servoPhi, phi, v);
      delay(200);

      // ВНУТРЕННИЙ — зенит
      for (int j = 0; j < steps; j++) {
        int tetta = tetta_min + (int)(tetta_step * j);
        smooth_move(serveTetta, tetta, v);

        delay(100);
        int dist = (int)sensor.readRangeSingleMillimeters();
        if (sensor.timeoutOccurred()) dist = 0;
        delay(100);

        Serial.println(dist);
      }
    }
    Serial.println("DONE");
  }
}