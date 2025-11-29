#include <Arduino.h>
#include "keyboard.h"
#include "tones.h"

int shift = 0; // adjust using rotary encoder
int tone_type = 0; // adjust using button

void setup() {
  synthInit();
  keyboardStart();
}

void loop() {
  scanOnce();
}