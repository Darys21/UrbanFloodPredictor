#include <WiFi.h>
#include <PubSubClient.h>
#include <DHT.h>

// Paramètres Wi-Fi
#define SSID "Wokwi-GUEST"
#define PASS ""

// Paramètres MQTT
#define MQTT_SERVER "thingsboard.cloud"
#define MQTT_PORT 1883
#define TOKEN_TB "f4z0rbksk2bqqhamvskz"

// Configuration du capteur DHT
#define DHTPIN 23     // Broche à laquelle le capteur DHT est connecté
#define DHTTYPE DHT22 // Type de capteur DHT (DHT22)

DHT dht(DHTPIN, DHTTYPE);
WiFiClient espClient;
PubSubClient client(espClient);

const int waterLevelPin = 34; // Broche analogique pour le capteur de niveau d'eau

// Configuration du HC-SR04
const int trigPin = 25; // Broche de déclenchement
const int echoPin = 26; // Broche d'écho

void setup() {
  Serial.begin(115200);
  dht.begin(); // Initialiser le capteur DHT

  // Connexion au Wi-Fi
  connectToWiFi();

  // Configuration du client MQTT
  client.setServer(MQTT_SERVER, MQTT_PORT);

  // Initialiser les broches du HC-SR04
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  // Lire les données du capteur DHT
  float humidity = dht.readHumidity();
  float temperature = dht.readTemperature();
  
  // Lire le niveau d'eau
  int waterLevel = analogRead(waterLevelPin);
  int waterLevelState = map(waterLevel, 0, 4095, 1, 3); // 1: EMPTY, 2: HALF_FULL, 3: FULL
  
  // Lire la distance du HC-SR04
  long duration, distance;
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  
  duration = pulseIn(echoPin, HIGH);
  distance = (duration * 0.034) / 2; // Calculer la distance en cm

  // Créer le payload JSON pour ThingsBoard
  String payload = String("{\"humidity\":") + humidity + ",\"temperature\":" + temperature + ",\"water_level\":" + waterLevelState + ",\"distance\":" + distance + "}";

  // Publier les données sur ThingsBoard
  client.publish("v1/devices/me/telemetry", payload.c_str());

  Serial.print("Humidité : ");
  Serial.print(humidity);
  Serial.print(", Température : ");
  Serial.print(temperature);
  Serial.print(", Niveau d'eau : ");
  Serial.print(waterLevelState);
  Serial.print(", Distance : ");
  Serial.println(distance);

  delay(5000); // Attendre 5 secondes avant la prochaine mesure
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

void reconnect() {
  while (!client.connected()) {
    Serial.print("Tentative de connexion à MQTT...");
    if (client.connect("ESP32Client", TOKEN_TB, NULL)) {
      Serial.println("connecté !");
    } else {
      Serial.print("Échec de la connexion, code d'erreur : ");
      Serial.print(client.state());
      delay(2000);
    }
  }
}
