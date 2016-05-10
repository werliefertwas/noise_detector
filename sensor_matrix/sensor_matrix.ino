#include <MAX7219_8x8_matrix.h>

//    MAX7219_8x8_matrix(DIN, CS, CLK);
MAX7219_8x8_matrix LEDmatrix(2,3,4);
const int DELAY_TIME = 20;  //ms
int matrix[8] = {0,0,0,0,0,0,0,0};

// Relay Output
int relayPin = 5;

// Analog Input
const int sensorPin = 0;
const int OFFSET = 120;

/*
  Generate a bitmask for the given input for the led column.
  All leds up to the given value will be turned on:
  0 = B00000000
  1 = B00000001
  2 = B00000011
  7 = B01111111
  8 = B11111111
*/
int set_leds(int input) {
  int ret = 0;
  for (int i=0; i<8; i++) {
    if (i < input) { ret |= (0x01 << i); }
  }
  return ret;
}

/*
  Write all eight columns to the LED matrix.
*/
void output_matrix(void) {
  for (int col=0;col<8;col++){
   LEDmatrix.set_col(col,set_leds(matrix[col]));
   delay(DELAY_TIME);
  }
}

/*
  Add a new value (0..8) to the output matrix.
  Since performance is no problem yet, we simply shift the matrix
  and add the new value to the first slot. As soon as performance 
  becomes an issue, we should use a pointer here and dont copy all
  values every time.
*/
void matrix_push(int val) {
  for (int i=7; i>0; i--) {
    matrix[i] = matrix[i-1];
  }
  matrix[0] = val;
}

/*
  Setup does the following stuff:
  * Init the relay output to OFF
  * Init the analog measurement
  * Init the LED Matrix with all leds turned off
*/
void setup() {
  pinMode(sensorPin, INPUT);      // Analog in for light measurement
  pinMode(relayPin, OUTPUT);      // Digital out for relay module
  digitalWrite(relayPin, HIGH);
  LEDmatrix.clear();              // 8x8 LED matrix
  LEDmatrix.setBrightness(8);
}

/*
  loop does the folloiwing stuff:
  * Read a new light value
  * scale the value to 0..8
  * Set the relay output: ON when measurement below threshold, OFF otherwise
  * Push the new value to the matrix
  * Print out the new matrix to the leds
*/
void loop() {
  int measurement;

  measurement = analogRead(sensorPin) - OFFSET;
  if (measurement > 0) { measurement /= 70; }
  else { measurement = 0; }

  if (measurement < 4) { digitalWrite(relayPin, HIGH); }
  else { digitalWrite(relayPin, LOW); }

  matrix_push(measurement);
  output_matrix();
  delay(DELAY_TIME);
}
