void setup() {
  // Initialize serial communication at 115200 baud rate
  Serial.begin(115200);
}

void loop() {
  // Check if data is available to read from the serial port
  if (Serial.available()) {
    // Read the incoming command
    String command = Serial.readStringUntil('\n');

    // Process the command
    if (command == "GET_WIFI") {
      // For demonstration purposes, let's say this command returns the current WiFi SSID
      Serial.println("Current WiFi SSID: MyNetwork");
    } else if (command.startsWith("SET_WIFI_SSID:")) {
      // Extract the SSID from the command
      String ssid = command.substring(14); // 14 is the length of "SET_WIFI_SSID:"
      // Here, you'd typically set the WiFi SSID using the ESP32's WiFi library
      // For demonstration purposes, we'll just print it out
      Serial.println("Setting WiFi SSID to: " + ssid);
    } else {
      // If the command is not recognized, send an error message
      Serial.println("ERROR: Unknown command");
    }
  }

  // Small delay to prevent flooding the serial port
  delay(10);
}
