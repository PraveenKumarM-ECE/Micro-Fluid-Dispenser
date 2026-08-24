#include "ProcessControl.h"
#include "Config.h"
#include "MotorControl.h"
#include "EEPROMStorage.h"

void runProcess() {

  bool is30Days = (digitalRead(SW2) == LOW);
  unsigned int PON = is30Days ? PON_30 : PON_60;
  unsigned int POFF = is30Days ? POFF_30 : POFF_60;
  unsigned int MON = is30Days ? MON_30 : MON_60;
  unsigned int MOFF = is30Days ? MOFF_30 : MOFF_60;

  bool sensor1 = !readSensor(DIN1);
  bool sensor2 =  readSensor(DIN2);
  bool sensor3 = !readSensor(DIN3);

  // Update LED state machine
  if (!sensor1 && !sensor2 && !sensor3) currentState = INSTALL_REFILL;
  else if (!sensor1 && !sensor2 && sensor3) currentState = READY;
  else if (sensor1 && !sensor2 && sensor3) currentState = RUNNING;
  else if (!sensor1 && sensor2 && sensor3) currentState = EMPTY_REVERSE;
  else if (!sensor1 && !sensor2 && sensor3) currentState = REPLACE_REFILL;

  

  // ✅ FIX: NOW IS DEFINED OUTSIDE SWITCH
  unsigned long now = millis();

  switch (processState) {

    case IDLE:
      if (digitalRead(START) == LOW) {
        prePumpCyclesDone = 0;
        timerStart = now;
        processState = PRE_PUMP_ON;
        stateJustEntered = true;
        motorForward();
        mySerial.println(F("🚦 Starting Pre-Pump process"));
        saveProcessState(PON, timerStart);
      }
      break;

    case PRE_PUMP_ON:
      if (stateJustEntered) {
        mySerial.print(F("Pre pump "));
        mySerial.print(prePumpCyclesDone + 1);
        mySerial.println(F(" Started"));
        stateJustEntered = false;
      }
      if (now - timerStart >= PON) {
        motorStop();
        timerStart = now;
        processState = PRE_PUMP_OFF;
        stateJustEntered = true;
        saveProcessState(POFF, timerStart);
      }
      break;

    case PRE_PUMP_OFF:
      if (stateJustEntered) {
        mySerial.print(F("Pre pump "));
        mySerial.print(prePumpCyclesDone + 1);
        mySerial.println(F(" Completed"));
        stateJustEntered = false;
      }
      if (now - timerStart >= POFF) {
        prePumpCyclesDone++;
        if (prePumpCyclesDone < PRE_PUMP_REPEATS) {
          motorForward();
          timerStart = now;
          processState = PRE_PUMP_ON;
          stateJustEntered = true;
          saveProcessState(PON, timerStart);
        }
        else {
          timerStart = now;
          processState = WAITING;
          stateJustEntered = true;
          mySerial.println(F("⏳ Pre-Pump completed "));
          saveProcessState(WAIT_15_MIN, timerStart);
        }
      }
      break;

    case WAITING:
      if (stateJustEntered) {
        mySerial.println(F("⏳ Waiting 15 minutes"));
        stateJustEntered = false;
      }

      if (now - timerStart >= WAIT_15_MIN) {
        motorForward();
        timerStart = now;
        processState = MAIN_PUMP_ON;
        stateJustEntered = true;

        mySerial.println(F("🚦 Starting Main-Pump process"));
        saveProcessState(MON, timerStart);
      }
      break;

    case MAIN_PUMP_ON:
      if (now - timerStart >= MON) {
        motorStop();
        timerStart = now;
        processState = MAIN_PUMP_OFF;
        cycleCount++;
        saveParameters();
        saveProcessState(MOFF, timerStart);
      }
      if (sensor2) {
        motorReverse();
        timerStart = now;
        processState = REVERSING;
        mySerial.println(F("↩️ Main Pumping Completed , Reversing motor"));
        saveProcessState(0, timerStart);
      }
      break;

    case MAIN_PUMP_OFF:
      if (now - timerStart >= MOFF) {
        motorForward();
        timerStart = now;
        processState = MAIN_PUMP_ON;
        saveProcessState(MON, timerStart);
      }
      if (sensor2) {
        motorReverse();
        timerStart = now;
        processState = REVERSING;
        mySerial.println(F("↩️ Main Pumping Completed , Reversing motor"));
        saveProcessState(0, timerStart);
      }
      break;

    case REVERSING:
      if (sensor1) {
        motorStop();
        processState = IDLE;
        mySerial.println(F("⏹️ Reversing complete. Replace refill."));
        saveProcessState(0, timerStart);
      }
      break;
  }
}
