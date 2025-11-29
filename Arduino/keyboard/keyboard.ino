#include <Arduino.h>
#include <vector>
#include <map>
#include <tuple>

const std::vector<float> Tones = {
  8.18, 8.66, 9.18, 9.72, 10.30, 10.91, 11.56, 12.25, 12.98, 13.75, 14.57, 15.43,
  16.35, 17.32, 18.35, 19.45, 20.60, 21.83, 23.12, 24.50, 25.96, 27.50, 29.14, 30.87,
  32.70, 34.65, 36.71, 38.89, 41.20, 43.65, 46.25, 49.00, 51.91, 55.00, 58.27, 61.74,
  65.41, 69.30, 73.42, 77.78, 82.41, 87.31, 92.50, 98.00, 103.83, 110.00, 116.54, 123.47,
  130.81, 138.59, 146.83, 155.56, 164.81, 174.61, 185.00, 196.00, 207.65, 220.00, 233.08, 246.94,
  261.63, 277.18, 293.66, 311.13, 329.63, 349.23, 369.99, 392.00, 415.30, 440.00, 466.16, 493.88,
  523.25, 554.37, 587.33, 622.25, 659.25, 698.46, 739.99, 783.99, 830.61, 880.00, 932.33, 987.77,
  1046.50, 1108.73, 1174.66, 1244.51, 1318.51, 1396.91, 1479.98, 1567.98, 1661.22, 1760.00, 1864.66, 1975.53,
  2093.00, 2217.46, 2349.32, 2489.02, 2637.02, 2793.83, 2959.96, 3135.96, 3322.44, 3520.00, 3729.31, 3951.07,
  4186.01, 4434.92, 4698.63, 4978.03, 5274.04, 5587.65, 5919.91, 6271.93, 6644.88, 7040.00, 7458.62, 7902.13,
  8372.02, 8869.84, 9397.27, 9956.06, 10548.08, 11175.30, 11839.82, 12543.85
};

const std::map<std::tuple<int,int>, int> note_identifier = {
  {{27,3}, 0}, {{27,2}, 1}, {{27,1}, 2}, {{27,0}, 3}, {{27,32}, 4}, {{27,31}, 5}, {{27,30}, 6}, {{27,29}, 7},
  {{26,3}, 8}, {{26,2}, 9}, {{26,1}, 10}, {{26,0}, 11}, {{26,32}, 12}, {{26,31}, 13}, {{26,30}, 14}, {{26,29}, 15},
  {{25,3}, 16}, {{25,2}, 17}, {{25,1}, 18}, {{25,0}, 19}, {{25,32}, 20}, {{25,31}, 21}, {{25,30}, 22}, {{25,29}, 23},
  {{5,3}, 24}
};

const uint8_t ROW_PINS[] = {3, 2, 1, 0, 32, 31, 30, 29};
const uint8_t COL_PINS[] = {27, 26, 25, 5};

const uint8_t NUM_ROWS = sizeof(ROW_PINS) / sizeof(ROW_PINS[0]);
const uint8_t NUM_COLS = sizeof(COL_PINS) / sizeof(COL_PINS[0]);

// Scan timing
const uint16_t settle_us = 33;     // settle after driving a column
const uint16_t scan_delay_ms = 2;  // pause between scans

int shift = 0 // adjust using rotary encoder
int tone_type = 0 // adjust using button

bool lastState[NUM_ROWS][NUM_COLS];

int keyIdx[NUM_COLS][NUM_ROWS] = {
  {0, 1, 2, 3, 4, 5, 6, 7},
  {8, 9, 10, 11, 12, 13, 14, 15},
  {16, 17, 18, 19, 20, 21, 22, 23},
  {24, -1, -1, -1, -1, -1, -1, -1},
};

static inline void idleAllColsAsInputs() {
  for (uint8_t c = 0; c < NUM_COLS; ++c) {
    pinMode(COL_PINS[c], INPUT_PULLUP);  // hi-Z with weak pullup
  }
}

void setup() {
  Serial.begin(115200);
  delay(300);

  idleAllColsAsInputs();
  for (uint8_t r = 0; r < NUM_ROWS; ++r) {
    pinMode(ROW_PINS[r], INPUT_PULLUP);
  }

  // Init edge tracker
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

int lookupNoteIndex(uint8_t colIdx, uint8_t rowIdx) {
  int colPin = COL_PINS[colIdx];
  int rowPin = ROW_PINS[rowIdx];

  auto key = std::make_tuple(colPin, rowPin);
  auto it  = note_identifier.find(key);

  if (it != note_identifier.end()) {
    return it->second;   // valid mapping
  } else {
    return -1;
  }
}

void scanOnce() {
  for (uint8_t c = 0; c < NUM_COLS; ++c) {
    driveOneColumn(c);

    for (uint8_t r = 0; r < NUM_ROWS; ++r) {
      bool pressed = readRowPressed(r);

      if (pressed && !lastState[r][c]) {
        Serial.print("Press:  COL pin "); Serial.print(COL_PINS[c]);
        Serial.print(" -> ROW pin "); Serial.println(ROW_PINS[r]);

        Serial.println(keyIdx[c][r]);
        Serial.println(Tones[keyIdx[c][r]]);

      } else if (!pressed && lastState[r][c]) {
        Serial.print("Release: COL pin "); Serial.print(COL_PINS[c]);
        Serial.print(" -> ROW pin "); Serial.println(ROW_PINS[r]);

        int note_index = lookupNoteIndex(c, r);
        if (note_index >= 0 && note_index < (int)Tones.size()) {
          Serial.print("Freq: ");
          Serial.println(Tones[note_index]);
        }
      }

      lastState[r][c] = pressed;
    }
  }
  idleAllColsAsInputs();
}


void loop() {
  scanOnce();
  delay(scan_delay_ms);
}
