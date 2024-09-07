#include <WiFiNINA.h>
#include <PubSubClient.h>

// WiFi and MQTT Configuration
const char* WIFI_SSID = "Abcdf";
const char* WIFI_PASS = "12345678";
const char* MQTT_BROKER = "broker.emqx.io";
const int MQTT_PORT = 1883;
const char* TOPIC_WAVE = "SIT210/wave";
const char* TOPIC_PAT = "SIT210/pat";

// LED Pin Definition
const int LED_PIN = 6;

// WiFi and MQTT Clients
WiFiClient espClient;
PubSubClient mqttClient(espClient);

// Function Prototypes
void initializeWiFi();
void reconnectMQTT();
void mqttCallback(char* topic, byte* payload, unsigned int length);
void blinkLED(int count, int onTime, int offTime);

void setup() {
  Serial.begin(115200);
  initializeWiFi();
  
  mqttClient.setServer(MQTT_BROKER, MQTT_PORT);
  mqttClient.setCallback(mqttCallback);
  
  pinMode(LED_PIN, OUTPUT);
}

void loop() {
  if (!mqttClient.connected()) {
    reconnectMQTT();
  }
  mqttClient.loop();
}

void initializeWiFi() {
  delay(10);
  Serial.println();
  Serial.print("Attempting to connect to WiFi network: ");
  Serial.println(WIFI_SSID);
  WiFi.begin(WIFI_SSID, WIFI_PASS);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nSuccessfully connected to WiFi!");
}

void reconnectMQTT() {
  while (!mqttClient.connected()) {
    Serial.print("Attempting MQTT connection...");
    if (mqttClient.connect("ESP32_Client")) {
      Serial.println("Connected to MQTT broker!");
      mqttClient.subscribe(TOPIC_WAVE);
      mqttClient.subscribe(TOPIC_PAT);
    } else {
      Serial.print("Connection failed, rc=");
      Serial.print(mqttClient.state());
      delay(2000);
    }
  }
}

void mqttCallback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived on topic: ");
  Serial.println(topic);
  Serial.print("Payload: ");
  for (unsigned int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();

  if (strcmp(topic, TOPIC_WAVE) == 0) {
    // Blink the LED 3 times for the wave topic
    blinkLED(3, 500, 500);
  } else if (strcmp(topic, TOPIC_PAT) == 0) {
    // Blink the LED 8 times for the pat topic
    blinkLED(8, 250, 250);
  }
}

void blinkLED(int count, int onTime, int offTime) {
  for (int i = 0; i < count; i++) {
    digitalWrite(LED_PIN, HIGH);
    delay(onTime);
    digitalWrite(LED_PIN, LOW);
    delay(offTime);
  }
}
