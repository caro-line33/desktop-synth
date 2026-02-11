#pragma once
#include <Arduino.h>
#include "tones.h"
// #include "synth.h"
#include <vector>
#include <map>
#include <tuple>

void idleAllColsAsInputs();
void keyboardStart();
void driveOneColumn(uint8_t cIdx);
bool readRowPressed(uint8_t rIdx);
void scanOnce();