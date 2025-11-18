const int MAX_BUTTONS = 2;
int frequencies[MAX_BUTTONS];
int count = 0;
String output = "";


void setup() {
  // put your setup code here, to run once:
  Serial.begin(38400);
  pinMode(7, INPUT_PULLUP);
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(9, INPUT_PULLUP);
}

void loop() {
  // put your main code here, to run repeatedly:
  bool pressed = digitalRead(7) == LOW;
  bool pressed2 = digitalRead(9) == LOW;
  
  count = 0;
  if (pressed){
    frequencies[count++] = 440;
  }

  if(pressed2){
    frequencies[count++] = 550;
  }

  output = "";
  for(int i = 0; i < count; i++){
    output = output +  String(frequencies[i]) + " ";
  }
  Serial.println(output);

  delay(100);
}
