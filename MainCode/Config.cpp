#include "Config.h"

// Software Serial instance
SoftwareSerial mySerial(RX_PIN, TX_PIN);

// Pump timing variables for two modes (ms)
unsigned int PON_30 = 500, POFF_30 = 500, MON_30 = 500, MOFF_30 = 500;
unsigned int PON_60 = 1000, POFF_60 = 1000, MON_60 = 1000, MOFF_60 = 1000;
unsigned long cycleCount = 0; // Total cycles count
unsigned long remainingTime = 0;
bool stsMode = false;
unsigned long stsTimer = 0;

MachineState currentState = INSTALL_REFILL;

ProcessState processState = IDLE;

// Timing control variables
unsigned long waitStartTime = 0;
//const unsigned long WAIT_15_MIN = 15UL * 60 * 1000UL; // 15 minutes in ms

const unsigned long WAIT_15_MIN = 15UL * 1000UL; // 15 seconds for testing

// Pre-pump fixed repetition count
const uint8_t PRE_PUMP_REPEATS = 3;

// Variables for process timing and count restored from EEPROM
uint8_t prePumpCyclesDone = 0;
unsigned long timerStart = 0;

bool stateJustEntered = false;
