#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <HardwareSerial.h>

// Configuration WiFi
const char* ssid = "iPhone 13 Pro de Ines";
const char* password = "honeyy45";  
const char* serverUrl = "http://172.20.10.3:8000/data";  // Endpoint for sensor data
const char* valveStatusUrl = "http://172.20.10.3:8000/valves";  // Endpoint for valve status

// Configuration UART pour Arduino DUE
#define RXD2 26
#define TXD2 27
HardwareSerial mySerial2(2);  // UART2

// Définition des capteurs
#define MOISTURE_SENSOR_PIN 34  // Capteur d'humidité
#define TEMPERATURE_SENSOR_PIN 35  // Capteur de température
#define LIGHT_SENSOR_PIN 32      // Capteur de lumière 
#define RAINGAUGE_SENSOR_PIN 33  // Capteur de pluie 

// Variables pour la gestion des commandes valves
unsigned long lastUpdate = 0;
const long updateInterval = 5000;  // 5 secondes

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
  Serial.begin(9600);
  mySerial2.begin(9600, SERIAL_8N1, RXD2, TXD2);

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
  unsigned long currentMillis = millis();

  // Vérifier périodiquement l'état des vannes
  if (currentMillis - lastUpdate >= updateInterval) {
    lastUpdate = currentMillis;
    checkValveCommands();
  }
  
  // Traiter les messages entrants de la DUE
  if (mySerial2.available()) {
    String message = mySerial2.readStringUntil('\n');
    Serial.println("Reçu de la DUE: " + message);
    // Ici vous pouvez traiter les messages remontant de la DUE
  }

  // Lire et envoyer les données des capteurs
  sendSensorData();
}

void checkValveCommands() {
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("Connexion WiFi perdue");
    return;
  }

  HTTPClient http;
  http.begin(valveStatusUrl);
  int httpCode = http.GET();

  if (httpCode == HTTP_CODE_OK) {
    String payload = http.getString();
    Serial.println("Payload brut : " + payload);
    DynamicJsonDocument doc(1024);
    deserializeJson(doc, payload);
    
    //JsonObject valves = doc["valve_state"];
    bool valve1 = doc["valve1"];
    bool valve2 = doc["valve2"];
    bool valve3 = doc["valve3"];
    bool valve4 = doc["valve4"];
    bool valve5 = doc["valve5"];

    if (valve1) {
      mySerial2.println("VALVE:1:OPEN");
      Serial.println("Commande envoyée à la DUE: VALVE:1:OPEN");
      delay(10);
    } else {
      mySerial2.println("VALVE:1:CLOSED");
      Serial.println("Commande envoyée à la DUE: VALVE:1:CLOSED");
      delay(10);
    }

    if (valve2) {
      mySerial2.println("VALVE:2:OPEN");
      Serial.println("Commande envoyée à la DUE: VALVE:2:OPEN");
      delay(10);
    } else {
      mySerial2.println("VALVE:2:CLOSED");
      Serial.println("Commande envoyée à la DUE: VALVE:2:CLOSED");
      delay(10);
    }

    if (valve3) {
      mySerial2.println("VALVE:3:OPEN");
      Serial.println("Commande envoyée à la DUE: VALVE:3:OPEN");
      delay(10);
    } else {
      mySerial2.println("VALVE:3:CLOSED");
      Serial.println("Commande envoyée à la DUE: VALVE:3:CLOSED");
      delay(10);
    }

    if (valve4) {
      mySerial2.println("VALVE:4:OPEN");
      Serial.println("Commande envoyée à la DUE: VALVE:4:OPEN");
      delay(10);
    } else {
      mySerial2.println("VALVE:4:CLOSED");
      Serial.println("Commande envoyée à la DUE: VALVE:4:CLOSED");
      delay(10);
    }

    if (valve5) {
      mySerial2.println("VALVE:5:OPEN");
      Serial.println("Commande envoyée à la DUE: VALVE:5:OPEN");
      delay(10);
    } else {
      mySerial2.println("VALVE:5:CLOSED");
      Serial.println("Commande envoyée à la DUE: VALVE:5:CLOSED");
      delay(10);
    }

  } else {
    Serial.printf("Erreur HTTP (%d) lors de GET /valves\n", httpCode);
  }

  http.end();
}


void sendSensorData() {
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    
    // Lecture des capteurs
    //int moistureValue = analogRead(MOISTURE_SENSOR_PIN);  // 0-4095 (sec-humide)
    //float temperatureValue = analogRead(TEMPERATURE_SENSOR_PIN) * 0.322; // Conversion en °C
    //int lightValue = analogRead(LIGHT_SENSOR_PIN);        // 0-4095 (sombre-lumineux)
    //float rainValue = tipCount * MM_PER_TIP;              // Calcul pluie (mm)
    int moistureValue = 67;
    float temperatureValue = 23;
    int lightValue = 2500;
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
}
