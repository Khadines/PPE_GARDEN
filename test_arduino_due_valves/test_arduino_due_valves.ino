#define NUM_VALVES 5
const int valvePins[NUM_VALVES] = {2, 3, 4, 5, 6};  // Ces broches supportent la PWM

void setup() {
  Serial.begin(9600);       // pour afficher les logs sur PC
  Serial2.begin(9600);      // communication avec l'ESP32

  // Initialisation des pins des vannes en sortie
  for (int i = 0; i < NUM_VALVES; i++) {
    pinMode(valvePins[i], OUTPUT);
    analogWrite(valvePins[i], 0);  // Toutes les vannes sont fermées par défaut (0 PWM)
  }

  Serial.println(">> Arduino Due prêt.");
}

void loop() {
  // Si l'Arduino reçoit des données de l'ESP32
  if (Serial2.available()) {
    String command = Serial2.readStringUntil('\n');
    Serial.print("Reçu : ");
    Serial.println(command);
    handleCommand(command);
  }
}

void handleCommand(String cmd) {
  cmd.trim();  // Retirer les espaces inutiles
  Serial.println(cmd);

  // Vérifier si la commande commence par "VALVE:"
  if (!cmd.startsWith("VALVE:")) return;

  // Extraire les différentes parties de la commande
  int firstColon = cmd.indexOf(':');
  int secondColon = cmd.indexOf(':', firstColon + 1);
  int thirdColon = cmd.indexOf(':', secondColon + 1);

  int valveNum = cmd.substring(firstColon + 1, secondColon).toInt(); // Numéro de la vanne
  String action = cmd.substring(secondColon + 1, (thirdColon == -1) ? cmd.length() : thirdColon);  // Action à effectuer
  int duration = (thirdColon != -1) ? cmd.substring(thirdColon + 1).toInt() : 0;  // Durée si spécifiée

  // Vérifier que le numéro de la vanne est valide
  if (valveNum < 1 || valveNum > NUM_VALVES) return;

  int pin = valvePins[valveNum - 1];
  Serial.println(action);

  // Effectuer l'action demandée
  if (action == "OPEN") {
    analogWrite(pin, 255);  // Ouvrir la vanne avec un signal PWM (valeur maximale)
    Serial.println("Vanne " + String(valveNum) + " ouverte.");

    // Si une durée est spécifiée, fermer la vanne après le délai
    if (duration > 0) {
      delay(duration * 1000);  // Conversion de la durée en millisecondes
      analogWrite(pin, 0);  // Fermer la vanne
      Serial.println("Vanne " + String(valveNum) + " refermée (timeout).");
    }
  } else if (action == "CLOSE") {
    analogWrite(pin, 0);  // Fermer la vanne avec un signal PWM à 0
    Serial.println("Vanne " + String(valveNum) + " fermée.");
  }
}
