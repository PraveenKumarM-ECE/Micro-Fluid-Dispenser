#ifndef EEPROM_STORAGE_H
#define EEPROM_STORAGE_H

#include <Arduino.h>

void loadParameters();
void saveParameters();
void printParameters();
void saveProcessState(unsigned long duration, unsigned long startTime);
void loadProcessState();

#endif
