
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

// Pin de botón externo que despertará al ESP32 desde deep sleep
const gpio_num_t BOTON_WAKE = GPIO_NUM_4; 
unsigned long tiempoUltimoMensaje = 0;
const unsigned long TIEMPO_INACTIVIDAD_MS = 10000;

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Mensaje recibido [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }

  tiempoUltimoMensaje = millis(); // Establece a la varible en que tiempo desde que lleva encendido el esp recibio un mensaje

  if ((char)payload[0] == 'C') {
    client.publish("SALIDA/01", "Calibrando (subiendo pulsador)");
    miServo.attach(pinServo);
    miServo.writeMicroseconds(1750); 
    delay(1250);
    miServo.detach();
  } 
  else if ((char)payload[0] == 'O' && (char)payload[1] == 'N') {
    client.publish("SALIDA/01", "Pulsando boton del ascensor");
    miServo.attach(pinServo);
    miServo.writeMicroseconds(500); 
    delay(1250);
    miServo.detach();

    miServo.attach(pinServo); 
    miServo.writeMicroseconds(1500);     
    delay(100);             
    miServo.detach();
  } 
  else if ((char)payload[0] == 'A') {
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

void entrarEnDeepSleep() {
  Serial.println("Inactividad detectada. Entrando en deep sleep...");

  // Configura el botón como fuente de interrupción para despertar
  esp_sleep_enable_ext0_wakeup(BOTON_WAKE, 0); // Se despierta con nivel LOW

  delay(100); // Espera final para limpieza de buffers
  esp_deep_sleep_start();
}

void setup() {
  Serial.begin(115200);

  pinMode(2, OUTPUT);
  pinMode(BOTON_WAKE, INPUT_PULLUP); // Botón con resistencia pull-up interna

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

  tiempoUltimoMensaje = millis(); // Iniciar temporizador
}

void loop() {
  checkWiFi();
  if (!client.connected()) {
    reconnect();
  }

   digitalWrite(2, HIGH);  //Dejo el led de la placa encendido para saber cuando NO esta en deep sleep

  client.loop();

  // Si pasan 10 segundos sin recibir mensajes, entrar en deep sleep
  if (millis() - tiempoUltimoMensaje > TIEMPO_INACTIVIDAD_MS) {
    entrarEnDeepSleep();
  }

  delay(100);
}
