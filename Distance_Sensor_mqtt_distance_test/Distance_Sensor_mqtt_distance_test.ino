#include <ESP8266WiFi.h>
#include <ArduinoMqttClient.h>
#include <ArduinoJson.h>



//#include <NewPing.h>  //https://www.makerguides.com/hc-sr04-arduino-tutorial/

const char* wifiSSID = "Brian's Pixel"; // In order for this to work, you MUST specify the SSID for your wifi
const char* wifiPSK = "00000007"; // And the preshared key (wifi password)

// Define pins and max distance:
int trigPin = D1;
int echoPin = D2;
//#define MAX_DISTANCE 350 // Maximum distance we want to ping for (in centimeters). Maximum sensor distance is rated at 400-500cm.

//NewPing sonar(trigPin, echoPin, MAX_DISTANCE); // NewPing setup of pins and maximum distance.
long duration;
int distance;

int state = 0;

WiFiClient wifiClient;
MqttClient mqttClient(wifiClient);

const char broker[] = "192.168.75.68";
int port = 1883;
const char topic[]  = "/sensor/distance";

//Last 3 readings for averages
int dis1 = 0;
int dis2 = 0;
int dis3 = 0;
int avgDis = 0;

int    HTTP_PORT   = 80;
String HTTP_METHOD = "GET"; // or "POST"
char   HOST_NAME[] = "192.168.34.141"; // hostname of web server:


WiFiClient client;


void setup() {
  

  Serial.begin(9600); // Open serial monitor at 9600 baud to see ping results.
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  delay(1000);

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
  // put your main code here, to run repeatedly:
  delay(50); // Wait 50ms between pings (about 20 pings/sec). 29ms should be the shortest delay between pings.
  //distance = sonar.ping_in();

  mqttClient.poll();

  digitalWrite(trigPin, LOW);
  delayMicroseconds(5);

  // Trigger the sensor by setting the trigPin high for 10 microseconds:
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  // Read the echoPin, pulseIn() returns the duration (length of the pulse) in microseconds:
  duration = pulseIn(echoPin, HIGH);
  // Calculate the distance:
  distance = duration * 0.034 / 2;
  
  Serial.print("Distance = ");
  Serial.print(distance); // Distance will be 0 when out of set max range.
  Serial.println(" cm");

  if(distance > 150){
    distance = dis1;
  }
  dis3 = dis2;
  dis2 = dis1;
  dis1 = distance;

  avgDis = (dis1 + dis2 + dis3) / 3;
  Serial.print(avgDis);
  Serial.println(" cm");

/*
  if(avgDis >= 13){
    Serial.println("in green if");
    if(client.connect(HOST_NAME, HTTP_PORT)) {
      // if connected:
      Serial.println("Connected to server");
      client.println(HTTP_METHOD + " " + "/ledGREEN=ON" + " HTTP/1.1");
      client.println("Host: " + String(HOST_NAME));
      client.println("Connection: close");
      client.println(); // end HTTP header
    }
  }
  else if(avgDis < 13 && avgDis > 4){
    Serial.println("in yellow if");
    if(client.connect(HOST_NAME, HTTP_PORT)) {
      // if connected:
      Serial.println("Connected to server");
      client.println(HTTP_METHOD + " " + "/ledYELLOW=ON" + " HTTP/1.1");
      client.println("Host: " + String(HOST_NAME));
      client.println("Connection: close");
      client.println(); // end HTTP header
    }
  }
  else if(avgDis < 4){
    Serial.println("in red if");
    if(client.connect(HOST_NAME, HTTP_PORT)) {
      // if connected:
      Serial.println("Connected to server");
      client.println(HTTP_METHOD + " " + "/ledRED=ON" + " HTTP/1.1");
      client.println("Host: " + String(HOST_NAME));
      client.println("Connection: close");
      client.println(); // end HTTP header
    }
  }
  
  if(avgDis >= 15 && state != 1){
    Serial.println("in green if");
    mqttClient.beginMessage(topic);
    mqttClient.print("green");
    mqttClient.endMessage();
    state = 1;
  }
  else if(avgDis < 15 && avgDis > 8 && state != 2){
    Serial.println("in yellow if");
    mqttClient.beginMessage(topic);
    mqttClient.print("yellow");
    mqttClient.endMessage();
    state = 2;
  }
  else if(avgDis < 8 && avgDis > 4 && state != 3){
    Serial.println("in red if");
    mqttClient.beginMessage(topic);
    mqttClient.print("red");
    mqttClient.endMessage();
    state = 3;    
  }
  else if(avgDis < 4 && state != 4){
    Serial.println("in red blink if");
    mqttClient.beginMessage(topic);
    mqttClient.print("blink");
    mqttClient.endMessage();  
    state = 4;  
  }
  */
  Serial.println("sending json");
  StaticJsonDocument<200> doc;
  doc["distance"] = (String)avgDis;
  serializeJsonPretty(doc, Serial);
  Serial.println("");
  char data[200];
  serializeJson(doc, data);
  mqttClient.beginMessage(topic);
  mqttClient.print(data);
  mqttClient.endMessage();
  delay(200);

}
