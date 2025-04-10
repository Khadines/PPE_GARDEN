#define NUM_VALVES 5
const byte valvePins[NUM_VALVES] = {2, 3, 4, 5, 6};
const byte VALVE_OPEN = 255;
const byte VALVE_CLOSED = 0;
unsigned long valveCloseTime[NUM_VALVES] = {0};

void setup() {
  Serial.begin(9600);
  Serial2.begin(9600);
  
  for (byte i = 0; i < NUM_VALVES; i++) {
    pinMode(valvePins[i], OUTPUT);
    analogWrite(valvePins[i], VALVE_CLOSED);
  }
  Serial.println(">> Arduino Due prêt.");
}

void loop() {
  handleSerialCommands();
  checkValveTimers();
}

void handleSerialCommands() {
  if (Serial2.available()) {
    String cmd = Serial2.readStringUntil('\n');
    cmd.trim();
    parseValveCommand(cmd);
  }
}

void parseValveCommand(const String &cmd) {
  if (!cmd.startsWith("VALVE:")) return;

  byte firstColon = cmd.indexOf(':');
  byte secondColon = cmd.indexOf(':', firstColon + 1);
  
  byte valveNum = cmd.substring(firstColon + 1, secondColon).toInt();
  if (valveNum < 1 || valveNum > NUM_VALVES) return;

  String action = cmd.substring(secondColon + 1);
  action.toUpperCase();
  
  if (action == "OPEN") {
    setValve(valveNum, true);
  } 
  else if (action == "CLOSE") {
    setValve(valveNum, false);
  }
  else if (action.startsWith("OPEN:")) {
    unsigned long duration = action.substring(5).toInt();
    setTimedValve(valveNum, duration);
  }
}

void setValve(byte valveNum, bool open) {
  byte pin = valvePins[valveNum - 1];
  analogWrite(pin, open ? VALVE_OPEN : VALVE_CLOSED);
  valveCloseTime[valveNum - 1] = 0;
  Serial.print("Vanne ");
  Serial.print(valveNum);
  Serial.println(open ? " ouverte" : " fermée");
}

void setTimedValve(byte valveNum, unsigned long duration) {
  if (duration == 0) return;
  setValve(valveNum, true);
  valveCloseTime[valveNum - 1] = millis() + (duration * 1000UL);
}

void checkValveTimers() {
  unsigned long currentTime = millis();
  for (byte i = 0; i < NUM_VALVES; i++) {
    if (valveCloseTime[i] > 0 && currentTime >= valveCloseTime[i]) {
      analogWrite(valvePins[i], VALVE_CLOSED);
      valveCloseTime[i] = 0;
      Serial.print("Vanne ");
      Serial.print(i + 1);
      Serial.println(" refermée (timeout)");
    }
  }
}