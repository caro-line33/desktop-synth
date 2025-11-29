const int to_test[] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 14, 15, 16, 18, 19, 20, 21};
const size_t N = sizeof(to_test) / sizeof(to_test[0]);

const int LED_PIN = 17;

void setup() {
  // set all test pins to INPUT_PULLUP so they can sense if they are connected to ground.
  for (size_t i = 0; i < N; ++i) {
    pinMode(to_test[i], INPUT_PULLUP);
  }

  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, HIGH); // start ON (high turns internal LED off but turns external LED on bc of diode polarity)
}

void loop() {
  for (size_t i = 0; i < N; ++i) {
    if (digitalRead(to_test[i]) == LOW) { // if grounded
      digitalWrite(LED_PIN, LOW);  // ON
      delay(200);
      digitalWrite(LED_PIN, HIGH); // OFF
      delay(200);
    }
  }
}
