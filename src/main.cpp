

#include <WiFi.h>
#include <PubSubClient.h>
#include <Arduino.h>
#include "ESP32_Servo.h"

const char* mqtt_server = "mqtt.eclipseprojects.io"; //mqtt server
//const char* ssid = "WiFi_OliveNet-F7BFAC";
//const char* password = "X4HmTsqD";
const char* ssid = "Redmi Note 13";
const char* password = "movistar98";

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
    delay(1250);
    miServo.detach();

    miServo.attach(pinServo); // Esperar 1 segundo
    miServo.write(90);       // Mover el servo a 90 grados
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
    Serial.println("Intentando conectar a MQTT...");

    if (client.connect("ESP32_clientID")) {
      Serial.println("Conectado a MQTT");
      client.publish("SALIDA/01", "Nodemcu conectado a MQTT");
      client.subscribe("Entrada/01");
    } else {
      Serial.print("❌ Error de conexión MQTT, código: ");
      Serial.println(client.state());  // Muestra el error específico
      Serial.println("Reintentando en 5 segundos...");
      delay(5000);
    }
  }
}

void checkWiFi() {
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("⚠️ WiFi desconectado, intentando reconectar...");
    client.publish("SALIDA/01", "⚠️ WiFi desconectado, intentando reconectar...");
    WiFi.disconnect();
    WiFi.reconnect();
    delay(5000);
  }
}

/*
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
*/

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
  delay(5000);
  reconnect();
}


void loop()
{
  // put your main code here, to run repeatedly:
  checkWiFi();
  if (!client.connected())
  {
    reconnect();
  }

  client.loop();
}