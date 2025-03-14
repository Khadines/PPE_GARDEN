#include <WiFi.h>
#include <HTTPClient.h>

const char* ssid = "iPhone 13 Pro de Ines";  
const char* password = "honeyy45";
const char* server_url = "http://10.5.24.231:8000/data";  

void setup() {
    Serial.begin(115200);
    WiFi.disconnect(true);
    delay(1000);
    
    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid, password);

    Serial.print("Connexion à ");
    Serial.println(ssid);

    unsigned long startAttemptTime = millis();
    while (WiFi.status() != WL_CONNECTED && millis() - startAttemptTime < 20000) {
        delay(500);
        Serial.print(".");
    }

    if (WiFi.status() == WL_CONNECTED) {
        Serial.println("\nConnecté !");
        Serial.print("📶 Adresse IP : ");
        Serial.println(WiFi.localIP());
    } else {
        Serial.println("\nÉchec de connexion !");
    }
}

void loop() {
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    http.begin(server_url);
    http.addHeader("Content-Type", "application/json");

    float temperature = random(200, 300) / 10.0;
    float moisture = random(400, 600) / 10.0;

    String jsonData = "{\"temperature\": " + String(temperature, 1) + ", \"moisture\": " + String(moisture, 1) + "}";

    Serial.println("Envoi des données : " + jsonData);
    
    int httpResponseCode = http.POST(jsonData);

    Serial.print("Réponse serveur : ");
    Serial.println(httpResponseCode);

    if (httpResponseCode > 0) {
        String response = http.getString();
        Serial.println("Réponse du serveur : " + response);
    } else {
        Serial.println("⚠️ Échec de l'envoi des données !");
    }

    http.end();
  } else {
    Serial.println("Wi-Fi déconnecté !");
  }

  delay(5000);
}
