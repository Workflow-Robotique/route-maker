
// STEP 7 — BONUS 2 : Capteur ultrason HC-SR04
// Objectif : détecter un intrus qui s'approche à moins de 50 cm.
// Le capteur envoie une impulsion ultrasonore et mesure le temps
// qu'elle met à revenir après avoir rebondi sur un obstacle.
// Nouveau : pulseIn(), calcul de distance avec la vitesse du son.
// Broches : TRIG → pin 3, ECHO → pin 5 (imposées par le règlement).
// Test : approcher la main devant le capteur → alerte.


#include <Servo.h>

const int PIN_LDR    = A0;
const int PIN_POT    = A1;
const int PIN_RED    = 9;
const int PIN_GREEN  = 10;
const int PIN_BLUE   = 11;
const int PIN_BUZZER = 8;
const int PIN_SERVO  = 6;
const int PIN_TAPIS  = 4;

// Capteur ultrason HC-SR04
const int PIN_TRIG = 3;  // TRIG envoie l'impulsion ultrasonore
const int PIN_ECHO = 5;  // ECHO reçoit l'écho et mesure la durée

// Distance seuil en centimètres.
// En dessous de 50 cm, on considère qu'un intrus est présent.
const int DISTANCE_SEUIL = 50;

const int FREQ_ALARME  = 1000;
const int ANGLE_NORMAL = 0;
const int ANGLE_ALERTE = 90;

Servo alertServo;
bool inAlert = false;

void setup() {
  Serial.begin(9600);
  pinMode(PIN_RED,    OUTPUT);
  pinMode(PIN_GREEN,  OUTPUT);
  pinMode(PIN_BLUE,   OUTPUT);
  pinMode(PIN_BUZZER, OUTPUT);
  pinMode(PIN_TAPIS,  INPUT_PULLUP);

  // TRIG est une SORTIE : on lui envoie une impulsion pour déclencher la mesure.
  // ECHO est une ENTRÉE  : on lit la durée du retour ultrasonore.
  pinMode(PIN_TRIG, OUTPUT);
  pinMode(PIN_ECHO, INPUT);

  alertServo.attach(PIN_SERVO);
  alertServo.write(ANGLE_NORMAL);

  setEtatNormal();
  Serial.println("Step 7 pret — capteur ultrason ajoute (Bonus 2)");
}

void loop() {
  int  lightValue   = analogRead(PIN_LDR);
  int  seuil        = map(analogRead(PIN_POT), 0, 1023, 200, 800);
  bool tapisAppuye  = (digitalRead(PIN_TAPIS) == LOW);
  long distance     = lireDistance();

  Serial.print("Lumiere : "); Serial.print(lightValue);
  Serial.print(" | Seuil : "); Serial.print(seuil);
  Serial.print(" | Tapis : "); Serial.print(tapisAppuye ? "OUI" : "non");
  Serial.print(" | Distance : "); Serial.print(distance); Serial.println(" cm");

  // Trois déclencheurs indépendants en logique OU.
  bool conditionAlerte = (lightValue < seuil)
                      || tapisAppuye
                      || (distance > 0 && distance < DISTANCE_SEUIL);

  if (conditionAlerte) {
    if (!inAlert) {
      setEtatAlerte();
      inAlert = true;
      if (lightValue < seuil)                        Serial.println(">> Cause : lumiere");
      if (tapisAppuye)                               Serial.println(">> Cause : tapis");
      if (distance > 0 && distance < DISTANCE_SEUIL) Serial.println(">> Cause : ultrason");
    }
    tone(PIN_BUZZER, FREQ_ALARME);
  } else {
    if (inAlert) {
      setEtatNormal();
      inAlert = false;
    }
  }

  delay(100);
}

// FONCTION : Mesurer la distance avec le HC-SR04
// Principe :
//   1. Envoyer une impulsion TRIG de 10 microsecondes
//   2. Le capteur émet 8 ultrasons à 40kHz
//   3. pulseIn() mesure la durée de l'écho en microsecondes
//   4. Distance = (durée × vitesse du son) ÷ 2
//      La vitesse du son est 0.034 cm/µs.
//      On divise par 2 car le son fait l'aller ET le retour.

long lireDistance() {
  // S'assurer que TRIG est bas avant de déclencher
  digitalWrite(PIN_TRIG, LOW);
  delayMicroseconds(2);

  // Impulsion haute de 10µs pour déclencher la mesure
  digitalWrite(PIN_TRIG, HIGH);
  delayMicroseconds(10);
  digitalWrite(PIN_TRIG, LOW);

  // pulseIn() attend que ECHO passe à HIGH, mesure combien de
  // microsecondes il reste HIGH, puis retourne cette durée.
  // Timeout à 30000µs (~5m max) pour éviter de bloquer le programme
  // si rien n'est détecté.
  long duree = pulseIn(PIN_ECHO, HIGH, 30000);

  // Conversion durée → distance en centimètres
  long distance = duree * 0.034 / 2;

  return distance;
}

void setEtatNormal() {
  setRGB(0, 255, 0);
  noTone(PIN_BUZZER);
  alertServo.write(ANGLE_NORMAL);
}

void setEtatAlerte() {
  setRGB(255, 0, 0);
  alertServo.write(ANGLE_ALERTE);
}

void setRGB(int r, int g, int b) {
  analogWrite(PIN_RED,   r);
  analogWrite(PIN_GREEN, g);
  analogWrite(PIN_BLUE,  b);
}
