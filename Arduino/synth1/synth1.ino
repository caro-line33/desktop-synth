const uint8_t rowPins[] = {15, 14};
const uint8_t colPins[] = {22, 21};

bool prev[2][2] = {}; // state tracker array for key on/off

void setup() {
  for (uint8_t c = 0; c < 2; c++) pinMode(colPins[c], INPUT_PULLUP); // col pins input pullup
  for (uint8_t r = 0; r < 2; r++) pinMode(rowPins[r], INPUT); // row pins hi Z
  Serial.println("hello!");
}



void loop() {
  
  for (uint8_t r = 0; r < 2; r++) { // activate this row, others stay high-Z
    // set pins to output low (voltage = ground)
    pinMode(rowPins[r], OUTPUT);
    digitalWrite(rowPins[r], LOW);

    delayMicroseconds(3);  // settle
    for (uint8_t c = 0; c < 2; c++) {
      bool pressed = (digitalRead(colPins[c]) == LOW);

      if (pressed && !prev[r][c]) { // dont keep sensing it
        Serial.println("hello");
        pinMode(6, OUTPUT);
        tone(6, 50000, 100); //Set the voltage to high and makes a noise
        Serial.println(r);
        Serial.println(c);
      }

      else if (!pressed && prev[r][c]) {
          Serial.println("release");
          noTone(6); //Sets the voltage to low and makes no noise
          pinMode(6, INPUT);
      }
      prev[r][c] = pressed; // record pressed or not in the state tracker array
    }

    pinMode(rowPins[r], INPUT); // set back to hi Z and move on to next
  }

  delay(1);  // small scan delay
}


// switch (expression) {
//     case constant1:
//         // Code to execute if expression matches constant1
//         break;
//     case constant2:
//         // Code to execute if expression matches constant2
//         break;
//     // ... more case statements
//     default:
//         // Code to execute if no case matches
//         break;
// }
