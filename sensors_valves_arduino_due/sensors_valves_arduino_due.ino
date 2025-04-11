#define VALVE1_PIN 2
#define VALVE2_PIN 3
#define VALVE3_PIN 4
#define VALVE4_PIN 5
#define VALVE5_PIN 6

int soil_pin1 = A0;
int soil_pin2 = A1;
int soil_pin3 = A2;

unsigned long lastSensorSend = 0;
const unsigned long sensorInterval = 5000; // Envoie toutes les 5 secondes

void setup() {
  Serial.begin(9600);      
  Serial3.begin(9600);     // Communication avec ESP32

  // Config vannes
  pinMode(VALVE1_PIN, OUTPUT);
  pinMode(VALVE2_PIN, OUTPUT);
  pinMode(VALVE3_PIN, OUTPUT);
  pinMode(VALVE4_PIN, OUTPUT);
  pinMode(VALVE5_PIN, OUTPUT);

  analogWrite(VALVE1_PIN, 0);
  analogWrite(VALVE2_PIN, 0);
  analogWrite(VALVE3_PIN, 0);
  analogWrite(VALVE4_PIN, 0);
  analogWrite(VALVE5_PIN, 0);

  Serial.println(">> Arduino prêt.");
}

void loop() {
  if (Serial3.available()) {
    String command = Serial3.readStringUntil('\n');
    handleCommand(command);
  }

  unsigned long currentMillis = millis();
  if (currentMillis - lastSensorSend > sensorInterval) {
    lastSensorSend = currentMillis;
    sendSoilMoistureData();
  }
}

void sendSoilMoistureData() {
  int raw1 = analogRead(soil_pin1);
  int raw2 = analogRead(soil_pin2);
  int raw3 = analogRead(soil_pin3);

  String message = "MOISTURE:" + String(raw1) + "," + String(raw2) + "," + String(raw3);
  Serial3.println(message);
  delay(10); // Petit délai pour assurer la transmission
  Serial.println(message);

}

void handleCommand(String cmd) {
  cmd.trim();

  int valveIndex = cmd.indexOf("VALVE:");
  if (valveIndex != -1) cmd = cmd.substring(valveIndex);
  if (!cmd.startsWith("VALVE:")) return;

  int firstColon = cmd.indexOf(':');
  int secondColon = cmd.indexOf(':', firstColon + 1);

  if (firstColon == -1 || secondColon == -1) return;

  int valveNum = cmd.substring(firstColon + 1, secondColon).toInt();
  String action = cmd.substring(secondColon + 1);

  int valvePin = -1;
  switch (valveNum) {
    case 1: valvePin = VALVE1_PIN; break;
    case 2: valvePin = VALVE2_PIN; break;
    case 3: valvePin = VALVE3_PIN; break;
    case 4: valvePin = VALVE4_PIN; break;
    case 5: valvePin = VALVE5_PIN; break;
    default: 
      Serial.println("Numéro de vanne invalide: " + String(valveNum));
      return;
  }

  if (action == "OPEN") {
    analogWrite(valvePin, 255);
    Serial.println("Valve " + String(valveNum) + " ouverte on pin " + valvePin);
  } else if (action == "CLOSED") {
    analogWrite(valvePin, 0);
    Serial.println("Valve " + String(valveNum) + " fermée on pin " + valvePin);
  } else {
    Serial.println("Action invalide: " + action);
  }
}
