#include "SerialCommands.h"
#include "Config.h"
#include "MotorControl.h"
#include "EEPROMStorage.h"
#include <EEPROM.h>

void handleCommands() {
  if (mySerial.available()) {
    char cmd[32];
    byte len = mySerial.readBytesUntil('\n', cmd, sizeof(cmd) - 1);
    cmd[len] = '\0';

    if (strncmp(cmd, "PON30", 5) == 0) {
      int v = atoi(cmd + 5);
      if (v > 0) {
        PON_30 = v;
        EEPROM.put(EEPROM_PON_30_ADDR, PON_30);
        mySerial.print(F("✅ Pre pump ON time (30 days) updated: ")); mySerial.println(PON_30);
      }
    }
    else if (strncmp(cmd, "POFF30", 6) == 0) {
      int v = atoi(cmd + 6);
      if (v > 0) {
        POFF_30 = v;
        EEPROM.put(EEPROM_POFF_30_ADDR, POFF_30);
        mySerial.print(F("✅ Pre pump OFF time (30 days) updated: ")); mySerial.println(POFF_30);
      }
    }
    else if (strncmp(cmd, "MON30", 5) == 0) {
      int v = atoi(cmd + 5);
      if (v > 0) {
        MON_30 = v;
        EEPROM.put(EEPROM_MON_30_ADDR, MON_30);
        mySerial.print(F("✅ Main pump ON time (30 days) updated: ")); mySerial.println(MON_30);
      }
    }
    else if (strncmp(cmd, "MOFF30", 6) == 0) {
      int v = atoi(cmd + 6);
      if (v > 0) {
        MOFF_30 = v;
        EEPROM.put(EEPROM_MOFF_30_ADDR, MOFF_30);
        mySerial.print(F("✅ Main pump OFF time (30 days) updated: ")); mySerial.println(MOFF_30);
      }
    }
    else if (strncmp(cmd, "PON60", 5) == 0) {
      int v = atoi(cmd + 5);
      if (v > 0) {
        PON_60 = v;
        EEPROM.put(EEPROM_PON_60_ADDR, PON_60);
        mySerial.print(F("✅ Pre pump ON time (60 days) updated: ")); mySerial.println(PON_60);
      }
    }
    else if (strncmp(cmd, "POFF60", 6) == 0) {
      int v = atoi(cmd + 6);
      if (v > 0) {
        POFF_60 = v;
        EEPROM.put(EEPROM_POFF_60_ADDR, POFF_60);
        mySerial.print(F("✅ Pre pump OFF time (60 days) updated: ")); mySerial.println(POFF_60);
      }
    }
    else if (strncmp(cmd, "MON60", 5) == 0) {
      int v = atoi(cmd + 5);
      if (v > 0) {
        MON_60 = v;
        EEPROM.put(EEPROM_MON_60_ADDR, MON_60);
        mySerial.print(F("✅ Main pump ON time (60 days) updated: ")); mySerial.println(MON_60);
      }
    }
    else if (strncmp(cmd, "MOFF60", 6) == 0) {
      int v = atoi(cmd + 6);
      if (v > 0) {
        MOFF_60 = v;
        EEPROM.put(EEPROM_MOFF_60_ADDR, MOFF_60);
        mySerial.print(F("✅ Main pump OFF time (60 days) updated: ")); mySerial.println(MOFF_60);
      }
    }

//-----STATUS----------

    else if (strcasecmp(cmd, "STS") == 0) {
      stsMode = true;
      mySerial.println(F("✅ Continuous Status (STS) enabled. Send X to stop."));
    }
    else if (strcasecmp(cmd, "X") == 0) {    // stop continuous mode
      stsMode = false;
      mySerial.println(F("❌ Continuous Status (STS) disabled."));
    }
    
    
    else if (strcasecmp(cmd, "READ") == 0) {
      printParameters();
    }
    else if (strcasecmp(cmd, "TOT") == 0) {
      mySerial.print(F("🔢 Total Cycles Completed: "));
      mySerial.println(cycleCount);
    }
    else if (strcasecmp(cmd, "TOT RST") == 0) {
      cycleCount=0;
      EEPROM.put(EEPROM_CYCLE_ADDR, cycleCount);
      mySerial.println(F("🔄 Total Cycles Count RESET to 0."));
    }
    
    else if (strcasecmp(cmd, "FON") == 0) {
      motorForward();
      mySerial.println(F("🟢 Motor running FORWARD"));
    }
    else if (strcasecmp(cmd, "RON") == 0) {
      motorReverse();
      mySerial.println(F("🔵 Motor running REVERSE"));
    }
    else {
      mySerial.println(F("? Invalid command"));
    }
  }
}
