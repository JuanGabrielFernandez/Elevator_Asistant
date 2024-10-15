#include <Arduino.h>
#include "ESP32_Servo.h"
#include <WiFi.h>
#include "BluetoothSerial.h"

String device_name = "ESP32-BT-Slave";
// Check if Bluetooth is available
#if !defined(CONFIG_BT_ENABLED) || !defined(CONFIG_BLUEDROID_ENABLED)
#error Bluetooth is not enabled! Please run `make menuconfig` to and enable it
#endif

// Check Serial Port Profile
#if !defined(CONFIG_BT_SPP_ENABLED)
#error Serial Port Profile for Bluetooth is not available or not enabled. It is only available for the ESP32 chip.
#endif

BluetoothSerial SerialBT;

Servo miServo;

const int pinServo = 18;

void setup()
{
  Serial.begin(115200);
  SerialBT.begin(device_name); // Bluetooth device name
  // SerialBT.deleteAllBondedDevices(); // Uncomment this to delete paired devices; Must be called after begin
  Serial.printf("The device with name \"%s\" is started.\nNow you can pair it with Bluetooth!\n", device_name.c_str());
  miServo.attach(pinServo);
}

void loop()
{
  if (Serial.available())
  {
    SerialBT.write(Serial.read());
  }
  if (SerialBT.available())
  {
    char data = ((byte)SerialBT.read());
    switch (data)
    {
    case 'O':
      miServo.attach(pinServo);
      miServo.write(0); // Mover el servo a 0 grados
      delay(1000);
      miServo.detach();
      delay(1000);
      miServo.attach(pinServo); // Esperar 1 segundo
      miServo.write(180);       // Mover el servo a 90 grados
      delay(1000);              // Esperar 1 segundo
      miServo.detach();
      break;

    default:
      break;
    }
    Serial.println(data);
  }

  delay(20);
  // put your main code here, to run repeatedly:ç
}
