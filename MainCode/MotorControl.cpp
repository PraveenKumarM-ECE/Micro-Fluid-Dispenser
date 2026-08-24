#include "MotorControl.h"
#include "Config.h"

// Helper: read sensor with logic inversion for NC/NO if needed
bool readSensor(int pin) {
  return digitalRead(pin) == HIGH; // assuming HIGH means active trigger (NC close or NO close)
}

void motorForward() {
  digitalWrite(MTR_IN1, HIGH);
  digitalWrite(MTR_IN2, LOW);
}

void motorReverse() {
  digitalWrite(MTR_IN1, LOW);
  digitalWrite(MTR_IN2, HIGH);
}

void motorStop() {
  digitalWrite(MTR_IN1, LOW);
  digitalWrite(MTR_IN2, LOW);
}
