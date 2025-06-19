

#include <WiFi.h>
#include <PubSubClient.h>
#include <Arduino.h>
#include "ESP32_Servo.h"

const char* mqtt_server = "mqtt.eclipseprojects.io"; 

const char* ssid = "";
const char* password = "";
WiFiClient espClient;
PubSubClient client(espClient); 

Servo miServo;
const int pinServo = 18;

void callback(char* topic, byte* payload, unsigned int length) {   
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < length; i++)
  {
    Serial.print((char)payload[i]);
  }
  if ((char)payload[0] == 'C') //on
  {
    client.publish("SALIDA/01", "Calibrando (subiendo pulsador)");
    miServo.attach(pinServo);
    miServo.writeMicroseconds(1500); 
    delay(1250);
    miServo.detach();

  }
  else if ((char)payload[0] == 'O' && (char)payload[1] == 'N') //on
  {
    client.publish("SALIDA/01", "Pulsando boton del ascensor");
    miServo.attach(pinServo);
    miServo.writeMicroseconds(750); 
    delay(1250);
    miServo.detach();


    miServo.attach(pinServo); 
    miServo.writeMicroseconds(1250);     
    delay(100);             
    miServo.detach();

  }
  else if ((char)payload[0] == 'A') 
  {
    client.publish("SALIDA/01", "Moviendo pulsador abajo del todo");
    miServo.attach(pinServo); 
    miServo.writeMicroseconds(500);     
    delay(100);              
    miServo.detach();
  }

  Serial.println();
}
void reconnect() {
  while (!client.connected()) {
    Serial.println("Intentando conectar a MQTT...");

    if (client.connect("ESP32_clientID")) {
      Serial.println("Conectado a MQTT");
      client.publish("SALIDA/01", "Conectado a MQTT");
      client.subscribe("Entrada/01");
    } else {
      Serial.print("Error de conexión MQTT, código: ");
      Serial.println(client.state());  
      Serial.println("Reintentando en 5 segundos...");
      delay(5000);
    }
  }
}

void checkWiFi() {
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("WiFi desconectado, intentando reconectar...");
    client.publish("SALIDA/01", "WiFi desconectado, intentando reconectar...");
    WiFi.disconnect();
    WiFi.reconnect();
    delay(5000);
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

  Serial.println("Conectado");
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
  delay(5000);
  reconnect();
}


void loop()
{
  checkWiFi();
  if (!client.connected())
  {
    reconnect();
  }

  client.loop();
}