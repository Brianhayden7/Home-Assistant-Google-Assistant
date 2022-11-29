/*
  Button

  Turns on and off a light emitting diode(LED) connected to digital pin 13,
  when pressing a pushbutton attached to pin 2.

  The circuit:
  - LED attached from pin 13 to ground through 220 ohm resistor
  - pushbutton attached to pin 2 from +5V
  - 10K resistor attached to pin 2 from ground

  - Note: on most Arduinos there is already an LED on the board
    attached to pin 13.

  created 2005
  by DojoDave <http://www.0j0.org>
  modified 30 Aug 2011
  by Tom Igoe

  This example code is in the public domain.

  https://www.arduino.cc/en/Tutorial/BuiltInExamples/Button
*/
#include <ESP8266WiFi.h>
#include <ArduinoMqttClient.h>

const char* wifiSSID = "Brian's Pixel"; // In order for this to work, you MUST specify the SSID for your wifi
const char* wifiPSK = "00000007"; // And the preshared key (wifi password)

// constants won't change. They're used here to set pin numbers:
const int buttonPin = D5;     // the number of the pushbutton pin
//const int ledPin =  13;      // the number of the LED pin

WiFiClient wifiClient;
MqttClient mqttClient(wifiClient);

const char broker[] = "192.168.75.68";
int port = 1883;
const char topic[]  = "/sensor/magnet";

// variables will change:
int buttonState = 0;         // variable for reading the pushbutton status
int state = 0;

int count = 0;

void setup() {
  // initialize the LED pin as an output:
  // pinMode(ledPin, OUTPUT);
  // initialize the pushbutton pin as an input:
  pinMode(buttonPin, INPUT_PULLUP);
  Serial.begin(9600);

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
}

void loop() {
  // read the state of the pushbutton value:
  buttonState = digitalRead(buttonPin);
  // call poll() regularly to allow the library to send MQTT keep alive which
  // avoids being disconnected by the broker
  mqttClient.poll();

  // check if the pushbutton is pressed. If it is, the buttonState is HIGH:
  if (buttonState == HIGH && state != 1) {
    // turn LED on:
   // digitalWrite(ledPin, HIGH);
    Serial.println("on");
    mqttClient.beginMessage(topic);
    mqttClient.print(1);
    mqttClient.endMessage();
    state = 1;
  } else if (buttonState == LOW && state != 2) {
    // turn LED off:
   // digitalWrite(ledPin, LOW);
    Serial.println("off");
    mqttClient.beginMessage(topic);
    mqttClient.print(0);
    mqttClient.endMessage();
    state = 2;
  }
  delay(500);
}
