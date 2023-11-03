#include <ArduinoJson.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>

const char* WIFI_SSID = "Rwanda-ltd";
const char* WIFI_PASSWORD = "VRT0931VRT";
const char* SERVER_URL = "http://192.168.112.225/inkoko/nodemcu.php";

const int BUTTON_PIN = D0;
const int PING_TRIGGER_PIN = D6;
const int PING_ECHO_PIN = D7;

WiFiClient client;
HTTPClient http;

void setup() {
  Serial.begin(115200);
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(BUTTON_PIN, INPUT);
  pinMode(PING_TRIGGER_PIN, OUTPUT);
  pinMode(PING_ECHO_PIN, INPUT);

  connectToWiFi();
}

void loop() {
  int buttonValue = digitalRead(BUTTON_PIN);

  if (buttonValue == HIGH) {
   // sendSensorData(2, "0");
     Serial.println(buttonValue);
  }
  else if (buttonValue == LOW) {
   sendSensorData(2, "1");
    Serial.println(buttonValue);
  }

  long cm = readUltrasonicSensor();
    Serial.println(cm);
  if (cm <10) {
    sendSensorData(cm, "6");
  }
  delay(1000);
}

void connectToWiFi() {
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
  }

  Serial.print("Connected to ");
  Serial.println(WIFI_SSID);
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());
}

void sendSensorData(int value1, String value2) {
  // Create a JSON object to hold the data
  StaticJsonDocument<200> jsonDoc;
  jsonDoc["sendval"] = value1;
  jsonDoc["sendval2"] = value2;
  String jsonStr;
  serializeJson(jsonDoc, jsonStr);

  http.begin(client, SERVER_URL);
  http.setTimeout(3000);
  http.addHeader("Content-Type", "application/json");

  int httpCode = http.POST(jsonStr);

  if (httpCode > 0) {
    if (httpCode == HTTP_CODE_OK || httpCode == HTTP_CODE_MOVED_PERMANENTLY) {
      String payload = http.getString();
      Serial.println("HTTP POST success: " + payload);
    } else {
      Serial.println("HTTP POST request failed with error code: " + String(httpCode));
    }
  } else {
    Serial.println("HTTP POST request failed");
  }

  http.end();
}

long readUltrasonicSensor() {
  digitalWrite(PING_TRIGGER_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(PING_TRIGGER_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(PING_TRIGGER_PIN, LOW);

  long duration = pulseIn(PING_ECHO_PIN, HIGH);
  return duration / 29 / 2; // Convert duration to centimeters
}
