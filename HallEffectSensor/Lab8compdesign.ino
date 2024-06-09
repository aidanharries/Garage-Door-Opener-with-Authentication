// lab7 andy freshnock
#include <WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>

const char* ssid = "ece631Lab";
const char* password = "esp32!IOT!";
const char* mqtt_server = "144.126.217.239";

WiFiClient espClient;
PubSubClient client(espClient);

unsigned long lastMsg = 0;
#define MSG_BUFFER_SIZE  (50)
char msg[MSG_BUFFER_SIZE];



#define SEND_TIME 1000
#define READ_TIME 100
#define TIMECLAMP 0xFFFFFFFF
#define HALL_PIN 36
int OFFSET = 0;

#define WINDOW_SIZE 5

uint64_t CurrentMillis = 0;
uint64_t OldMillis = 0;
uint64_t OldRead = 0;
uint64_t CurrentRead = 0;
uint16_t windowIndex = 0;
uint64_t windowAvg[WINDOW_SIZE];
int64_t windowSum = 0;
uint8_t state = 0;

JsonDocument jDoc;


void setup_wifi() {

  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  randomSeed(micros());

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}


void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Create a random client ID
    String clientId = "ESP8266Client-";
    clientId += String(random(0xffff), HEX);
    // Attempt to connect
    if (client.connect(clientId.c_str())) {
      Serial.println("connected");
      // Once connected, publish an announcement...
      client.publish("outTopic", "hello world");
      // ... and resubscribe
      client.subscribe("ece631/Lab5/LED/Action/");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}


void setup() {
  //mqtt
  setup_wifi();
  client.setServer(mqtt_server, 1883);
  // put your setup code here, to run once:
  
  pinMode(HALL_PIN,OUTPUT);
  
  Serial.begin(115200);

  OFFSET += analogRead(HALL_PIN);
  delay(10);
  OFFSET += analogRead(HALL_PIN);
  delay(10);
  OFFSET += analogRead(HALL_PIN);
  delay(10);
  OFFSET += analogRead(HALL_PIN);
  OFFSET /= 4;
}

void loop() {
  // do mqtt thingys
    if (!client.connected()) {
    reconnect();
  }
  client.loop();

  //lab7 logic
  CurrentRead = millis() % 0xFFFF;
  if ( ( CurrentRead - OldRead + 0xFFFF) % 0xFFFF >= READ_TIME) {  //print every second
    OldRead = CurrentRead;
    uint64_t reading = analogRead(HALL_PIN) - OFFSET;
    windowSum -= windowAvg[windowIndex%WINDOW_SIZE];
    windowSum += reading;
    windowAvg[windowIndex%WINDOW_SIZE] = reading;
    windowIndex++;
  }

  CurrentMillis = millis() % 0xFFFF;
  if ( ( CurrentMillis - OldMillis + 0xFFFF) % 0xFFFF >= SEND_TIME) {  //print every second
    OldMillis = CurrentMillis;
    
    double magnetOutput = windowSum / (float)WINDOW_SIZE; // calc avg and inches
    Serial.println(magnetOutput);
    Serial.println(analogRead(HALL_PIN));
    jDoc["Value"] = magnetOutput;
    switch(state){
      case 0:
        if(magnetOutput > 60)state = 1;
        else if(magnetOutput < -60)state = 3;
        jDoc["Switch"] = "0";
        break;
      case 1: // north
        if(magnetOutput < 10) state = 2;
        jDoc["Switch"] = "OPEN"; 
        break;
      case 2: // north
        if(magnetOutput < -60) state = 3;
        jDoc["Switch"] = "CLOSING"; 
        break;
      case 3: // south
        if(magnetOutput > -10) state = 4;
        jDoc["Switch"] = "CLOSED"; 
        break;
      case 4: // south
        if(magnetOutput > 60) state = 1;
        jDoc["Switch"] = "OPENING"; 
        break;
      default:
        state = 0;
        break;
    }
    char output [64];
    serializeJson(jDoc, output);   
    client.publish("ece631/Lab7/Distance/SensorID/0", output);
  }

}
