/*
  Basic ESP32 MQTT example
  This sketch demonstrates the capabilities of the pubsub library in combination
  with the ESP32 board/library.
  It connects to an MQTT server then:
  - publishes "connected to MQTT" to the topic "outTopic"
  - subscribes to the topic "inTopic", printing out messages
    it receives. NB - it assumes the received payloads are strings not binary
  - If the "ON" payload is send to the topic "inTopic" , LED will be turned on, and acknowledgement will be send to Topic "outTopic"
  - If the "OFF" payload is send to the topic "inTopic" , LED will be turned OFF, and acknowledgement will be send to Topic "outTopic"
  It will reconnect to the server if the connection is lost using a
  reconnect function.
*/

#include <WiFi.h>
#include <PubSubClient.h>
#include <Arduino.h>
#include "ESP32_Servo.h"

const char* mqtt_server = "test.mosquitto.org"; //mqtt server
const char* ssid = "WiFi_OliveNet-F7BFAC";
const char* password = "X4HmTsqD";

WiFiClient espClient;
PubSubClient client(espClient); //lib required for mqtt

Servo miServo;
const int pinServo = 18;

void callback(char* topic, byte* payload, unsigned int length) {   //callback includes topic and payload ( from which (topic) the payload is comming)
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < length; i++)
  {
    Serial.print((char)payload[i]);
  }
  if ((char)payload[0] == 'O' && (char)payload[1] == 'N') //on
  {
    Serial.println("on");
    client.publish("SALIDA/01", "SERVO turned ON");
    miServo.attach(pinServo);
    miServo.write(0); // Mover el servo a 0 grados
    delay(1000);
    miServo.detach();
    delay(1000);
    miServo.attach(pinServo); // Esperar 1 segundo
    miServo.write(180);       // Mover el servo a 90 grados
    delay(1000);              // Esperar 1 segundo
    miServo.detach();

  }
  else if ((char)payload[0] == 'O' && (char)payload[1] == 'F' && (char)payload[2] == 'F') //off
  {

  }
  Serial.println();
}
void reconnect() {
  while (!client.connected()) {
    Serial.println("Attempting MQTT connection...");
    if (client.connect("ESP32_clientID")) {
      Serial.println("connected");
      // Once connected, publish an announcement...
      client.publish("SALIDA/01", "Nodemcu connected to MQTT");
      // ... and resubscribe
      client.subscribe("Entrada/01");

    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}
void connectmqtt()
{
  client.connect("ESP32_clientID");  // ESP will connect to mqtt broker with clientID
  {
    Serial.println("connected to MQTT");
    // Once connected, publish an announcement...

    // ... and resubscribe
    client.subscribe("Entrada/01"); //topic=Demo
    client.publish("SALIDA/01",  "connected to MQTT");

    if (!client.connected())
    {
      reconnect();
    }
  }
}
void setup()
{
  Serial.begin(115200);
    Serial.print("Conectándose a ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED)
  {
    Serial.print(".");
    delay(500);
  }
  Serial.println("");
  Serial.println("Conectado a WiFi");
  Serial.println("Dirección IP: ");
  Serial.println(WiFi.localIP());

  Serial.println("connected");
  client.setServer(mqtt_server, 1883);//connecting to mqtt server
  client.setCallback(callback);
  //delay(5000);
  connectmqtt();
}


void loop()
{
  // put your main code here, to run repeatedly:
  if (!client.connected())
  {
    reconnect();
  }

  client.loop();
}


