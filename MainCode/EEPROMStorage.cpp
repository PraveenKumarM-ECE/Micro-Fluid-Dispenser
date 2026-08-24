#include "EEPROMStorage.h"
#include "Config.h"
#include <EEPROM.h>

void loadParameters() {
  EEPROM.get(EEPROM_PON_30_ADDR, PON_30);
  EEPROM.get(EEPROM_POFF_30_ADDR, POFF_30);
  EEPROM.get(EEPROM_MON_30_ADDR, MON_30);
  EEPROM.get(EEPROM_MOFF_30_ADDR, MOFF_30);

  EEPROM.get(EEPROM_PON_60_ADDR, PON_60);
  EEPROM.get(EEPROM_POFF_60_ADDR, POFF_60);
  EEPROM.get(EEPROM_MON_60_ADDR, MON_60);
  EEPROM.get(EEPROM_MOFF_60_ADDR, MOFF_60);

  EEPROM.get(EEPROM_CYCLE_ADDR, cycleCount);

  // Validate time ranges and cycleCount
  if (PON_30 == 0 || PON_30 > 10000) PON_30 = 500;
  if (POFF_30 == 0 || POFF_30 > 10000) POFF_30 = 500;
  if (MON_30 == 0 || MON_30 > 10000) MON_30 = 500;
  if (MOFF_30 == 0 || MOFF_30 > 10000) MOFF_30 = 500;

  if (PON_60 == 0 || PON_60 > 10000) PON_60 = 1000;
  if (POFF_60 == 0 || POFF_60 > 10000) POFF_60 = 1000;
  if (MON_60 == 0 || MON_60 > 10000) MON_60 = 1000;
  if (MOFF_60 == 0 || MOFF_60 > 10000) MOFF_60 = 1000;

  if (cycleCount > 1000000) cycleCount = 0; // arbitrary large limit
}

void saveParameters() {
  EEPROM.put(EEPROM_PON_30_ADDR, PON_30);
  EEPROM.put(EEPROM_POFF_30_ADDR, POFF_30);
  EEPROM.put(EEPROM_MON_30_ADDR, MON_30);
  EEPROM.put(EEPROM_MOFF_30_ADDR, MOFF_30);

  EEPROM.put(EEPROM_PON_60_ADDR, PON_60);
  EEPROM.put(EEPROM_POFF_60_ADDR, POFF_60);
  EEPROM.put(EEPROM_MON_60_ADDR, MON_60);
  EEPROM.put(EEPROM_MOFF_60_ADDR, MOFF_60);

  EEPROM.put(EEPROM_CYCLE_ADDR, cycleCount);
}

void saveProcessState(unsigned long duration, unsigned long startTime) {

  unsigned long elapsed = millis() - startTime;

  if (elapsed >= duration) remainingTime = 0;
  else remainingTime = duration - elapsed;

  uint8_t ps = (uint8_t)processState;

  EEPROM.put(EEPROM_PROCESS_STATE_ADDR, ps);
  EEPROM.put(EEPROM_PREPUMP_COUNT_ADDR, prePumpCyclesDone);
  EEPROM.put(EEPROM_TIMER_START_ADDR, remainingTime);
}

// Load saved process state from EEPROM
void loadProcessState() {
  uint8_t ps;
  EEPROM.get(EEPROM_PROCESS_STATE_ADDR, ps);
  EEPROM.get(EEPROM_PREPUMP_COUNT_ADDR, prePumpCyclesDone);
  EEPROM.get(EEPROM_TIMER_START_ADDR, timerStart);

  if (ps <= REVERSING) {
    processState = (ProcessState)ps;
    mySerial.print(F("Resuming process at state: "));
    mySerial.println(ps);
  } else {
    // invalid saved state, reset
    processState = IDLE;
    prePumpCyclesDone = 0;
    timerStart = 0;
  }

  // For safety: If in IDLE or invalid states, reset timers and counts
  if (processState == IDLE) {
    prePumpCyclesDone = 0;
    timerStart = 0;
  }
}

void printParameters() {
  mySerial.println(F("Stored Parameters for 30 days mode:"));
  mySerial.print(F("PON30=")); mySerial.println(PON_30);
  mySerial.print(F("POFF30=")); mySerial.println(POFF_30);
  mySerial.print(F("MON30=")); mySerial.println(MON_30);
  mySerial.print(F("MOFF30=")); mySerial.println(MOFF_30);
  mySerial.println(F("Stored Parameters for 60 days mode:"));
  mySerial.print(F("PON60=")); mySerial.println(PON_60);
  mySerial.print(F("POFF60=")); mySerial.println(POFF_60);
  mySerial.print(F("MON60=")); mySerial.println(MON_60);
  mySerial.print(F("MOFF60=")); mySerial.println(MOFF_60);
  mySerial.print(F("TOT (cycles)=")); mySerial.println(cycleCount);
}
