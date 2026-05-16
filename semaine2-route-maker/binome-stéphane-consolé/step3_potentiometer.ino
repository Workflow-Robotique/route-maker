


const int PIN_LDR   = A0;
const int PIN_POT   = A1;   // Potentiometer wiper → analog pin A1

const int PIN_RED   = 9;
const int PIN_GREEN = 10;
const int PIN_BLUE  = 11;

void setup() {
  Serial.begin(9600);
  pinMode(PIN_RED,   OUTPUT);
  pinMode(PIN_GREEN, OUTPUT);
  pinMode(PIN_BLUE,  OUTPUT);
  Serial.println("Step 3 ready — adjustable threshold");
}

void loop() {
  int lightValue = analogRead(PIN_LDR);
  int potValue   = analogRead(PIN_POT);  // Raw: 0 to 1023

  // map() converts potValue (0–1023) to a threshold between 200 and 800.
  // We avoid the extremes (0 and 1023) to prevent the system from being
  // permanently in alert or permanently impossible to trigger.
  int threshold = map(potValue, 0, 1023, 200, 800);

  Serial.print("Light: ");
  Serial.print(lightValue);
  Serial.print(" | Threshold: ");
  Serial.println(threshold);

  if (lightValue < threshold) {
    setRGB(255, 0, 0);  // Red = ALERT
  } else {
    setRGB(0, 255, 0);  // Green = Normal
  }

  delay(200);
}

void setRGB(int r, int g, int b) {
  analogWrite(PIN_RED,   r);
  analogWrite(PIN_GREEN, g);
  analogWrite(PIN_BLUE,  b);
}
