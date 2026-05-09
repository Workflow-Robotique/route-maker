
#include <LiquidCrystal_I2C.h>   // Library for I2C LCD screen
#include <Wire.h>                // I2C communication library
#include <Servo.h>               // Servo motor library

// Create LCD object
LiquidCrystal_I2C lcd(0x27, 16, 2);

// ---------------- SENSORS ----------------

// LDR pin
int ldr = A0;

// Variable storing LDR value
int ldrVal;

// Potentiometer pin
int potentiometer = A1;

// Variables used for light threshold
int limit;
int Limit;

// Ultrasonic sensor pins
int trig = 6;
int echo = 7;

// ---------------- LEDS ----------------

// Red LED pin
int rgb_red = 3;

// Green LED pin
int rgb_green = 4;

// Radar indicator LED
int led_radar = 2;

// ---------------- BUZZERS ----------------

// Main intrusion buzzer
int buzzer = 10;

// Second buzzer for light warning
int buzzer2 = 13;

// ---------------- SERVOS ----------------

// Door servo motor
Servo servoDoor;

// Radar servo motor
Servo servoRadar;

// ---------------- RADAR BUTTON ----------------

// Radar ON/OFF button
int bpRadar = 12;

// Current button state
int etatbp;

// Previous button state
int mem = HIGH;

// Radar enabled by default
bool radarEnabled = true;

// ---------------- RADAR ----------------

// Current radar position
int radarPos = 90;

// Radar direction
// 1 = right
// -1 = left
int radarDir = 1;

// ---------------- BUZZER 2 ----------------

// Stores previous buzzer time
unsigned long lastBuzz = 0;

// Stores buzzer ON/OFF state
bool buzzState = false;

// ---------------- SETUP ----------------
void setup() {

  // Start serial communication
  Serial.begin(9600);

  // Initialize LCD
  lcd.init();

  // Turn LCD backlight ON
  lcd.backlight();

  // Attach door servo
  servoDoor.attach(9);

  // Attach radar servo
  servoRadar.attach(8);

  // Close the door initially
  servoDoor.write(0);

  // Center radar initially
  servoRadar.write(90);

  // Configure LEDs as outputs
  pinMode(rgb_red, OUTPUT);
  pinMode(rgb_green, OUTPUT);
  pinMode(led_radar, OUTPUT);

  // Configure buzzers as outputs
  pinMode(buzzer, OUTPUT);
  pinMode(buzzer2, OUTPUT);

  // Configure ultrasonic pins
  pinMode(trig, OUTPUT);
  pinMode(echo, INPUT);

  // Configure button with internal pullup resistor
  pinMode(bpRadar, INPUT_PULLUP);
}

// ---------------- LOOP ----------------
void loop() {

  // Check if lighting condition is normal
  bool lightOK = normalLighting();

  // Check if intrusion is detected
  bool intrus = intrusion();

  // ---------------- Radar button ----------------

  // Read radar button state
  etatbp = digitalRead(bpRadar);

  // Detect button press
  if (etatbp == LOW && mem == HIGH) {

    // Debounce delay
    delay(30);

    // Confirm button press
    if (digitalRead(bpRadar) == LOW) {

      // Toggle radar state
      radarEnabled = !radarEnabled;
    }
  }

  // Save current button state
  mem = etatbp;

  // Print LDR value in serial monitor
  Serial.println(ldrVal);

  // ---------------- RADAR ----------------

  // If radar is enabled
  if (radarEnabled) {

    // Move radar
    radarSweep(intrus);
  }

  // If radar is disabled
  else {

    // Turn radar LED OFF
    digitalWrite(led_radar, LOW);

    // Center radar servo
    servoRadar.write(90);
  }

  // ---------------- LIGHTING ----------------

  // If light is normal
  if (lightOK) {

    // Turn green LED ON
    digitalWrite(rgb_green, HIGH);

    // Turn red LED OFF
    digitalWrite(rgb_red, LOW);
  }

  // If environment is dark
  else {

    // Turn green LED OFF
    digitalWrite(rgb_green, LOW);

    // Turn red LED ON
    digitalWrite(rgb_red, HIGH);
  }

  // ---------------- INTRUSION ----------------

  // If intrusion detected and radar enabled
  if (intrus && radarEnabled) {

    // Turn radar LED ON
    digitalWrite(led_radar, HIGH);

    // Open the door
    servoDoor.write(90);

    // Activate alarm buzzer
    tone(buzzer, 1500);
  }

  // Otherwise
  else {

    // Turn radar LED OFF
    digitalWrite(led_radar, LOW);

    // Stop alarm buzzer
    noTone(buzzer);
  }

  // ---------------- BUZZER 2 (millis) ----------------

  // If red LED is ON
  if (digitalRead(rgb_red) == HIGH) {

    // Current time
    unsigned long now = millis();

    // Every 300 milliseconds
    if (now - lastBuzz >= 300) {

      // Update previous time
      lastBuzz = now;

      // Toggle buzzer state
      buzzState = !buzzState;

      // If state is ON
      if (buzzState)

        // Play sound
        tone(buzzer2, 2000);

      // Otherwise
      else

        // Stop sound
        noTone(buzzer2);
    }
  }

  // If red LED OFF
  else {

    // Stop second buzzer
    noTone(buzzer2);
  }
}

// ---------------- LDR ----------------

// Function checking lighting condition
bool normalLighting() {

  // Read LDR value
  ldrVal = analogRead(ldr);

  // Read potentiometer value
  limit = analogRead(potentiometer);

  // Map potentiometer range
  Limit = map(limit, 0, 1023, 50, 1000);

  // Return true if light level is acceptable
  return ldrVal <= Limit;
}

// ---------------- ULTRASONIC SENSOR ----------------

// Function measuring distance
float readDistance() {

  // Send LOW pulse
  digitalWrite(trig, LOW);
  delayMicroseconds(5);

  // Send HIGH pulse
  digitalWrite(trig, HIGH);
  delayMicroseconds(10);

  // Return to LOW
  digitalWrite(trig, LOW);

  // Measure echo duration
  float duration = pulseIn(echo, HIGH, 30000);

  // If timeout occurs
  if (duration == 0) return -1;

  // Convert time into distance
  return 0.034 * duration / 2;
}

// Intrusion detection function
bool intrusion() {

  // Measure distance
  float d = readDistance();

  // Return true if object is close
  return (d > 0 && d <= 20);
}

// ---------------- RADAR ----------------

// Radar sweeping function
void radarSweep(bool intrus) {

  // If intrusion detected
  if (intrus) {

    // Keep radar still
    servoRadar.write(radarPos);

    return;
  }

  // Move radar to current position
  servoRadar.write(radarPos);

  // Change radar position
  radarPos += radarDir;

  // Right limit reached
  if (radarPos >= 150)
    radarDir = -1;

  // Left limit reached
  if (radarPos <= 30)
    radarDir = 1;

  // Control radar speed
  delay(15);
}

