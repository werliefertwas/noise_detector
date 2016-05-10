
const int ledPinGreen = 13;
const int ledPinYel = 12;
const int ledPinRed = 11;
const int sensorPin = 0;

int level;

const int thresholdDark = 850;
const int thresholdLight = 900;

// the setup function runs once when you press reset or power the board
void setup() {
  pinMode(ledPinGreen, OUTPUT);
  pinMode(ledPinYel, OUTPUT);
  pinMode(ledPinRed, OUTPUT);
  pinMode(sensorPin, INPUT);
  Serial.begin(9600); 
}

// the loop function runs over and over again forever
void loop() {
  level = analogRead(sensorPin);
  Serial.println(level);
  if (level > thresholdLight) {
    digitalWrite(ledPinGreen, HIGH);
    digitalWrite(ledPinYel, LOW);
    digitalWrite(ledPinRed, LOW);
  } else if (level < thresholdDark) {
    digitalWrite(ledPinGreen, LOW);
    digitalWrite(ledPinYel, LOW);
    digitalWrite(ledPinRed, HIGH);
  } else {
    digitalWrite(ledPinGreen, LOW);
    digitalWrite(ledPinYel, HIGH);
    digitalWrite(ledPinRed, LOW);
  }
  delay(50);
}
