void setup() {
  pinMode(8, OUTPUT);  // Ustaw pin D2 jako wyjście
}

void loop() {
  digitalWrite(8, HIGH); // Włącz LED
  delay(1000);            // Czekaj 0.5 sekundy
  digitalWrite(8, LOW);  // Wyłącz LED
  delay(200);            // Czekaj 0.5 sekundy
}