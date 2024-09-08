#include <WiFiNINA.h>
#include <PubSubClient.h>

// WiFi and MQTT Broker Configuration
const char* WIFI_SSID = "Abcdf";
const char* WIFI_PASS = "12345678";
const char* MQTT_BROKER = "broker.emqx.io";
const int MQTT_PORT = 1883;
const char* TOPIC_WAVE = "SIT210/wave";
const char* TOPIC_PAT = "SIT210/pat";

// Ultrasonic Sensor Pin Definitions
const int TRIG_PIN = 9;
const int ECHO_PIN = 10;

// WiFi and MQTT Client Instances
WiFiClient netClient;
PubSubClient mqttClient(netClient);

// Function Declarations
void initializeWiFi();
void ensureMQTTConnection();
long measureDistance();
void sendMQTTMessage(const char* topic, const char* message);
void evaluateAndSend();

void setup() {
  Serial.begin(9600);
  initializeWiFi();
  mqttClient.setServer(MQTT_BROKER, MQTT_PORT);

  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
}

void loop() {
  if (!mqttClient.connected()) {
    ensureMQTTConnection();
  }
  mqttClient.loop();
  evaluateAndSend();
  delay(2000);  // Evaluation every 2 seconds
}

void initializeWiFi() {
  delay(10);
  Serial.println();
  Serial.print("Connecting to WiFi: ");
  Serial.println(WIFI_SSID);
  WiFi.begin(WIFI_SSID, WIFI_PASS);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi connection established!");
}

void ensureMQTTConnection() {
  while (!mqttClient.connected()) {
    Serial.print("Connecting to MQTT server...");
    if (mqttClient.connect("ESP32_Device_Publisher")) {
      Serial.println("Connection successful!");
    } else {
      Serial.print("Connection failed, rc=");
      Serial.print(mqttClient.state());
      delay(2000);
    }
  }
}

long measureDistance() {
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);
  
  long pulseDuration = pulseIn(ECHO_PIN, HIGH);
  long distanceCm = pulseDuration * 0.034 / 2;
  return distanceCm;
}

void sendMQTTMessage(const char* topic, const char* message) {
  if (!mqttClient.connected()) {
    ensureMQTTConnection();
  }
  mqttClient.loop();
  mqttClient.publish(topic, message);
  Serial.print("Published message to ");
  Serial.println(topic);
}

void evaluateAndSend() {
  long distance = measureDistance();
  Serial.print("Measured Distance: ");
  Serial.println(distance);

  if (distance < 10) {  // "Pat" condition
    sendMQTTMessage(TOPIC_PAT, "pat from [Harkaran]");
  } else if (distance < 25) {  // "Wave" condition
    sendMQTTMessage(TOPIC_WAVE, "wave from [Harkaran]");
  }
}
