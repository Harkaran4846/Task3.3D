#include <WiFiNINA.h>          // Include WiFi library for connecting to WiFi
#include <PubSubClient.h>      // Include MQTT library for message publishing

// WiFi and MQTT Broker Configuration
const char* WIFI_SSID = "Abcdf";               // WiFi network SSID
const char* WIFI_PASS = "12345678";            // WiFi network password
const char* MQTT_BROKER = "broker.emqx.io";    // MQTT broker address
const int MQTT_PORT = 1883;                    // Port for MQTT communication
const char* TOPIC_WAVE = "SIT210/wave";        // MQTT topic for "wave" messages
const char* TOPIC_PAT = "SIT210/pat";          // MQTT topic for "pat" messages

// Ultrasonic Sensor Pin Definitions
const int TRIG_PIN = 9;                        // Ultrasonic sensor trigger pin
const int ECHO_PIN = 10;                       // Ultrasonic sensor echo pin

// WiFi and MQTT Client Instances
WiFiClient netClient;                          // WiFi client for network communication
PubSubClient mqttClient(netClient);            // MQTT client for publishing messages

// Function Declarations
void initializeWiFi();                         // Function to connect to WiFi
void ensureMQTTConnection();                   // Function to connect to the MQTT server
long measureDistance();                        // Function to measure distance using ultrasonic sensor
void sendMQTTMessage(const char* topic, const char* message);  // Function to send an MQTT message
void evaluateAndSend();                        // Function to evaluate distance and send messages

void setup() {
  Serial.begin(9600);                          // Initialize serial communication for debugging
  initializeWiFi();                            // Connect to WiFi
  mqttClient.setServer(MQTT_BROKER, MQTT_PORT); // Set up the MQTT server

  pinMode(TRIG_PIN, OUTPUT);                   // Set trigger pin as output
  pinMode(ECHO_PIN, INPUT);                    // Set echo pin as input
}

void loop() {
  if (!mqttClient.connected()) {               // Ensure MQTT connection is active
    ensureMQTTConnection();                    // Reconnect if disconnected
  }
  mqttClient.loop();                           // Keep MQTT connection alive
  evaluateAndSend();                           // Measure distance and send message if conditions met
  delay(2000);                                 // Wait 2 seconds before repeating
}

// Function to connect to WiFi
void initializeWiFi() {
  delay(10);
  Serial.println();
  Serial.print("Connecting to WiFi: ");
  Serial.println(WIFI_SSID);
  WiFi.begin(WIFI_SSID, WIFI_PASS);            // Attempt to connect to WiFi
  while (WiFi.status() != WL_CONNECTED) {      // Wait until WiFi is connected
    delay(500);
    Serial.print(".");                         // Print dots as connection is attempted
  }
  Serial.println("\nWiFi connection established!");  // Confirm successful connection
}

// Function to connect to MQTT broker
void ensureMQTTConnection() {
  while (!mqttClient.connected()) {            // Check if MQTT client is connected
    Serial.print("Connecting to MQTT server...");
    if (mqttClient.connect("ESP32_Device_Publisher")) {  // Connect with client ID
      Serial.println("Connection successful!");
    } else {
      Serial.print("Connection failed, rc=");  // If connection fails, print error code
      Serial.print(mqttClient.state());
      delay(2000);                             // Retry after 2 seconds
    }
  }
}

// Function to measure distance using ultrasonic sensor
long measureDistance() {
  digitalWrite(TRIG_PIN, LOW);                 // Ensure trigger pin is low
  delayMicroseconds(2);                        // Short delay
  digitalWrite(TRIG_PIN, HIGH);                // Set trigger pin high for 10 microseconds
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);                 // Set trigger pin low again
  
  long pulseDuration = pulseIn(ECHO_PIN, HIGH);   // Measure duration of the pulse
  long distanceCm = pulseDuration * 0.034 / 2;    // Convert to distance in cm
  return distanceCm;
}

// Function to publish a message to an MQTT topic
void sendMQTTMessage(const char* topic, const char* message) {
  if (!mqttClient.connected()) {               // Ensure MQTT connection
    ensureMQTTConnection();
  }
  mqttClient.loop();                           // Maintain connection
  mqttClient.publish(topic, message);          // Publish the message
  Serial.print("Published message to ");       // Confirm message sent
  Serial.println(topic);
}

// Function to evaluate distance and send corresponding MQTT messages
void evaluateAndSend() {
  long distance = measureDistance();           // Measure the distance
  Serial.print("Measured Distance: ");
  Serial.println(distance);

  if (distance < 10) {                         // If distance is less than 10 cm
    sendMQTTMessage(TOPIC_PAT, "pat from [Harkaran]"); // Send "pat" message
  } else if (distance < 25) {                  // If distance is less than 25 cm but >= 10 cm
    sendMQTTMessage(TOPIC_WAVE, "wave from [Harkaran]"); // Send "wave" message
  }
}
