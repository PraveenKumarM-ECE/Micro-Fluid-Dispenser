#ifndef MOTOR_CONTROL_H
#define MOTOR_CONTROL_H

#include <Arduino.h>

void motorForward();
void motorReverse();
void motorStop();
bool readSensor(int pin);

#endif
