#include "keyboard.h"
#include "synth.h"
#include "controls.h"

const uint16_t settle_us = 5;     // settle after driving a column
const uint16_t scan_delay_ms = 1;  // pause between scans

const uint8_t ROW_PINS[] = {3, 2, 1, 0, 32, 31, 30, 29};
const uint8_t COL_PINS[] = {27, 26, 25, 5};
const uint8_t NUM_ROWS = sizeof(ROW_PINS) / sizeof(ROW_PINS[0]);
const uint8_t NUM_COLS = sizeof(COL_PINS) / sizeof(COL_PINS[0]);

bool lastState[NUM_ROWS][NUM_COLS];

int keyIdx[NUM_COLS][NUM_ROWS] = {
  {0, 1, 2, 3, 4, 5, 6, 7},
  {8, 9, 10, 11, 12, 13, 14, 15},
  {16, 17, 18, 19, 20, 21, 22, 23},
  {24, -1, -1, -1, -1, -1, -1, -1}
};

void idleAllColsAsInputs() {
  for (uint8_t c = 0; c < NUM_COLS; ++c) {
    pinMode(COL_PINS[c], INPUT_PULLUP);
  }
}

void keyboardStart() {
  Serial.begin(115200);
  delay(300);

  idleAllColsAsInputs();
  for (uint8_t r = 0; r < NUM_ROWS; ++r) {
    pinMode(ROW_PINS[r], INPUT_PULLUP);
  }

  for (uint8_t r = 0; r < NUM_ROWS; ++r)
    for (uint8_t c = 0; c < NUM_COLS; ++c)
      lastState[r][c] = false;
}

void driveOneColumn(uint8_t cIdx) {
  idleAllColsAsInputs();
  pinMode(COL_PINS[cIdx], OUTPUT);
  digitalWrite(COL_PINS[cIdx], LOW);
  delayMicroseconds(settle_us);
}

bool readRowPressed(uint8_t rIdx) {
  int v = digitalRead(ROW_PINS[rIdx]);
  return (v == LOW);
}

void scanOnce() {
  for (uint8_t c = 0; c < NUM_COLS; ++c) {
    driveOneColumn(c);

    for (uint8_t r = 0; r < NUM_ROWS; ++r) {
      bool pressed = readRowPressed(r);

      if (pressed && !lastState[r][c]) {
        int key_idx = keyIdx[c][r];
        Serial.print("Press: key "); Serial.println(key_idx);
        noteOn(key_idx, Tones[key_idx]);

      } else if (!pressed && lastState[r][c]) {
        int key_idx = keyIdx[c][r];
        Serial.print("Release: key "); Serial.println(key_idx);
        noteOff(key_idx);
      }

      lastState[r][c] = pressed;
    }
  }
  idleAllColsAsInputs();
  delay(scan_delay_ms);
}