#include <WiFi.h>
char ssid[] = "Xiaomi 11 Lite 5G NE"; // my network SSID (name)
char pass[] = "12345678989"; // my network password
WiFiClient client;

void setup() {
  Serial.begin(115200);
  delay(1000);
  Serial.println("Attempting to connect to WPA network...");

  int status = WiFi.begin(ssid, pass);
  while (WiFi.status() != WL_CONNECTED) 
  {
    Serial.println("Trying to connect");
    delay(1000);
  }
  Serial.println("Connected");
}

void loop() {}