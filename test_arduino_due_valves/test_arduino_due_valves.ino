#define NUM_VALVES 5
const int valvePins[NUM_VALVES] = {2, 3, 4, 5, 6};

void setup() {
  Serial.begin(9600);       // pour afficher les logs sur PC
  Serial3.begin(9600);      // communication avec l'ESP32

  for (int i = 0; i < NUM_VALVES; i++) {
    pinMode(valvePins[i], OUTPUT);
    analogWrite(valvePins[i], 0);
  }

  Serial.println(">> Arduino Due prêt.");
}

void loop() {
  if (Serial3.available()) {
    String command = Serial3.readStringUntil('\n');
    Serial.print("Reçu : ");
    Serial.println(command);
    handleCommand(command);
  }
}

void handleCommand(String cmd) {
  cmd.trim();
  Serial.println(cmd);

  if (!cmd.startsWith("VALVE:")) return;

  int firstColon = cmd.indexOf(':');
  int secondColon = cmd.indexOf(':', firstColon + 1);
  int thirdColon = cmd.indexOf(':', secondColon + 1);

  int valveNum = cmd.substring(firstColon + 1, secondColon).toInt();
  String action = cmd.substring(secondColon + 1, (thirdColon == -1) ? cmd.length() : thirdColon);
  int duration = (thirdColon != -1) ? cmd.substring(thirdColon + 1).toInt() : 0;

  if (valveNum < 1 || valveNum > NUM_VALVES) return;
  int pin = valvePins[valveNum - 1];
  Serial.println(action);


  if (action == "OPEN") {
    analogWrite(pin, 255);
    Serial.println("Vanne " + String(valveNum) + " ouverte.");

    if (duration > 0) {
      delay(duration * 1000);
      analogWrite(pin, 0);
      Serial.println("Vanne " + String(valveNum) + " refermée (timeout).");
    }
  } else if (action == "CLOSE") {
    analogWrite(pin, 0);
    Serial.println("Vanne " + String(valveNum) + " fermée.");
  }
}
