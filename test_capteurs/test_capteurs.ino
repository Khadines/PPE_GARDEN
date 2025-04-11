// Soil Moisture Sensor Testing Code
// Reads voltage output from the sensor and displays raw & converted values


int soil_pin = A0; // Analog pin connected to sensor's AOUT
int soil_pin2 = A1; // Analog pin connected to sensor's AOUT
int soil_pin3 = A2; // Analog pin connected to sensor's AOUT




void setup() {
  Serial.begin(9600);  // Start serial communication
  delay(1000);         // Short delay to stabilize sensor readings
}


void loop() {
  int raw_value = analogRead(soil_pin); // Read raw analog value
  float voltage = (float(raw_value) / 1023.0) * 5.0; // Convert to voltage (use 3.3V if using a 3.3V board)


  Serial.print("Raw Sensor Value: ");
  Serial.print(raw_value);
  Serial.print(" | Voltage: ");
  Serial.print(voltage);
  Serial.println(" V");

  int raw_value2 = analogRead(soil_pin2); // Read raw analog value
  float voltage2 = (float(raw_value2) / 1023.0) * 5.0; // Convert to voltage (use 3.3V if using a 3.3V board)


  Serial.print("Raw Sensor Value: ");
  Serial.print(raw_value2);
  Serial.print(" | Voltage: ");
  Serial.print(voltage2);
  Serial.println(" V");

  int raw_value3 = analogRead(soil_pin3); // Read raw analog value
  float voltage3 = (float(raw_value3) / 1023.0) * 5.0; // Convert to voltage (use 3.3V if using a 3.3V board)


  Serial.print("Raw Sensor Value: ");
  Serial.print(raw_value3);
  Serial.print(" | Voltage: ");
  Serial.print(voltage3);
  Serial.println(" V");
  Serial.println(" ------------- ");


  delay(500); // Wait before next reading
}