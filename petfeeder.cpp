#include <Wire.h>
#include <WiFi.h>
#include <WiFiClient.h>
#include <BlynkSimpleEsp32.h>
#include <ESP32Servo.h>

#define BLYNK_TEMPLATE_ID " "
#define BLYNK_TEMPLATE_NAME " "
#define BLYNK_AUTH_TOKEN " "
#define BLYNK_PRINT Serial
#define trigPin 4
#define echoPin 2

Servo myServo;  
char auth[] = BLYNK_AUTH_TOKEN;
char ssid[] = " ";
char pass[] = " ";

BlynkTimer timer;

long duration, distance;
bool isServoActivated = false;
unsigned long servoActivationTime = 0;

BLYNK_WRITE(V3) {
  int servoPosition = param.asInt();
  servoPosition = constrain(servoPosition, 0, 90);  // Limit the range to 0-90
  myServo.write(servoPosition);
}

void sendSensorData() {
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  
  duration = pulseIn(echoPin, HIGH);
  distance = duration / 58.2;
  String disp = String(distance);

  Serial.print("Distance: ");
  Serial.print(disp);
  Serial.println(" cm");
  
  Blynk.virtualWrite(V2, distance);

  if (distance < 10 && !isServoActivated) {
    myServo.write(90);  // Turn the servo to 90 degrees
    isServoActivated = true;
    servoActivationTime = millis();
  }

  if (isServoActivated && millis() - servoActivationTime >= 500) {
    myServo.write(0);  // Turn the servo to 0 degrees after 10 seconds
    isServoActivated = false;
  }
  if (distance > 15 ){
    //myServo.write(0);  // Turn the servo to 90 degrees
    //isServoActivated = true;
    //servoActivationTime = millis();
    Blynk.logEvent("makanan_kucing", "SUDAH HABIS");
  }
}

void setup() {
  Serial.begin(9600);

  Blynk.begin(auth, ssid, pass);

  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  myServo.attach(15);  // Attaches D15 the servo on the specified pin

  timer.setInterval(1000L, sendSensorData);
}

void loop() {
  Blynk.run();
  timer.run();
}
