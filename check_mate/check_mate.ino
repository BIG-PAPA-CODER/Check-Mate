#include "Adafruit_VL53L0X.h"
#include <WiFiClientSecure.h>
#include <WiFi.h>
#include <UniversalTelegramBot.h>
#include <time.h>

time_t 

char ssid[] = "XXX"; // my network SSID (name)
char pass[] = "XXX"; // my network password
WiFiClientSecure client;

// Telegram bot details
const char* botToken = "XXX";
const char* chatID = "XXX";

UniversalTelegramBot bot(botToken, client);

// address we will assign if dual sensor is present
#define LOX1_ADDRESS 0x30
#define LOX2_ADDRESS 0x31

// set the pins to shutdown
#define SHT_LOX1 5
#define SHT_LOX2 6

#define SDA_PIN 3
#define SCL_PIN 4

unsigned long lastSent = 0;
const unsigned long interval = 10000; // 10 seconds

void sendTelegramMessage(String message) {
  // if (!client.connect("api.telegram.org", 443)) {
  //   Serial.println("Connection to Telegram failed");
  //   return;
  // }

  // String url = "/bot" + String(botToken) + "/sendMessage?chat_id=" + chatID + "&text=" + message;
  // client.print(String("GET ") + url + " HTTP/1.1\r\n" +
  //              "Host: api.telegram.org\r\n" +
  //              "User-Agent: ESP32\r\n" +
  //              "Connection: close\r\n\r\n");

  // while (client.connected()) {
  //   String line = client.readStringUntil('\n');
  //   if (line == "\r") break;
  // }
  // client.stop();
  client.setCACert(TELEGRAM_CERTIFICATE_ROOT);
  bot.sendMessage(chatID, message);
}

// objects for the vl53l0x
Adafruit_VL53L0X lox1 = Adafruit_VL53L0X();
Adafruit_VL53L0X lox2 = Adafruit_VL53L0X();

// this holds the measurement
VL53L0X_RangingMeasurementData_t measure1;
VL53L0X_RangingMeasurementData_t measure2;

/*
    Reset all sensors by setting all of their XSHUT pins low for delay(10), then set all XSHUT high to bring out of reset
    Keep sensor #1 awake by keeping XSHUT pin high
    Put all other sensors into shutdown by pulling XSHUT pins low
    Initialize sensor #1 with lox.begin(new_i2c_address) Pick any number but 0x29 and it must be under 0x7F. Going with 0x30 to 0x3F is probably OK.
    Keep sensor #1 awake, and now bring sensor #2 out of reset by setting its XSHUT pin high.
    Initialize sensor #2 with lox.begin(new_i2c_address) Pick any number but 0x29 and whatever you set the first sensor to
 */
void setID() {
  // all reset
  digitalWrite(SHT_LOX1, LOW);    
  digitalWrite(SHT_LOX2, LOW);
  delay(10);
  // all unreset
  digitalWrite(SHT_LOX1, HIGH);
  digitalWrite(SHT_LOX2, HIGH);
  delay(10);

  // activating LOX1 and resetting LOX2
  digitalWrite(SHT_LOX1, HIGH);
  digitalWrite(SHT_LOX2, LOW);

  // initing LOX1
  if(!lox1.begin(LOX1_ADDRESS)) {
    Serial.println(F("Failed to boot first VL53L0X"));
    while(1);
  }
  delay(10);

  // activating LOX2
  digitalWrite(SHT_LOX2, HIGH);
  delay(10);

  //initing LOX2
  if(!lox2.begin(LOX2_ADDRESS)) {
    Serial.println(F("Failed to boot second VL53L0X"));
    while(1);
  }
}

void read_dual_sensors(int* val_1, int* val_2) {
  
  lox1.rangingTest(&measure1, false); // pass in 'true' to get debug data printout!
  lox2.rangingTest(&measure2, false); // pass in 'true' to get debug data printout!

  // print sensor one reading
  Serial.print(F("1: "));
  if(measure1.RangeStatus != 4) {     // if not out of range
    Serial.print(measure1.RangeMilliMeter);
    *val_1 = measure1.RangeMilliMeter;
  } else {
    Serial.print(F("Out of range"));
  }
  
  Serial.print(F(" "));

  // print sensor two reading
  Serial.print(F("2: "));
  if(measure2.RangeStatus != 4) {
    Serial.print(measure2.RangeMilliMeter);
    *val_2 = measure2.RangeMilliMeter;
  } else {
    Serial.print(F("Out of range"));
  }
  
  Serial.println();
}

void setup() {
  Serial.begin(115200);

  Serial.println("Attempting to connect to WPA network...");

  WiFi.mode(WIFI_STA);
  int status = WiFi.begin(ssid, pass);
  while (WiFi.status() != WL_CONNECTED) 
  {
    Serial.println("Trying to connect");
    delay(1000);
  }
  Serial.println("Connected");
  delay(3000);

  Wire.begin(SDA_PIN, SCL_PIN);
  Wire.setClock(100000);
  // wait until serial port opens for native USB devices
  while (! Serial) { delay(1); }

  pinMode(SHT_LOX1, OUTPUT);
  pinMode(SHT_LOX2, OUTPUT);

  Serial.println(F("Shutdown pins inited..."));

  digitalWrite(SHT_LOX1, LOW);
  digitalWrite(SHT_LOX2, LOW);

  Serial.println(F("Both in reset mode...(pins are low)"));
  
  
  Serial.println(F("Starting..."));
  setID();
 
}

void loop() {
  int vl1, vl2;
  
  read_dual_sensors(&vl1, &vl2);
  
  // Serial.println(vl1);
  // Serial.println(vl2);

  unsigned long now = millis();
  if (now - lastSent > interval) {
    lastSent = now;

    String msg;
    if (vl1 > 0 && vl2 > 0) {
      msg = "First VL53L0X distance: " + String(vl1) + " mm || " + "Second VL53L0X distance: " + String(vl2) + " mm.\n Time:" + ;
    } else {
      msg = "Sensor out of range";
    }

    sendTelegramMessage(msg);
    Serial.println("Message sent: " + msg);
  }

  delay(100);
}
