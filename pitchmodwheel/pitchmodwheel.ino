/*
* Code for making one potentiometer control 3 LEDs, red, grn and blu, or one tri-color LED
* The program cross-fades from red to grn, grn to blu, and blu to red
* Clay Shirky <clay.shirky@nyu.edu> 
*/

// INPUT: Potentiometer should be connected to 5V and GND
int modwheel = A8; // Potentiometer output connected to analog pin 3
int pitchwheel = A9;
int modVal = 0; // Variable to store the input from the potentiometer
int pitchVal = 0;

void setup()
{
}

// Main program
void loop()
{
  pitchVal = analogRead(pitchwheel);   // read the potentiometer value at the input pin
  modVal = analogRead(modwheel);
  Serial.print("pitch wheel: ");
  Serial.println(pitchVal);
  Serial.print("mod wheel: ");
  Serial.println(modVal);
  delay(500);
}