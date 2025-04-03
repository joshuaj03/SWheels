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

#define MAX_SPEED 10

String direction = "";
int distance = 0;
int move_time = 0;
int angle = 0;
int speed = 0;

void left() {
  Serial.print("Turning left at speed: ");
  Serial.println(speed);
  ledcWrite(RPWM1, 0);
  ledcWrite(LPWM1, 100);
  ledcWrite(RPWM2, 255);
  ledcWrite(LPWM2, 0);
}

void right() {
  Serial.print("Turning right at speed: ");
  Serial.println(speed);
  ledcWrite(RPWM1, 255);
  ledcWrite(LPWM1, 0);
  ledcWrite(RPWM2, 0);
  ledcWrite(LPWM2, 100);
}

void forward() {
  Serial.print("Going reverse at speed: ");
    Serial.println(speed);
    ledcWrite(RPWM1, 0);
    ledcWrite(LPWM1, speed);
    ledcWrite(RPWM2, 0);
    ledcWrite(LPWM2, speed);
}

void backward() {
  Serial.print("Going front at speed: ");
  Serial.println(speed);
  ledcWrite(RPWM1, speed);
  ledcWrite(LPWM1, 0);
  ledcWrite(RPWM2, speed);
  ledcWrite(LPWM2, 0);
}

void stop() {
  Serial.println("Stopping Motor.");
  ledcWrite(RPWM1, 0);
  ledcWrite(LPWM1, 0);
  ledcWrite(RPWM2, 0);
  ledcWrite(LPWM2, 0);
}



void movement(String direction, int speed_m) {
  speed = 127;
  if (speed_m == 10005) { // Faster
    speed += 50;
  } else if (speed_m == 10000) { // Slower
    speed -= 50;
  } else {
    speed = (speed_m * 255) / MAX_SPEED; // Converting KMPH to pwm signals. Actual speeds need to be calculated before release.
  }

  if (speed < 0 || speed > 255) {
    Serial.println("Invalid speed! Speed range between 0 and 255.");
    return;
  } 
  if (direction == "backward") {
    stop();
    backward();

  } else if (direction == "forward") {
    stop();
    forward();
  } else if (direction == "stop") {
    stop();
  } else if (direction == "right") {
    stop();
    right();

  } else if (direction == "left") {
    stop();
    left();
  } else {
    Serial.println("Invalid Input.");
  }
}

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

  Serial.begin(115200);
  distance = 0;
  move_time = 0;
  angle = 0;
  speed = 0;  

  Serial.println("F --> Front R --> Reverse R --> Right L --> Left with speed (0-255): ");
}

void loop() {
  if (Serial.available() > 0) {
    String jsonString = Serial.readStringUntil('\n');
    Serial.println("received JSON: " + jsonString);

    // Parse JSON
    StaticJsonDocument<200> doc;
    DeserializationError error = deserializeJson(doc, jsonString);

    if (!error) { 
      direction = doc["direction"].as<String>();;
      distance = doc["distance"].as<int>();
      move_time = doc["time"].as<int>();
      angle = doc["angle"].as<int>();
      int speed_m = doc["speed"].as<int>();

      if (direction == "left" || direction == "right") {
        movement(direction,speed_m);
        delay(2000);
        movement("forward",speed_m);
      } else {
          movement(direction,speed_m);
      }
    } else {
      Serial.println("Not available");
    }
    Serial.println("F --> Front R --> Reverse R --> Right L --> Left with speed (0-255): ");
  }
}
