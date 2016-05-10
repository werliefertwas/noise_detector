#include <MAX7219_8x8_matrix.h>

//    MAX7219_8x8_matrix(DIN, CS, CLK);
MAX7219_8x8_matrix LEDmatrix(2,3,4);
int relayPin = 5;
int i = 0;
int delay_time = 20;  //ms
int m[8] = {1,1,1,1,1,1,1,1};
const int sensorPin = 0;
const int offset = 120;
int level;

int set_leds(int input) {
  int ret = 0;
  for (int i=0; i<8; i++) {
    if (i < input) { ret |= (0x01 << i); }
  }
  return ret;
}

void output_matrix(void) {
  for (int col=0;col<8;col++){
   LEDmatrix.set_col(col,set_leds(m[col]));
   delay(delay_time);
  }
}

void matrix_push(int val) {
  for (int i=7; i>0; i--) {
    m[i] = m[i-1];
  }
  m[0] = val;
}

void setup() {
  pinMode(sensorPin, INPUT);
  pinMode(relayPin, OUTPUT);
  digitalWrite(relayPin, HIGH);

  LEDmatrix.clear();
  delay(1000);
  LEDmatrix.setBrightness(8);
  randomSeed(6);
}

void loop() {
  level = analogRead(sensorPin) - offset;
  if (level > 0) { level /= 70; }
  else { level = 0; }

  if (level < 4) { digitalWrite(relayPin, HIGH); }
  else { digitalWrite(relayPin, LOW); }

  matrix_push(level);
  output_matrix();
  delay(delay_time);
}
