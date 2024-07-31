#include <WiFi.h>
#include <PubSubClient.h>
#include <ESP32Servo.h>

// Paramètres Wi-Fi
#define SSID "Wokwi-GUEST"
#define PASS ""

// Paramètres MQTT
#define MQTT_SERVER "thingsboard.cloud"
#define MQTT_PORT 1883
#define TOKEN_TB "7ttpqfkz6ow2moq4hdpq" // Token du dispositif

WiFiClient espClient;
PubSubClient client(espClient);
Servo myServo; // Créer un objet servomoteur

const int trigPin = 2; // Pin de déclenchement
const int echoPin = 3; // Pin d'écho
const int servoPin = 9; // Pin du servomoteur

void setup() {
  Serial.begin(115200);
  myServo.attach(servoPin); // Attacher le servomoteur
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  connectToWiFi();
  client.setServer(MQTT_SERVER, MQTT_PORT);
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
    if (client.connect("ESP32Client", TOKEN_TB, NULL)) {
      client.publish("v1/devices/me/telemetry", "{\"obstruction\":true}");
    }
  } else if (distance < 20) {
    myServo.write(180); // Soulever la plateforme
    delay(1000); // Attendre que le servomoteur se déplace
    myServo.write(0); // Ramener la plateforme
    Serial.println("#0"); // Désobstruction
    Serial.println("Desobstruction du canal");
    if (client.connect("ESP32Client", TOKEN_TB, NULL)) {
      client.publish("v1/devices/me/telemetry", "{\"obstruction\":false}");
    }
  } else {
    Serial.println("#0"); // Pas d'obstruction
    Serial.println("Pas d'obstruction");
  }
  
  delay(5000); // Attendre avant la prochaine mesure
}

void connectToWiFi() {
  Serial.print("Connexion à ");
  Serial.print(SSID);
  WiFi.begin(SSID, PASS);
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }
  Serial.println(" connecté !");
}
