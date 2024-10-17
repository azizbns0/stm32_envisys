#include <WiFi.h>
#include <HTTPClient.h>

// WiFi credentials
const char* ssid = "absi";
const char* password = "12459876355";
const char* serverUrl = "http://192.168.0.27:5000/data";

void setup() {
  Serial.begin(9600); // UART3 baud rate, same as STM32
  Serial.println("UART0 initialized. Waiting for data...");

  // Connect to WiFi
  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi...");
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }
  Serial.println("Connected to WiFi");
}

void loop() {
  // Check if data is available from STM32 (via GPIO 3 RX)
  if (Serial.available()) {
    // Read the data from UART0 (STM32 sends the data over GPIO 3)
    String dataFromSTM32 = Serial.readStringUntil('\n'); // Assuming STM32 sends data with newline ('\n')

    // Print the received data to Serial Monitor
    Serial.println("Received data from STM32: " + dataFromSTM32);

    // Send the data over HTTP
    if (WiFi.status() == WL_CONNECTED) {
      HTTPClient http;
      http.begin(serverUrl);
      http.addHeader("Content-Type", "application/x-www-form-urlencoded");

      // Prepare the payload
      String payload = "Data=" + dataFromSTM32; // Use 'Data' as the key for received data

      // Send the POST request
      int httpResponseCode = http.POST(payload);

      // Check the response
      if (httpResponseCode > 0) {
        String response = http.getString();
        Serial.println("HTTP Response code: " + String(httpResponseCode));
        Serial.println("HTTP Response: " + response);
      } else {
        Serial.println("Error on sending POST: " + String(httpResponseCode));
      }

      // Close the connection
      http.end();
    } else {
      Serial.println("WiFi not connected");
    }
  }

  delay(100);
}
