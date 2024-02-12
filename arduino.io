void setup() {
  // Initialize serial communication
  Serial.begin(9600);
}

void loop() {
  // Read analog input from pin A0
  int sensorValue = analogRead(A0);

  // Print the sensor value to the serial monitor
 // Serial.print("/n");
  Serial.println(sensorValue);

  // Add a small delay to prevent flooding the serial port
  delay(1);
}
