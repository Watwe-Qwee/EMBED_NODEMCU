#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <SoftwareSerial.h>
#define LED 16

const char* ssid     = "True-2.4G"; // WIFI
const char* password = "9980925822"; // PASSWORD
const char* mqtt_server = "broker.netpie.io";
const int mqtt_port = 1883;
const char* mqtt_Client = "f400cbf1-a9d2-48ac-87c7-d55cde31ad7e"; // CLIENT ID
const char* mqtt_username = "VuULQbP34P9C1VvEepK7mBmgXhu7mjwQ"; // TOKEN
const char* mqtt_password = "3CDE$JKR_vR)4pulYxa5seGlXZFeeEot"; // SECRET

WiFiClient espClient;
PubSubClient client(espClient);
SoftwareSerial NodeSerial(D2, D3); // RX TX
int num;
char msg[50];

void setup() {
  // put your setup code here, to run once:
  pinMode(D2, INPUT);
  pinMode(D3, OUTPUT);
  NodeSerial.begin(9600);
  Serial.begin(9600);
  pinMode(LED, OUTPUT);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    Serial.println("Try to connect");
    delay(500);
  }
  Serial.println("Connected");
  client.setServer(mqtt_server, mqtt_port);
  client.setCallback(callback);
  
}

void loop() {
  // put your main code here, to run repeatedly:
  while (!client.connected()) {
    if (client.connect(mqtt_Client, mqtt_username, mqtt_password)) {
        client.subscribe("@private/shadow/data/get/response");
        client.publish("@shadow/data/get", "");
    } else {
        delay(5000);
        return;
    }
  }
  while (NodeSerial.available() > 0) {
      char c = NodeSerial.read();
      if (c == 'H') {
        num++;
        String data = "{\"data\": {\"num_status\": \"H\", \"used\": " + String(num) + "}}";
        data.toCharArray(msg, (data.length() + 1));
        client.publish("@shadow/data/update", msg);
      }
      if (c == 'M') {
        num++;
        String data = "{\"data\": {\"num_status\": \"M\", \"used\": " + String(num) + "}}";
        data.toCharArray(msg, (data.length() + 1));
        client.publish("@shadow/data/update", msg);
      }
      if (c == 'L') {
        num++;
        String data = "{\"data\": {\"num_status\": \"L\", \"used\": " + String(num) + "}}";
        data.toCharArray(msg, (data.length() + 1));
        client.publish("@shadow/data/update", msg);
      }
  }
  client.loop();
}

void callback(char* topic, byte* payload, unsigned int length) {
    String message;
    for (int i = 0; i < length; i++) {
        message = message + (char)payload[i];
    }
    if(String(topic) == "@private/shadow/data/get/response") {
      String num_str, str;
      int idx;
      idx = message.indexOf("used");
      str = message.substring(idx+6);
      idx = str.indexOf("}");
      num_str = str.substring(0, idx);
      num = num_str.toInt();
    }
}
