#define RXD2 26
#define TXD2 27

const int NUM_VALVES = 5;

bool valveStates[NUM_VALVES] = {false};
unsigned long valveTimers[NUM_VALVES] = {0};
unsigned long valveDurations[NUM_VALVES] = {0};

// 🔧 Fonction pour commander les vannes
void controlValve(int valveNum, bool state, unsigned long duration = 0) {
  if (valveNum < 1 || valveNum > NUM_VALVES) return;

  valveStates[valveNum - 1] = state;

  String command = "VALVE:" + String(valveNum) + ":" + (state ? "OPEN" : "CLOSE");

  if (state && duration > 0) {
    valveTimers[valveNum - 1] = millis();
    valveDurations[valveNum - 1] = duration * 1000;
    command += ":" + String(duration);
  } else {
    valveDurations[valveNum - 1] = 0;
  }

  Serial2.println(command);
//  Serial.print(">> Envoyé à la Due : ");
  Serial.println(command);
}

// 🔄 Variables pour le test séquentiel
int testState = 0;
unsigned long testStartTime = 0;

void setup() {
  Serial.begin(9600);
  Serial2.begin(9600, SERIAL_8N1, RXD2, TXD2);

  delay(1000);

  Serial.println("=== ESP32 prêt ===");

  // Démarrer le test automatique
  testState = 0;
  testStartTime = millis();
  controlValve(5, true); // Étape 1 : ouvre sans durée, on gère via testState
}

void loop() {
  unsigned long now = millis();

  // ⏱ Gestion de la séquence de test automatique
  switch (testState) {
    case 0: // Vanne ouverte
      if (now - testStartTime >= 10000) {
        controlValve(5, false); // Étape 2 : ferme
        testStartTime = now;
        testState = 1;
      }
      break;

    case 1: // Vanne fermée
      if (now - testStartTime >= 10000) {
        controlValve(5, true); // Étape 3 : rouvre
        testStartTime = now;
        testState = 0;
      }
      break;



  }

  // 🔁 Contrôle automatique de durée (si tu ouvres avec une durée)
  for (int i = 0; i < NUM_VALVES; i++) {
    if (valveStates[i] && valveDurations[i] > 0 && (now - valveTimers[i] >= valveDurations[i])) {
      controlValve(i + 1, false);
    }
  }
}
