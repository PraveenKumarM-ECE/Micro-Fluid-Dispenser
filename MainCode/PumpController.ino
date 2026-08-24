#include "Config.h"
#include "MotorControl.h"
#include "EEPROMStorage.h"
#include "SerialCommands.h"
#include "StatusPrinting.h"
#include "LEDControl.h"
#include "ProcessControl.h"

void setup() {
  pinMode(INDI_SEL1, OUTPUT);
  pinMode(INDI_SEL2, OUTPUT);

  pinMode(MTR_IN1, OUTPUT);
  pinMode(MTR_IN2, OUTPUT);
  pinMode(START, INPUT_PULLUP);

  pinMode(DIN1, INPUT_PULLUP);
  pinMode(DIN2, INPUT_PULLUP);
  pinMode(DIN3, INPUT_PULLUP);

  pinMode(SW2, INPUT_PULLUP);  // new switch input

  motorStop();

  mySerial.begin(9600);

  delay(100);

  // Load saved timing parameters
  loadParameters();

  // Load process state and resume if valid
  loadProcessState();

  mySerial.println(F("ATtiny3224 Pump Controller Ready"));
  mySerial.println(F("Commands:"));
  mySerial.println(F("  PON30 <ms>   -> set Pre pump ON time for 30 days"));
  mySerial.println(F("  POFF30 <ms>  -> set Pre pump OFF time for 30 days"));
  mySerial.println(F("  MON30 <ms>   -> set Main pump ON time for 30 days"));
  mySerial.println(F("  MOFF30 <ms>  -> set Main pump OFF time for 30 days"));
  mySerial.println(F("  PON60 <ms>   -> set Pre pump ON time for 60 days"));
  mySerial.println(F("  POFF60 <ms>  -> set Pre pump OFF time for 60 days"));
  mySerial.println(F("  MON60 <ms>   -> set Main pump ON time for 60 days"));
  mySerial.println(F("  MOFF60 <ms>  -> set Main pump OFF time for 60 days"));
  mySerial.println(F("  TOT          -> print total cycles count"));
  mySerial.println(F("  TOT RST       -> reset total cycles count to 0"));
  mySerial.println(F("  READ         -> read stored values"));
  mySerial.println(F("  FON          -> run motor forward"));
  mySerial.println(F("  RON          -> run motor reverse"));
  mySerial.println();

  printParameters();
}

void loop() {
  handleCommands();
  runProcess();
  updateLEDs();
  handleStatusPrinting();
}
