#include "StatusPrinting.h"
#include "Config.h"
#include "MotorControl.h"
#include <EEPROM.h>

void handleStatusPrinting() {
  if (stsMode) {
    unsigned long now = millis();
    if (now - stsTimer >= 100) { // Check timer for non-blocking delay
      stsTimer = now;

      // Your original status gathering logic
      bool sensor1 = !readSensor(DIN1); // Note: Corrected sensor reading logic to match runProcess()
      bool sensor2 = readSensor(DIN2);
      bool sensor3 = !readSensor(DIN3);

      // detect process state
      const char* stateName;
      switch (processState) {
        case IDLE:
          stateName = "IDLE";
          break;
        case PRE_PUMP_ON:
        case PRE_PUMP_OFF:
          stateName = "PRE PUMPING";
          break;
        case WAITING:
          stateName = "WAITING";
          break;
        case MAIN_PUMP_ON:
        case MAIN_PUMP_OFF:
          stateName = "MAIN PUMPING";
          break;
        case REVERSING:
          stateName = "REVERSING";
          break;
        default:
          stateName = "Unknown";
      }

      uint32_t currentCycleCount; // Use uint32_t as cycleCount is unsigned long
      EEPROM.get(EEPROM_CYCLE_ADDR, currentCycleCount);

      // Get mode (0=30, 1=60)
      bool sw2 = (digitalRead(SW2) == LOW);
      
      // print in one line
      mySerial.print(stateName);
      mySerial.print("  (SEN1,"); mySerial.print(sensor2 ? 1 : 0);
      mySerial.print(") (SEN2,"); mySerial.print(sensor1 ? 1 : 0);
      mySerial.print(") (SEN3,"); mySerial.print(sensor3 ? 1 : 0);
      mySerial.print(") (MODE:"); mySerial.print(sw2 ? 30 : 60);
      mySerial.print(") (TOT:"); mySerial.print(currentCycleCount);
      mySerial.println(")");
    }
  }
}
