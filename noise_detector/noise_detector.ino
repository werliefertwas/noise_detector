
#include <Bridge.h>
#include <SimpleTimer.h>
#include <MAX7219_8x8_matrix.h>

//    MAX7219_8x8_matrix(DIN, CS, CLK);
MAX7219_8x8_matrix LEDmatrix(2,3,4);
const int DELAY_TIME = 20;  //ms
int matrix[8] = {0,0,0,0,0,0,0,0};

// Relay Output
int relayPin = 5;

// Analog Input
const int lightSensorPin = 1;
const int LIGHTSENSOR_OFFSET = 420;

const int noiseMeanValue = 620;

const int noiseSensorPin = 0;   // the microphone connection pin

// these variables will change:
int noiseSensorValue = 0;      // variable to store the value read from the sensor pin
int lightSensorValue = 0;

const int dataUpdateInterval = 1000;
const int dataPrintInterval = 500;

const char *noiseApiUrl = "https://secure-spire-62884.herokuapp.com/api/events";

// timer for periodic data push
SimpleTimer dataPushTimer;


void pushData() {
  // connect to elixir server and deliver some data
  Process p;
  char command[500];
  sprintf(
    command,
    "curl -k -H \"Content-Type: application/json\" -X POST -d '{\"event\": {\"label\":\"noise_level\",\"value\":\"%i\"}}' %s",
    noiseSensorValue,
    noiseApiUrl
  );
  p.runShellCommandAsynchronously(command);

  sprintf(
    command,
    "curl -k -H \"Content-Type: application/json\" -X POST -d '{\"event\": {\"label\":\"light_level\",\"value\":\"%i\"}}' %s",
    lightSensorValue,
    noiseApiUrl
  );
  p.runShellCommandAsynchronously(command);
}

void printData() {
  Serial.print("Noise Value: ");
  Serial.println(noiseSensorValue);

  Serial.print("Light Value: ");
  Serial.println(lightSensorValue);
}

void setup() {
  Serial.begin(9600);
  Bridge.begin();

  pinMode(noiseSensorPin, INPUT);      // noise sensor pin / microphone
  pinMode(lightSensorPin, INPUT);      // Analog in for light measurement

  pinMode(relayPin, OUTPUT);           // Digital out for relay module
  digitalWrite(relayPin, HIGH);

  LEDmatrix.clear();                   // 8x8 LED matrix
  LEDmatrix.setBrightness(8);
  
  dataPushTimer.setInterval(dataUpdateInterval, pushData);
  dataPushTimer.setInterval(dataPrintInterval, printData);
}

void loop() {
  // read the sensor and store it in the variable noiseSensorValue:
  noiseSensorValue = analogRead(noiseSensorPin);
  lightSensorValue = analogRead(lightSensorPin);

  int barGraphValue = lightSensorValue - LIGHTSENSOR_OFFSET;
  if (barGraphValue > 0) { barGraphValue /= 70; }
  else { barGraphValue = 0; }

  if (barGraphValue >= 8) {
    digitalWrite(relayPin, LOW);
  } else {
    digitalWrite(relayPin, HIGH);
  }

  matrix_push(barGraphValue);
  output_matrix();

  // periodic check for timer run
  dataPushTimer.run();

  delay(20);  // delay to avoid overloading the serial port buffer
}


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



