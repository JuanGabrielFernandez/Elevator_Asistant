#include <WiFi.h>
#include <PubSubClient.h>
#include <Arduino.h>
#include "ESP32_Servo.h"

WiFiClient esp32Client;
PubSubClient mqttClient(esp32Client);

const char *ssid = "WiFi_OliveNet-F7BFAC";
const char *password = "X4HmTsqD";

char *server = "test.mosquitto.org";
int port = 1883;

Servo miServo;

const int pinServo = 18;

int var = 0;
char datos[40];
String resultS = "";

void wifiInit()
{
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
}

void callback(char *topic, byte *payload, unsigned int length)
{
  Serial.print("Mensaje recibido [");
  Serial.print(topic);
  Serial.print("] ");

  char payload_string[length + 1];

  int resultI;

  memcpy(payload_string, payload, length);
  payload_string[length] = '\0';
  resultI = atoi(payload_string);

  var = resultI;

  resultS = "";

  for (int i = 0; i < length; i++)
  {
    resultS = resultS + (char)payload[i];
  }
  Serial.println();
}

void reconnect()
{
  while (!mqttClient.connected())
  {
    Serial.print("Intentando conectarse MQTT...");

    if (mqttClient.connect("arduinoClient"))
    {
      Serial.println("Conectado");

      mqttClient.subscribe("Entrada/01");
    }
    else
    {
      Serial.print("Fallo, rc=");
      Serial.print(mqttClient.state());
      Serial.println(" intentar de nuevo en 5 segundos");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void setup()
{

  Serial.begin(115200);
  delay(10);
  wifiInit();
  mqttClient.setServer(server, port);
  mqttClient.setCallback(callback);
}

void loop()
{
  if (!mqttClient.connected())
  {
    reconnect();
  }
  mqttClient.loop();

  

  if (var == 5)
  {
    Serial.print("Activo servo \n");
    Serial.print("String: ");
    Serial.println(resultS);

    miServo.attach(pinServo);
    miServo.write(0); // Mover el servo a 0 grados
    delay(1000);
    miServo.detach();
    delay(1000);
    miServo.attach(pinServo); // Esperar 1 segundo
    miServo.write(180);       // Mover el servo a 90 grados
    delay(1000);              // Esperar 1 segundo
    miServo.detach();

    var = 0;// Inicializo la variable que envie por mqtt para que no entre al if todo el rato
  }
  
  delay(5000);
}
