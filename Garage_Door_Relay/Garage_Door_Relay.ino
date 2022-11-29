

#include <ESP8266WiFi.h>
#include <ArduinoMqttClient.h>

const char* wifiSSID = "Brian's Pixel"; // In order for this to work, you MUST specify the SSID for your wifi
const char* wifiPSK = "00000007"; // And the preshared key (wifi password)
const int RELAY_PIN = D1;

int mytime = 0;
int redLED = D5;
int yellowLED = D6;
int greenLED = D7;
bool isLOOP = false;
bool firstLoop = true;
bool isOpen = false;
bool pulse = false;
int state = -1;
String msg;
char letter;
String red = "red";
String yellow = "yellow";
String green = "green";
unsigned long previousMillis = 0;
const long interval = 500;
int ledState = LOW;


WiFiClient wifiClient;
MqttClient mqttClient(wifiClient);

const char broker[] = "192.168.75.68";
int port = 1883;
const char topic[]  = "/home-assistant/relay";

WiFiServer server(80); // This sets which port our server will listen on

//WiFiClient client = server.available(); // Create a new client object for available connections

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  // int myTime = 0;
  pinMode(RELAY_PIN, OUTPUT);  
  digitalWrite(RELAY_PIN, LOW);

  // ** Connect to WiFi network - Adapted from http://www.esp8266learning.com/wemos-webserver-example.php
  
  Serial.print("Connecting to "); // Display debugging connection info
  
  Serial.println(wifiSSID); // Print the configured SSID to the Serial Monitor
  
  WiFi.begin(wifiSSID, wifiPSK); // Use the provided SSID and PSK to connect
  
  while (WiFi.status() != WL_CONNECTED) { // If not connected to wifi
  
  delay(500); // Pause
  
  Serial.print("."); // Print a dot each loop while trying to connect
  
  }
  
  Serial.println("");
  
  Serial.println("WiFi connected"); // Print "connected" message to the Serial Monitor
  
  server.begin(); // Start the web server
  
  Serial.println("Server started");
  
  Serial.print("Use this URL : "); // Print the connected IP address to the Serial Monitor
  
  Serial.print("http://");
  
  Serial.print(WiFi.localIP());
  
  Serial.println("/");
  
  // ** End Adapted Code - This is the end of the code that was adapted from www.esplearning.com

  Serial.print("Attempting to connect to the MQTT broker: ");
  Serial.println(broker);

  if (!mqttClient.connect(broker, port)) {
    Serial.print("MQTT connection failed! Error code = ");
    Serial.println(mqttClient.connectError());

    while (1);
  }

  Serial.println("You're connected to the MQTT broker!");
  Serial.println();

  // set the message receive callback
  mqttClient.onMessage(onMqttMessage);

  Serial.print("Subscribing to topic: ");
  Serial.println(topic);
  Serial.println();
  
  // subscribe to a topic
  mqttClient.subscribe(topic);

  
}

void loop() {
  // put your main code here, to run repeatedly:
  //WiFiClient client = server.available(); // Create a new client object for available connections
  mqttClient.poll();

  unsigned long currentMillis = millis();
  int myTime = millis();
  if (currentMillis - previousMillis >= interval && pulse) {
    // save the last time you blinked the LED
    previousMillis = currentMillis;

    // if the LED is off turn it on and vice-versa:
    if (ledState == LOW) {
      ledState = HIGH;
    } else {
      ledState = LOW;
    }

    // set the LED with the ledState of the variable:
    digitalWrite(RELAY_PIN, ledState);
    if(ledState == LOW){
      pulse = false;
    }
  }
}

void onMqttMessage(int messageSize) {
  // we received a message, print out the topic and contents
  Serial.println("Received a message with topic '");
  Serial.print(mqttClient.messageTopic());
  Serial.print("', length ");
  Serial.print(messageSize);
  Serial.println(" bytes:");
  msg = "";
  // use the Stream interface to print the contents
  while (mqttClient.available()) {
    letter = (char)mqttClient.read();
    msg = msg + letter;  
    Serial.print(letter);
  }
  Serial.println();
  Serial.print(msg);
  Serial.println();
  pulse = true;
}
