
#include <WiFi.h>
#include <PubSubClient.h>
#include <Arduino.h>
#include "ESP32_Servo.h"

#include "esp_wifi.h"
#include "esp_sleep.h"

const char* mqtt_server = "mqtt.eclipseprojects.io"; 
const char* ssid = "";
const char* password = "";

WiFiClient espClient;
PubSubClient client(espClient); 

Servo miServo;
const int pinServo = 18; //Pin del servomotor
const int pinBoton = 4;  // Pin del botón físico


void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Mensaje recibido [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }

if (strcmp(topic, "Elevator") == 0)   //Compruebo que el topic sea Elevator
{
  if ((char)payload[0] == 'O' && (char)payload[1] == 'N') {
     client.publish("SALIDA/01", "Pulsando boton del ascensor");
    miServo.attach(pinServo);
    miServo.writeMicroseconds(750); 
    delay(1250);
    miServo.detach();

    miServo.attach(pinServo); 
    miServo.writeMicroseconds(1500);     
    delay(100);             
    miServo.detach();
  } 
}


  Serial.println();
}

void reconnect() {
  while (!client.connected()) {
    Serial.println("Intentando conectar a MQTT...");
    if (client.connect("ESP32_clientID")) {
      Serial.println("Conectado a MQTT");
      client.publish("SALIDA/01", "Conectado a MQTT");
      client.subscribe("Elevator");
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
    digitalWrite(2, LOW);  // Apaga el LED

    WiFi.disconnect();
    WiFi.begin(ssid, password);  //Intentamos reconectar con las credenciales

    unsigned long startAttemptTime = millis();
    const unsigned long timeout = 10000; // Esperamos hasta 10 segundos

    while (WiFi.status() != WL_CONNECTED && millis() - startAttemptTime < timeout) {
      delay(500);
      Serial.print(".");
    }

    if (WiFi.status() == WL_CONNECTED) {
      Serial.println("\nReconectado a WiFi");
      digitalWrite(2, HIGH);  // Enciende el LED
    } else {
      Serial.println("\nNo se pudo reconectar al WiFi");
    }
  } else {
    digitalWrite(2, HIGH); // Enciende el LED si ya está conectado
  }
}

/*
void entrarEnDeepSleep() {
  Serial.println("Inactividad detectada. Entrando en deep sleep...");

  // Configura el botón como fuente de interrupción para despertar
  esp_sleep_enable_ext0_wakeup(BOTON_WAKE, 0); // Se despierta con nivel LOW

  delay(100); // Espera final para limpieza de buffers
  esp_deep_sleep_start();
}

*/

void setup() {
  Serial.begin(115200);

  pinMode(2, OUTPUT);
   pinMode(pinBoton, INPUT_PULLUP); // Configuración del botón

  Serial.print("Conectándose a ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
  Serial.println();
  Serial.println("Conectado a WiFi");
  Serial.println("Dirección IP: ");
  Serial.println(WiFi.localIP());

  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
  delay(500);
  reconnect();

  esp_wifi_set_ps(WIFI_PS_MIN_MODEM); // ahorro de energía moderado

}

void loop() {
  checkWiFi();
  if (!client.connected()) {
    reconnect();
  }
 digitalWrite(2, HIGH);  // ENCIENDE el LED
  
  client.loop();
if (digitalRead(pinBoton) == LOW) {
    Serial.println("Botón presionado, activando servo...");
    client.publish("SALIDA/01", "Botón físico presionado");

    miServo.attach(pinServo);
    miServo.writeMicroseconds(750);
    delay(1250);
    miServo.detach();

    miServo.attach(pinServo);
    miServo.writeMicroseconds(1500);
    delay(100);
    miServo.detach();

    delay(1000); // Antirrebote simple
  }
  delay(100);
}

