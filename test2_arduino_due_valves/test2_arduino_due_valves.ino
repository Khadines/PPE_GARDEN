#define VALVE1_PIN 2
#define VALVE2_PIN 3
#define VALVE3_PIN 4
#define VALVE4_PIN 5
#define VALVE5_PIN 6

void setup() {
  Serial.begin(9600);     // Pour debug via USB
  Serial3.begin(9600);    // Pour communication avec ESP32

  // Configuration des pins des électrovannes
  pinMode(VALVE1_PIN, OUTPUT);
  pinMode(VALVE2_PIN, OUTPUT);
  pinMode(VALVE3_PIN, OUTPUT);
  pinMode(VALVE4_PIN, OUTPUT);
  pinMode(VALVE5_PIN, OUTPUT);

  // Par défaut, toutes les vannes fermées
  analogWrite(VALVE1_PIN, 0);
  analogWrite(VALVE2_PIN, 0);
  analogWrite(VALVE3_PIN, 0);
  analogWrite(VALVE4_PIN, 0);
  analogWrite(VALVE5_PIN, 0);
  
  Serial.println(">> Arduino Due prêt.");
}

void loop() {
  static unsigned long lastSend = 0;
  if(millis() - lastSend > 5000) {
    Serial3.println("TEST_DUE");
    lastSend = millis();
  }

  if (Serial3.available()) {
    String command = Serial3.readStringUntil('\n');
    handleCommand(command);
  }

}

void handleCommand(String cmd) {
  cmd.trim();
  
  // Extraire seulement la partie VALVE:... si le message contient le préfixe
  int valveIndex = cmd.indexOf("VALVE:");
  if (valveIndex != -1) {
    cmd = cmd.substring(valveIndex);
  }

  // Vérifier si c'est bien une commande VALVE
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
