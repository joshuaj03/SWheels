#include <Arduino.h>
#include <ArduinoJson.h>

#define RPWM1 14
#define LPWM1 12
#define EN1 13

#define RPWM2 33
#define LPWM2 25
#define EN2 26

#define PWM_FREQUENCY 10000
#define PWM_RESOLUTION 8

void setup() {
  pinMode(RPWM1, OUTPUT);
  pinMode(LPWM1, OUTPUT);
  pinMode(EN1, OUTPUT);

  pinMode(RPWM2, OUTPUT);
  pinMode(LPWM2, OUTPUT);
  pinMode(EN2, OUTPUT);

  // Motor 1 Config
  ledcAttach(RPWM1, PWM_FREQUENCY, PWM_RESOLUTION);
  ledcAttach(LPWM1, PWM_FREQUENCY, PWM_RESOLUTION);
  digitalWrite(EN1, HIGH);

  ledcWrite(RPWM1, 0);
  ledcWrite(LPWM1, 0);

  // Motor 2 Config
  ledcAttach(RPWM2, PWM_FREQUENCY, PWM_RESOLUTION);
  ledcAttach(LPWM2, PWM_FREQUENCY, PWM_RESOLUTION);
  digitalWrite(EN2, HIGH);

  ledcWrite(RPWM2, 0);
  ledcWrite(LPWM2, 0);

  Serial.begin(9600);

    String direction = None;
    int distance = 0;
    int time = 0;
    int angle = 0;
    int speed = 0;

  Serial.println("F --> Front R --> Reverse R --> Right L --> Left with speed (0-255): ");
}

void loop() {
  if (Serial.available() > 0) {
    String input = Serial.readStringUntil('\n');
    Serial.println("received: " + input);
    input.trim();    

    if (input.length() > 0) { 
      char direction = input.charAt(0);
      int speed = input.substring(2).toInt();

      if (speed < 0 || speed > 255) {
        Serial.println("Invalid speed! Speed range between 0 and 255.");
        return;
      }

      if (direction == 'f') {
        Serial.print("Going front at speed: ");
        Serial.println(speed);
        ledcWrite(RPWM1, speed);
        ledcWrite(LPWM1, 0);
        ledcWrite(RPWM2, speed);
        ledcWrite(LPWM2, 0);

      } else if (direction == 'b') {
        Serial.print("Going reverse at speed: ");
        Serial.println(speed);
        ledcWrite(RPWM1, 0);
        ledcWrite(LPWM1, speed);
        ledcWrite(RPWM2, 0);
        ledcWrite(LPWM2, speed);

      } else if (direction == 's') {
        Serial.println("Stopping Motor.");
        ledcWrite(RPWM1, 0);
        ledcWrite(LPWM1, 0);
        ledcWrite(RPWM2, 0);
        ledcWrite(LPWM2, 0);

      } else if (direction == 'r') {
        Serial.print("Turning right at speed: ");
        Serial.println(speed);
        ledcWrite(RPWM1, 255);
        ledcWrite(LPWM1, 0);
        ledcWrite(RPWM2, 0);
        ledcWrite(LPWM2, 100);

      } else if (direction == 'l') {
        Serial.print("Turning left at speed: ");
        Serial.println(speed);
        ledcWrite(RPWM1, 0);
        ledcWrite(LPWM1, 100);
        ledcWrite(RPWM2, 255);
        ledcWrite(LPWM2, 0);
      } else {
        Serial.println("Invalid Input.");
      }
    } else {
      Serial.println("Not available");
    }
    Serial.println("F --> Front R --> Reverse R --> Right L --> Left with speed (0-255): ");
  }
}
