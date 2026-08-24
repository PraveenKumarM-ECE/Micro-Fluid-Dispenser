#include "LEDControl.h"
#include "Config.h"
#include "MotorControl.h"

// State management for LEDs based on sensors and machine state
void updateLEDs() {
  bool sensor1 = readSensor(DIN1);
  bool sensor2 = readSensor(DIN2);
  bool sensor3 = readSensor(DIN3);

  // If motor is reversing, keep blue LED ON until motor stops
  if (processState == REVERSING) {
    digitalWrite(INDI_SEL2, HIGH);  // BLUE LED stays ON
    digitalWrite(INDI_SEL1, LOW);   // GREEN LED OFF
    return; // skip other state logic
  }

  switch (currentState) {

    case INSTALL_REFILL:
    case REPLACE_REFILL:
      // Blue LED blinks, others off
      digitalWrite(INDI_SEL2, millis() % 50 < 10 ? HIGH : LOW);
      digitalWrite(INDI_SEL1, LOW);
      break;

    case READY:
      // Green LED blinks
      digitalWrite(INDI_SEL2, LOW);
      digitalWrite(INDI_SEL1, millis() % 350 < 150 ? HIGH : LOW);
      break;

    case RUNNING:
      // Green LED solid ON
      digitalWrite(INDI_SEL2, HIGH);
      digitalWrite(INDI_SEL1, HIGH);
      break;

    case EMPTY_REVERSE:
      // Both RED and GREEN LED
      digitalWrite(INDI_SEL1, LOW);
      digitalWrite(INDI_SEL2, HIGH);
      break;
  }
}
