#define VALVE1_PIN 2
#define VALVE2_PIN 3
#define VALVE3_PIN 4
#define VALVE4_PIN 5
#define VALVE5_PIN 6

void setup() {
  Serial.begin(9600);     // Pour debug via USB
  Serial1.begin(9600);    // Pour communication avec ESP32

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
  if (Serial1.available()) {
    String command = Serial1.readStringUntil('\n');
    Serial.println("Commande reçue : " + command);

    // Analyse et exécution de la commande
    if (command == "VALVE:1:OPEN") analogWrite(VALVE1_PIN, 255);
    else if (command == "VALVE:1:CLOSED") analogWrite(VALVE1_PIN, 0);

    else if (command == "VALVE:2:OPEN") analogWrite(VALVE2_PIN, 255);
    else if (command == "VALVE:2:CLOSED") analogWrite(VALVE2_PIN, 0);

    else if (command == "VALVE:3:OPEN") analogWrite(VALVE3_PIN, 255);
    else if (command == "VALVE:3:CLOSED") analogWrite(VALVE3_PIN, 0);

    else if (command == "VALVE:4:OPEN") analogWrite(VALVE4_PIN, 255);
    else if (command == "VALVE:4:CLOSED") analogWrite(VALVE4_PIN, 0);

    else if (command == "VALVE:5:OPEN") analogWrite(VALVE5_PIN, 255);
    else if (command == "VALVE:5:CLOSED") analogWrite(VALVE5_PIN, 0);
  }
}
