#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>

// Identifiants du Wi-Fi 
const char* ssid = "iPhone 13 Pro de Ines";
const char* password = "code";  

// Adresse de ton serveur FastAPI
const char* serverUrl = "http://172.20.10.3:8000/data";

// Définition des capteurs
#define MOISTURE_SENSOR_PIN 34  // Capteur d'humidité
#define TEMPERATURE_SENSOR_PIN 35  // Capteur de température

void setup() {
    Serial.begin(115200);
    
    WiFi.begin(ssid, password);
    Serial.print("Connexion à ");
    Serial.print(ssid);

    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    
    Serial.println("\nConnecté !");
    Serial.print("Adresse IP : ");
    Serial.println(WiFi.localIP());
}

void loop() {
    if (WiFi.status() == WL_CONNECTED) {
        HTTPClient http;
        
        // Lecture des capteurs
        //int moistureValue = analogRead(MOISTURE_SENSOR_PIN);
        //int temperatureValue = analogRead(TEMPERATURE_SENSOR_PIN); 
        int moistureValue = 500;  
        float temperatureValue = 25.0; 
        float temperature = (temperatureValue / 4095.0) * 100.0; // Conversion

        // Création du JSON
        StaticJsonDocument<200> jsonDoc;
        jsonDoc["moisture"] = moistureValue;
        jsonDoc["temperature"] = temperature;

        String jsonString;
        serializeJson(jsonDoc, jsonString);

        // Envoi de la requête HTTP POST
        http.begin(serverUrl);
        http.addHeader("Content-Type", "application/json");
        int httpResponseCode = http.POST(jsonString);

        // Affichage de la réponse serveur
        Serial.print("Réponse HTTP: ");
        Serial.println(httpResponseCode);
        if (httpResponseCode > 0) {
            Serial.println(" Réponse du serveur : " + http.getString());
        } else {
            Serial.println("⚠️ Échec de l'envoi des données !");
        }

        http.end();
    } else {
        Serial.println("⚠️ Wi-Fi déconnecté !");
    }

    delay(5000);  // Attendre 5 sec avant le prochain envoi
}
