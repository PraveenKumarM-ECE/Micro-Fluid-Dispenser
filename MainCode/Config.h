#ifndef CONFIG_H
#define CONFIG_H

#include <Arduino.h>
#include <SoftwareSerial.h>

// Program for 2 LED
// LED Pins
#define INDI_SEL1 PIN_PB0   // GREEN
#define INDI_SEL2 PIN_PB1   // BLUE
#define TX_PIN    PIN_PA2
#define RX_PIN    PIN_PA1
#define MTR_IN1   PIN_PA6
#define MTR_IN2   PIN_PA7
#define START     PIN_PA5
#define DIN1 PIN_PB3   // CONTAINER FULL
#define DIN2 PIN_PB2   // CONTAINER EMPTY 
#define DIN3 PIN_PA4   // REFILL 
#define SW2 PIN_PA3   // 30/60 days switch (NEW)

// Software Serial instance
extern SoftwareSerial mySerial;

// Pump timing variables for two modes (ms)
extern unsigned int PON_30, POFF_30, MON_30, MOFF_30;
extern unsigned int PON_60, POFF_60, MON_60, MOFF_60;
extern unsigned long cycleCount; // Total cycles count
extern unsigned long remainingTime;
extern bool stsMode;
extern unsigned long stsTimer;

// EEPROM address allocation for 30 days params
#define EEPROM_PON_30_ADDR   0
#define EEPROM_POFF_30_ADDR  (EEPROM_PON_30_ADDR + sizeof(PON_30))
#define EEPROM_MON_30_ADDR   (EEPROM_POFF_30_ADDR + sizeof(POFF_30))
#define EEPROM_MOFF_30_ADDR  (EEPROM_MON_30_ADDR + sizeof(MON_30))

// EEPROM address allocation for 60 days params
#define EEPROM_PON_60_ADDR   (EEPROM_MOFF_30_ADDR + sizeof(MOFF_30))
#define EEPROM_POFF_60_ADDR  (EEPROM_PON_60_ADDR + sizeof(PON_60))
#define EEPROM_MON_60_ADDR   (EEPROM_POFF_60_ADDR + sizeof(POFF_60))
#define EEPROM_MOFF_60_ADDR  (EEPROM_MON_60_ADDR + sizeof(MON_60))

#define EEPROM_CYCLE_ADDR    (EEPROM_MOFF_60_ADDR + sizeof(MOFF_60))

// New EEPROM addresses for process state saving
#define EEPROM_PROCESS_STATE_ADDR   (EEPROM_CYCLE_ADDR + sizeof(cycleCount))
#define EEPROM_PREPUMP_COUNT_ADDR   (EEPROM_PROCESS_STATE_ADDR + sizeof(uint8_t))
#define EEPROM_TIMER_START_ADDR     (EEPROM_PREPUMP_COUNT_ADDR + sizeof(uint8_t))

// Machine states
enum MachineState {
  INSTALL_REFILL,
  READY,
  RUNNING,
  EMPTY_REVERSE,
  REPLACE_REFILL
};

extern MachineState currentState;

// Process states for runProcess
enum ProcessState {
  IDLE,
  PRE_PUMP_ON,
  PRE_PUMP_OFF,
  WAITING,
  MAIN_PUMP_ON,
  MAIN_PUMP_OFF,
  REVERSING
};

extern ProcessState processState;

// Timing control variables
extern unsigned long waitStartTime;
//const unsigned long WAIT_15_MIN = 15UL * 60 * 1000UL; // 15 minutes in ms
extern const unsigned long WAIT_15_MIN; // 15 seconds for testing

// Pre-pump fixed repetition count
extern const uint8_t PRE_PUMP_REPEATS;

// Variables for process timing and count restored from EEPROM
extern uint8_t prePumpCyclesDone;
extern unsigned long timerStart;

extern bool stateJustEntered;

#endif
