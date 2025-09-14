const uint8_t rowPins[] = {34, 35, 36, 37, 38, 39, 40};
const uint8_t colPins[] = {23, 22};

const int LED = LED_BUILTIN;     // Teensy 4.1 onboard LED = pin 13
const uint16_t flashMs = 80;     // flash length per press

bool prev[7][2] = {}; // state tracker array for key on/off
uint32_t ledOffAt = 0;

void setup() {
  pinMode(LED, OUTPUT);
  digitalWrite(LED, LOW);
  for (uint8_t c = 0; c < 2; c++) pinMode(colPins[c], INPUT_PULLUP); // col pins input pullup
  for (uint8_t r = 0; r < 7; r++) pinMode(rowPins[r], INPUT); // row pins hi Z
  Serial.println("hello!");
}

void loop() {
  for (uint8_t r = 0; r < 7; r++) { // activate this row, others stay high-Z
    // set pins to output low (voltage = ground)
    pinMode(rowPins[r], OUTPUT);
    digitalWrite(rowPins[r], LOW);

    delayMicroseconds(3);  // settle
    for (uint8_t c = 0; c < 2; c++) {
      bool pressed = (digitalRead(colPins[c]) == LOW);

      if (pressed && !prev[r][c]) { // dont keep sensing it
        Serial.println("hello");
      }

      else if (!pressed && prev[r][c]) {
          Serial.println("release");
      }
      prev[r][c] = pressed; // record pressed or not in the state tracker array
    }

    pinMode(rowPins[r], INPUT); // set back to hi Z and move on to next
  }

  delay(1);  // small scan delay
}

