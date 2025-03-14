#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include "DHT.h"  // Pour le capteur DHT

#define DHTPIN 4       // Broche du capteur DHT
#define DHTTYPE DHT11  // Type de capteur (DHT11 ou DHT22)
DHT dht(DHTPIN, DHTTYPE);

// 🛜 Identifiants Wi-Fi
const char* ssid = "TON_SSID";  // Remplace avec ton réseau Wi-Fi
const char* password = "TON_MOT_DE_PASSE";

// 🌐 Adresse du serveur FastAPI (met l'IP du PC si sur le même réseau)
const char* serverUrl = "http://192.168.1.100:8000/data";  

void setup() {
    Serial.begin(115200);
    dht.begin();

    // 📡 Connexion Wi-Fi
    WiFi.begin(ssid, password);
    Serial.print("Connexion à Wi-Fi...");
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    Serial.println("\n✅ Connecté à Wi-Fi !");
    Serial.println(WiFi.localIP());  // Affiche l'IP de l'ESP32
}

void loop() {
    if (WiFi.status() == WL_CONNECTED) {
        HTTPClient http;
        
        // 🔹 Lecture des données du capteur
        float temperature = dht.readTemperature();
        float moisture = dht.readMoisture();

        if (isnan(temperature) || isnan(moisture)) {
            Serial.println("❌ Erreur de lecture du capteur !");
            return;
        }

        // 📦 Création du JSON
        StaticJsonDocument<200> jsonDoc;
        jsonDoc["temperature"] = temperature;
        jsonDoc["moisture"] = moisture;
        String jsonString;
        serializeJson(jsonDoc, jsonString);

        // 🌍 Envoi de la requête HTTP POST
        http.begin(serverUrl);
        http.addHeader("Content-Type", "application/json");
        int httpResponseCode = http.POST(jsonString);

        // 📊 Affichage de la réponse
        Serial.print("Réponse HTTP: ");
        Serial.println(httpResponseCode);
        Serial.println(http.getString());

        http.end();
    } else {
        Serial.println("⚠️ Wi-Fi déconnecté !");
    }

    delay(5000);  // Attendre 5 sec avant le prochain envoi
}
