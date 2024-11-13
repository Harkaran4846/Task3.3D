#include <WiFiNINA.h>         // Library for WiFi functionality
#include <PubSubClient.h>     // Library for MQTT functionality

// WiFi and MQTT Configuration
const char* WIFI_SSID = "Abcdf";             // WiFi network SSID
const char* WIFI_PASS = "12345678";          // WiFi network password
const char* MQTT_BROKER = "broker.emqx.io";  // MQTT broker address
const int MQTT_PORT = 1883;                  // Port for MQTT communication
const char* TOPIC_WAVE = "SIT210/wave";      // MQTT topic for "wave" messages
const char* TOPIC_PAT = "SIT210/pat";        // MQTT topic for "pat" messages

// LED Pin Definition
const int LED_PIN = 8;                       // Pin where the LED is connected

// WiFi and MQTT Clients
WiFiClient espClient;                        // WiFi client for network communication
PubSubClient mqttClient(espClient);          // MQTT client for message handling

// Function Prototypes
void initializeWiFi();                       // Function to connect to WiFi
void reconnectMQTT();                        // Function to connect to MQTT broker
void mqttCallback(char* topic, byte* payload, unsigned int length);  // Function for handling incoming MQTT messages
void blinkLED(int count, int onTime, int offTime);  // Function to blink LED a specified number of times

void setup() {
  Serial.begin(9600);                        // Initialize serial for debugging
  initializeWiFi();                          // Connect to WiFi

  mqttClient.setServer(MQTT_BROKER, MQTT_PORT);  // Set up MQTT broker details
  mqttClient.setCallback(mqttCallback);          // Set up callback for incoming messages
  
  pinMode(LED_PIN, OUTPUT);                  // Set LED pin as output
}

void loop() {
  if (!mqttClient.connected()) {             // Check if MQTT is connected
    reconnectMQTT();                         // Reconnect if disconnected
  }
  mqttClient.loop();                         // Process incoming messages and keep connection alive
}

// Function to connect to WiFi
void initializeWiFi() {
  delay(10);
  Serial.println();
  Serial.print("Attempting to connect to WiFi network: ");
  Serial.println(WIFI_SSID);
  WiFi.begin(WIFI_SSID, WIFI_PASS);          // Begin WiFi connection attempt
  while (WiFi.status() != WL_CONNECTED) {    // Wait until connected
    delay(500);
    Serial.print(".");                       // Print dots while waiting
  }
  Serial.println("\nSuccessfully connected to WiFi!");  // Confirm successful connection
}

// Function to reconnect to MQTT broker if connection is lost
void reconnectMQTT() {
  while (!mqttClient.connected()) {          // Check if MQTT client is connected
    Serial.print("Attempting MQTT connection...");
    if (mqttClient.connect("ESP32_Client")) {  // Attempt to connect with client ID
      Serial.println("Connected to MQTT broker!");
      mqttClient.subscribe(TOPIC_WAVE);       // Subscribe to "wave" topic
      mqttClient.subscribe(TOPIC_PAT);        // Subscribe to "pat" topic
    } else {
      Serial.print("Connection failed, rc=");  // If connection fails, print error code
      Serial.print(mqttClient.state());
      delay(2000);                           // Retry after 2 seconds
    }
  }
}

// Callback function for handling incoming MQTT messages
void mqttCallback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived on topic: ");
  Serial.println(topic);
  Serial.print("Payload: ");
  for (unsigned int i = 0; i < length; i++) {  // Print the payload received
    Serial.print((char)payload[i]);
  }
  Serial.println();

  if (strcmp(topic, TOPIC_WAVE) == 0) {       // Check if the message is from the "wave" topic
    blinkLED(3, 500, 500);                    // Blink LED 3 times if "wave" message received
  } else if (strcmp(topic, TOPIC_PAT) == 0) { // Check if the message is from the "pat" topic
    blinkLED(7, 250, 250);                    // Blink LED 7 times if "pat" message received
  }
}

// Function to blink LED a specified number of times with custom on/off times
void blinkLED(int count, int onTime, int offTime) {
  for (int i = 0; i < count; i++) {           // Loop for the number of blinks
    digitalWrite(LED_PIN, HIGH);              // Turn LED on
    delay(onTime);                            // Wait for specified on time
    digitalWrite(LED_PIN, LOW);               // Turn LED off
    delay(offTime);                           // Wait for specified off time
  }
}
