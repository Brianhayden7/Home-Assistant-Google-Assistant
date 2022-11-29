#include <ESP8266WiFi.h>
#include <ArduinoMqttClient.h>

const char* wifiSSID = "Brian's Pixel"; // In order for this to work, you MUST specify the SSID for your wifi
const char* wifiPSK = "00000007"; // And the preshared key (wifi password)

int mytime = 0;
int redLED = D5;
int yellowLED = D6;
int greenLED = D7;
bool isLOOP = false;
bool firstLoop = true;
bool isOpen = false;
int state = -1;
String msg;
char letter;
String red = "red";
String yellow = "yellow";
String green = "green";
unsigned long previousMillis = 0;
const long interval = 100;
int ledState = LOW;


WiFiClient wifiClient;
MqttClient mqttClient(wifiClient);

const char broker[] = "192.168.75.68";
int port = 1883;
const char topic[]  = "/sensor/#";

WiFiServer server(80); // This sets which port our server will listen on

//WiFiClient client = server.available(); // Create a new client object for available connections

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  // int myTime = 0;
  pinMode(redLED, OUTPUT);
  pinMode(yellowLED, OUTPUT);
  pinMode(greenLED, OUTPUT);
  digitalWrite(greenLED, LOW);
  digitalWrite(yellowLED, LOW);
  digitalWrite(redLED, LOW);

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
  //if(firstLoop){
  //  myTime = 0;
  //}
  if (currentMillis - previousMillis >= interval && isOpen && state == 3) {
    // save the last time you blinked the LED
    previousMillis = currentMillis;

    // if the LED is off turn it on and vice-versa:
    if (ledState == LOW) {
      ledState = HIGH;
    } else {
      ledState = LOW;
    }

    // set the LED with the ledState of the variable:
    digitalWrite(redLED, ledState);
  }

  if(state == 0 && isOpen){
    digitalWrite(greenLED, LOW);
    digitalWrite(yellowLED, LOW);
    digitalWrite(redLED, HIGH);
  }
  if(state == 1 && isOpen){
    digitalWrite(greenLED, LOW);
    digitalWrite(yellowLED, HIGH);
    digitalWrite(redLED, LOW);
  }
  if(state == 2 && isOpen){
    digitalWrite(greenLED, HIGH);
    digitalWrite(yellowLED, LOW);
    digitalWrite(redLED, LOW);
  }

  /*
  if((myTime / 2000) % 3 == 0 && isLOOP){
    // Serial.println("0");
    digitalWrite(greenLED, HIGH);
    digitalWrite(yellowLED, LOW);
    digitalWrite(redLED, LOW);
    firstLoop = false;
  }
  if((myTime / 2000) % 3 == 1 && isLOOP){
    // Serial.println("1");
    digitalWrite(greenLED, LOW);
    digitalWrite(redLED, LOW);
    digitalWrite(yellowLED, HIGH);
    firstLoop = false;
  }
  if((myTime / 2000) % 3 == 2 && isLOOP){
    // Serial.println("2");
    digitalWrite(yellowLED, LOW);
    digitalWrite(redLED, HIGH);
    digitalWrite(greenLED, LOW);
    firstLoop = false;
  }

  */
/*
  if (client) { // If a client is connected, wait until it sends some data
    //while (!client.available()) { // If the client hasn't sent info, wait for it
    //  delay(10);
    //}
    
    String request = client.readStringUntil('\r'); // read the first line of the request
    Serial.println(request); // Echo the request to the Serial Monitor for debug
    
    client.flush(); // Wait until the buffers are clear
  
    if (request.indexOf("/loop") != -1) { // If the request is for the page "/led=on"
      isLOOP = true;
      firstLoop = true;
    }
    
    if (request.indexOf("/led=OFF") != -1) { // If the request is for the page "/led=off"
      isLOOP = false;
      digitalWrite(yellowLED, LOW);
      digitalWrite(redLED, LOW);
      digitalWrite(greenLED, LOW);
    }
    if (request.indexOf("/ledGREEN=ON") != -1) { // If the request is for the page "/led=off"
      isLOOP = false;
      digitalWrite(yellowLED, LOW);
      digitalWrite(redLED, LOW);
      digitalWrite(greenLED, HIGH);
    }
    if (request.indexOf("/ledYELLOW=ON") != -1) { // If the request is for the page "/led=off"
      isLOOP = false;
      digitalWrite(yellowLED, HIGH);
      digitalWrite(redLED, LOW);
      digitalWrite(greenLED, LOW);
    }
    if (request.indexOf("/ledRED=ON") != -1) { // If the request is for the page "/led=off"
      isLOOP = false;
      digitalWrite(yellowLED, LOW);
      digitalWrite(redLED, HIGH);
      digitalWrite(greenLED, LOW);
    }
    // ** End Adapted Code - This is the end of the code that was adapted from www.esplearning.com
  
    // Return the response
  
    client.println("HTTP/1.1 200 OK");  
    client.println("Content-Type: text/html");  
    client.println("");  
    client.println("<!DOCTYPE HTML>");  
    client.println("<html>");  
    client.println("<head></head>");
    client.println("<body>");
    client.println("<h1>Stoplight Controller</h1>");
    client.println("<br>");
    client.println("<a href=\ledGREEN=ON><button>Green</button></a><br>");
    client.println("<a href=\ledYELLOW=ON><button>Yellow</button></a><br>");
    client.println("<a href=\ledRED=ON><button>Red</button></a><br>");
    client.println("<a href=\led=OFF><button>OFF</button></a><br>");
    client.println("<a href=\loop><button>Loop</button></a>");
    client.println("</body>");
    client.println("</html>");
  }
  
  //delay(100); // This introduces a little pause in each cycle. Probably helps save some power.
*/
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
  if(msg == red){
      state = 0;
      Serial.println(state);
      //digitalWrite(greenLED, LOW);
      //digitalWrite(yellowLED, LOW);
      //digitalWrite(redLED, HIGH);
    }
    else if(msg.equals(yellow)){
      state = 1;
      Serial.println(state);
      //digitalWrite(greenLED, LOW);
      //digitalWrite(yellowLED, HIGH);
      //digitalWrite(redLED, LOW);
    }
    else if(msg.equals(green)){
      state = 2;
      Serial.println(state);
      //digitalWrite(greenLED, HIGH);
      //digitalWrite(yellowLED, LOW);
      //digitalWrite(redLED, LOW);
    }
    else if(msg.equals("blink")){
      Serial.println("in blink loop");
      state = 3;
    }
    else if(msg.equals("1")) {
      Serial.println("in open state");
      isOpen = true;
    }
    else if(msg.equals("0")){
      Serial.println("in closed state");
      isOpen = false;
      digitalWrite(greenLED, LOW);
      digitalWrite(yellowLED, LOW);
      digitalWrite(redLED, LOW);
    }
}
