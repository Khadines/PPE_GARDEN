#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include "esp_wpa2.h"  

// Identifiants du Wi-Fi WPA2-Enterprise
const char* ssid = "OMNES Education";
const char* identity = "khadidiatou.ascofare@edu.ece.fr";  
const char* password = "5W2j_-n1j";

// Adresse de ton serveur FastAPI
const char* serverUrl = "http://10.5.24.231:8000/data";  

// 🌱 Définition du capteur moisture
#define MOISTURE_SENSOR_PIN 34  // Broche où est branché le capteur

void setup() {
    Serial.begin(115200);
    
    WiFi.disconnect(true);
    delay(1000);

    WiFi.mode(WIFI_STA);
    esp_wifi_sta_wpa2_ent_set_identity((uint8_t*)identity, strlen(identity));  
    esp_wifi_sta_wpa2_ent_set_username((uint8_t*)identity, strlen(identity));  
    esp_wifi_sta_wpa2_ent_set_password((uint8_t*)password, strlen(password));  
    esp_wifi_sta_wpa2_ent_enable();

    WiFi.begin(ssid);
    Serial.print("Connexion à ");
    Serial.print(ssid);

    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    
    Serial.println("\nConnecté !");
    Serial.println("Adresse IP : " + WiFi.localIP().toString());
}

void loop() {
    if (WiFi.status() == WL_CONNECTED) {
        HTTPClient http;
        
        // Lecture du capteur de moisture
        int moistureValue = analogRead(MOISTURE_SENSOR_PIN);
        
        // Création du JSON
        StaticJsonDocument<200> jsonDoc;
        jsonDoc["moisture"] = moistureValue;
        String jsonString;
        serializeJson(jsonDoc, jsonString);

        // Envoi de la requête HTTP POST
        http.begin(serverUrl);
        http.addHeader("Content-Type", "application/json");
        int httpResponseCode = http.POST(jsonString);

        // Affichage de la réponse
        Serial.print("Réponse HTTP: ");
        Serial.println(httpResponseCode);
        Serial.println(http.getString());

        http.end();
    } else {
        Serial.println("⚠️ Wi-Fi déconnecté !");
    }

    delay(5000);  // Attendre 5 sec avant le prochain envoi
}
