import requests
import json

# Configuration de ThingsBoard
THINGSBOARD_URL = "http://thingsboard.cloud/api/v1/YOUR_DEVICE_TOKEN"
HEADERS = {'Content-Type': 'application/json'}

# Fonction pour récupérer l'état du canal
def get_channel_status():
    # Remplacez par l'URL de votre API ou méthode pour obtenir l'état du canal
    response = requests.get("http://your_api_endpoint/channel_status")
    if response.status_code == 200:
        return response.json().get("obstruction", False)
    return False

# Fonction pour récupérer les dernières mesures
def get_latest_measurements():
    response = requests.get(f"{THINGSBOARD_URL}/telemetry", headers=HEADERS)
    if response.status_code == 200:
        return response.json()
    return None

# Fonction pour prédire la pluie (exemple simple)
def will_it_rain(humidity, temperature):
    # Implémentez votre modèle de prédiction ici
    # Exemple simple : si l'humidité est supérieure à 80%, il va probablement pleuvoir
    return humidity > 80

# Évaluer les risques d'inondation
def evaluate_flood_risk(water_level, will_rain):
    NULL, LOW, MODERATE, HIGH = 0, 1, 2, 3
    flood_risk = NULL

    if will_rain:
        if water_level == 1:  # EMPTY
            flood_risk = LOW
        elif water_level == 2:  # HALF_FULL
            flood_risk = MODERATE
        elif water_level == 3:  # FULL
            flood_risk = HIGH
    else:
        if water_level == 3:  # FULL
            flood_risk = LOW
        else:
            flood_risk = NULL

    return flood_risk

# Main
if __name__ == "__main__":
    channel_obstruction = get_channel_status()
    
    if channel_obstruction:
        print("Obstruction détectée dans le canal.")
        
        # Récupérer les dernières mesures
        measurements = get_latest_measurements()
        if measurements:
            humidity = measurements.get("humidity", 0)
            temperature = measurements.get("temperature", 0)
            water_level = measurements.get("water_level", 1)  # DEFAULT to EMPTY
            
            print(f"Mesures récupérées - Humidité: {humidity}, Température: {temperature}, Niveau d'eau: {water_level}")
            
            # Prédire la pluie
            rain_prediction = will_it_rain(humidity, temperature)
            flood_risk = evaluate_flood_risk(water_level, rain_prediction)
            
            print(f"Niveau de risque d'inondation : {flood_risk}")
        else:
            print("Erreur lors de la récupération des mesures.")
    else:
        print("Aucune obstruction détectée dans le canal.")
