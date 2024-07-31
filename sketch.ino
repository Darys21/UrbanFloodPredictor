#include <Servo.h>

Servo myServo; // Créer un objet servomoteur

const int trigPin = 2; // Pin de déclenchement
const int echoPin = 3; // Pin d'écho
const int servoPin = 9; // Pin du servomoteur

void setup() {
  Serial.begin(9600); // Initialiser la communication série
  myServo.attach(servoPin); // Attacher le servomoteur
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  Serial.println("Simulation lancée!");
}

void loop() {
  long duration, distance;
  
  // Émettre un signal ultrasonique
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  
  // Lire le temps de réponse
  duration = pulseIn(echoPin, HIGH);
  
  // Calculer la distance
  distance = (duration * 0.034) / 2; // Distance en cm
  
  // Vérifier la distance et agir en conséquence
  if (distance < 5) {
    Serial.println("#1"); // Obstruction détectée
    Serial.println("Obstruction du Canal");
  } else if (distance < 20) {
    myServo.write(180); // Soulever la plateforme
    delay(1000); // Attendre que le servomoteur se déplace
    myServo.write(0); // Ramener la plateforme
    Serial.println("#0"); // Désobstruction
    Serial.println("Desobstruction du canal ");
  } else {
    Serial.println("#0"); // Pas d'obstruction
    Serial.println("Pas d'obstruction");
  }
  
  delay(5000); // Attendre avant la prochaine mesure
}