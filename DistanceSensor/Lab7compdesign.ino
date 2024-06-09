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


#define PWM_FREQ 16
#define PWM_CHANNEL 0
#define PWM_RES 8
#define PWM_DUTY 1
#define TRIGPIN 23
#define ECHOPIN 22
#define SEND_TIME 5000
#define TIMECLAMP 0xFFFFFFFF

#define WINDOW_SIZE 5

uint64_t CurrentMillis = 0;
uint64_t OldMillis = 0;
uint64_t OldMikes = 0;
uint64_t CurrentMikes = 0;
uint16_t windowIndex = 0;
uint64_t windowAvg[WINDOW_SIZE];
int64_t windowSum = 0;

JsonDocument jDoc;

void EchoInterrupt(){
  //do things
  uint64_t now = micros();
  int trigger = digitalRead(ECHOPIN);
  if(trigger == 1 ){
    OldMikes = now %TIMECLAMP;
  }else{ // trigger = 0
    CurrentMikes = now % TIMECLAMP;
    uint64_t echoTime = ( CurrentMikes - OldMikes + TIMECLAMP) % TIMECLAMP; //
    windowSum -= windowAvg[windowIndex]; // subtract old value
    windowSum += echoTime; // add new value
    windowAvg[windowIndex] = echoTime;  // add new value to array
    windowIndex= (windowIndex+1) % WINDOW_SIZE;
  }
  
}

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
  ledcSetup(PWM_CHANNEL, PWM_FREQ, PWM_RES);
  ledcAttachPin(TRIGPIN, PWM_CHANNEL);
  ledcWrite(PWM_CHANNEL, PWM_DUTY);
  attachInterrupt(digitalPinToInterrupt(ECHOPIN), EchoInterrupt, CHANGE);
  Serial.begin(115200);
}

void loop() {
  // do mqtt thingys
    if (!client.connected()) {
    reconnect();
  }
  client.loop();

  //lab7 logic

  CurrentMillis = millis() % 0xFFFF;
  if ( ( CurrentMillis - OldMillis + 0xFFFF) % 0xFFFF >= SEND_TIME) {  //print every second
    OldMillis = CurrentMillis;
    double distanceOutput = (windowSum / (float)WINDOW_SIZE) * 0.0135039; // calc avg and inches
    Serial.println(distanceOutput);
    jDoc["Distance"] = distanceOutput;
    jDoc["Unit"] = "Inches";
    char output [64];
    serializeJson(jDoc, output);   
    client.publish("ece631/Lab7/Distance/SensorID/0", output);
  }

}
