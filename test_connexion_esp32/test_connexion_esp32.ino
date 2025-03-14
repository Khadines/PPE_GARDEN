#include <WiFi.h>
#include <HTTPClient.h>
#include "esp_wpa2.h"  

const char* ssid = "OMNES Education";  
const char* identity = "khadidiatou.ascofare@edu.ece.fr";  
const char* password = "5W2j_-n1j";
const char* server_url = "http://10.5.24.231:8000/data";  

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
    Serial.print("Adresse IP : ");
    Serial.println(WiFi.localIP());
}

void loop() {
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    http.begin(server_url);
    http.addHeader("Content-Type", "application/json");  // JSON

    //  Simulation des données (remplace avec des lectures de capteur)
    float temperature = random(20, 30);
    float moisture = random(40, 60);

    //  Création du JSON
    String jsonData = "{\"temperature\": " + String(temperature) + ", \"moisture\": " + String(moisture) + "}";

    //  Envoi de la requête HTTP POST
    int httpResponseCode = http.POST(jsonData);

    Serial.print("Réponse serveur : ");
    Serial.println(httpResponseCode);

    http.end();  // Fermer la connexion
  } else {
    Serial.println("Wi-Fi déconnecté !");
  }

  delay(5000);  // Attendre 5 secondes avant le prochain envoi
}
