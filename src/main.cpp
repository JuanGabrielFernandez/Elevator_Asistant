
CODIGO POR UDP
#include <micro_ros_arduino.h>
#include <ESP32_Servo.h>
#include <WiFi.h>  // Para conectarse a WiFi

#include <rcl/rcl.h>
#include <rclc/rclc.h>
#include <rclc/executor.h>
#include <std_msgs/msg/int32.h>

#define pinServo 18

rcl_subscription_t subscriber;
std_msgs__msg__Int32 msg_sub;

rclc_executor_t executor;
rclc_support_t support;
rcl_allocator_t allocator;
rcl_node_t node;

Servo miServo;

#define RCCHECK(fn) { rcl_ret_t rc = fn; if(rc != RCL_RET_OK) {while(1);}}
#define RCSOFTCHECK(fn) { rcl_ret_t rc = fn; if(rc != RCL_RET_OK) {}}

void subscription_callback(const void * msgin)
{
  const std_msgs__msg__Int32 * msg = (const std_msgs__msg__Int32 *)msgin;

  if(msg->data == 0) {
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

void setup() {
  // Configurar transporte UDPset_microros_udp_transports
  set_microros_wifi_transports((char*)"ssid", (char*)"pass", (char*)"ip", 8888u);

  delay(2000);


  allocator = rcl_get_default_allocator();
  RCCHECK(rclc_support_init(&support, 0, NULL, &allocator));
    // create node
  RCCHECK(rclc_node_init_default(&node, "micro_ros_arduino_wifi_node", "", &support));
  
  RCCHECK(rclc_subscription_init_default(
    &subscriber,
    &node,
    ROSIDL_GET_MSG_TYPE_SUPPORT(std_msgs, msg, Int32),
    "elevator"));

  RCCHECK(rclc_executor_init(&executor, &support.context, 1, &allocator));
  RCCHECK(rclc_executor_add_subscription(&executor, &subscriber, &msg_sub, &subscription_callback, ON_NEW_DATA));
}

void loop() {
  Serial.println("cargando...");
  RCCHECK(rclc_executor_spin_some(&executor, RCL_MS_TO_NS(100)));
  delay(100);
}

