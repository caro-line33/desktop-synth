// Teensy Keybed Matrix Mapper (custom pinout)
// Rows (inputs): 0,1,2,3,4,6,7,8,31,32   <-- sensed with INPUT_PULLUP
// Cols (drive):  29,30,9,10,11,12        <-- driven one at a time
//
// Open Serial Monitor @ 115200, press keys, watch pin pairs.

#include <Arduino.h>

// === Pin assignments ===
const uint8_t ROW_PINS[] = {0, 1, 2, 3, 29, 30, 31, 32, 6};   // 10 rows
const uint8_t COL_PINS[] = {4, 5, 7, 8, 9, 10, 11, 12};            // 6 columns

// Derived counts
const uint8_t NUM_ROWS = sizeof(ROW_PINS) / sizeof(ROW_PINS[0]);
const uint8_t NUM_COLS = sizeof(COL_PINS) / sizeof(COL_PINS[0]);

// Most keybeds are active-low (rows pulled up; drive one column LOW)
const bool ACTIVE_LOW = true;

// Scan timing
const uint16_t settle_us = 50;     // settle after driving a column
const uint16_t scan_delay_ms = 2;  // pause between scans

// State tracking to reduce serial spam
bool lastState[NUM_ROWS][NUM_COLS];

static inline void idleAllColsAsInputs() {
  for (uint8_t c = 0; c < NUM_COLS; ++c) {
    pinMode(COL_PINS[c], INPUT_PULLUP);  // hi-Z with weak pullup
  }
}

void setup() {
  Serial.begin(115200);
  delay(300);

  Serial.println("Teensy keybed scan (custom pinout)");
  Serial.print("ROWS (inputs): ");
  for (uint8_t i = 0; i < NUM_ROWS; ++i) { Serial.print(ROW_PINS[i]); Serial.print(i+1<NUM_ROWS?", ":"\n"); }
  Serial.print("COLS (drive):  ");
  for (uint8_t i = 0; i < NUM_COLS; ++i) { Serial.print(COL_PINS[i]); Serial.print(i+1<NUM_COLS?", ":"\n"); }
  Serial.print("Polarity: "); Serial.println(ACTIVE_LOW ? "ACTIVE_LOW" : "ACTIVE_HIGH");

  // Safe idle
  idleAllColsAsInputs();
  for (uint8_t r = 0; r < NUM_ROWS; ++r) {
    pinMode(ROW_PINS[r], INPUT_PULLUP);
  }

  // Init edge tracker
  for (uint8_t r = 0; r < NUM_ROWS; ++r)
    for (uint8_t c = 0; c < NUM_COLS; ++c)
      lastState[r][c] = false;

  Serial.println("Press keys to see pin pairs. Expect two hits per physical key (two contacts).");
}

void driveOneColumn(uint8_t cIdx) {
  idleAllColsAsInputs();               // release others
  pinMode(COL_PINS[cIdx], OUTPUT);
  digitalWrite(COL_PINS[cIdx], ACTIVE_LOW ? LOW : HIGH); // drive selected column
  delayMicroseconds(settle_us);
}

bool readRowPressed(uint8_t rIdx) {
  int v = digitalRead(ROW_PINS[rIdx]);
  return ACTIVE_LOW ? (v == LOW) : (v == HIGH);
}

void scanOnce() {
  for (uint8_t c = 0; c < NUM_COLS; ++c) {
    driveOneColumn(c);

    for (uint8_t r = 0; r < NUM_ROWS; ++r) {
      bool pressed = readRowPressed(r);

      if (pressed && !lastState[r][c]) {
        Serial.print("Press:  COL pin "); Serial.print(COL_PINS[c]);
        Serial.print(" -> ROW pin "); Serial.println(ROW_PINS[r]);
      } else if (!pressed && lastState[r][c]) {
        Serial.print("Release: COL pin "); Serial.print(COL_PINS[c]);
        Serial.print(" -> ROW pin "); Serial.println(ROW_PINS[r]);
      }
      lastState[r][c] = pressed;
    }
  }
  idleAllColsAsInputs();  // return to safe idle between scans
}

void loop() {
  scanOnce();
  delay(scan_delay_ms);
}
