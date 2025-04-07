#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>

// Identifiants du Wi-Fi 
const char* ssid = "iPhone 13 Pro de Ines";
const char* password = "honeyy45";  

// Adresse de ton serveur FastAPI
const char* serverUrl = "http://172.20.10.3:8000/data";

// Définition des capteurs
#define MOISTURE_SENSOR_PIN 34  // Capteur d'humidité
#define TEMPERATURE_SENSOR_PIN 35  // Capteur de température
#define LIGHT_SENSOR_PIN 32      // Capteur de lumière 
#define RAINGAUGE_SENSOR_PIN 33  // Capteur de pluie 

// Variables pour le pluviomètre
volatile unsigned int tipCount = 0;  // Compteur d'impulsions
unsigned long lastRainTime = 0;
const float MM_PER_TIP = 0.2794;     // mm par impulsion (à calibrer)

void IRAM_ATTR countTip() {
  if (millis() - lastRainTime > 100) {  // Anti-rebond
    tipCount++;
    lastRainTime = millis();
  }
}

void setup() {
    Serial.begin(115200);
    
    // Configuration du pluviomètre (interruption)
    pinMode(RAINGAUGE_SENSOR_PIN, INPUT_PULLUP);
    attachInterrupt(digitalPinToInterrupt(RAINGAUGE_SENSOR_PIN), countTip, FALLING);
    
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
        
        // Lecture des capteurs (simulés ou réels)
        //int moistureValue = analogRead(MOISTURE_SENSOR_PIN);  // 0-4095 (sec-humide)
        //float temperatureValue = analogRead(TEMPERATURE_SENSOR_PIN) * 0.322; // Conversion en °C
        //int lightValue = analogRead(LIGHT_SENSOR_PIN);        // 0-4095 (sombre-lumineux)
        int moistureValue = 67;
        float temperatureValue = 23;
        int lightValue = 2500;
        
        // Calcul pluie (mm/heure)
        //float rainValue = tipCount * MM_PER_TIP;
        float rainValue = random(0, 5);
        tipCount = 0;  // Réinitialisation après lecture
  
        // Création du JSON avec les 4 capteurs
        StaticJsonDocument<256> jsonDoc;
        JsonObject sensors = jsonDoc.createNestedObject("sensors");
        sensors["moisture"] = moistureValue;
        sensors["temperature"] = temperatureValue;
        sensors["light"] = lightValue;
        sensors["rain"] = rainValue;

        String jsonString;
        serializeJson(jsonDoc, jsonString);
        Serial.println("Données envoyées: " + jsonString);

        // Envoi de la requête HTTP POST
        http.begin(serverUrl);
        http.addHeader("Content-Type", "application/json");
        int httpResponseCode = http.POST(jsonString);

        if (httpResponseCode > 0) {
            Serial.print("Réponse HTTP: ");
            Serial.println(httpResponseCode);
        } else {
            Serial.print("⚠️ Échec de l'envoi: ");
            Serial.println(httpResponseCode);
        }
        http.end();
    } else {
        Serial.println("⚠️ Wi-Fi déconnecté !");
    }

    delay(5000);  // Envoi toutes les 5 secondes
}