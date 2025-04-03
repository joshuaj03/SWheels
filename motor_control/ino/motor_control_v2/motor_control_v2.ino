#include <Arduino.h>
#include <ArduinoJson.h>
#include "Wire.h"
#include "I2Cdev.h"
#include "MPU6050_6Axis_MotionApps612.h"

#define LED_BLUE 2

// Motor driver section
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

// MPU Section
MPU6050 mpu68(0x68);

bool mpuDetected = false;
bool dmpReady = false;
uint8_t devStatus;
uint16_t packetSize;
uint16_t fifoCount;
uint8_t  fifoBuffer[64];

// Orientation/Motion
Quaternion q; // [w, x, y, z]
VectorFloat gravity;
float ypr[3];

// Sensor Fusion Variables
float alpha = 0.98; // Complementary filter weight
float gyroYaw = 0.0, accelYaw = 0.0, fusedYaw = 0.0;
unsigned long prevTime = 0;

// Recalibration Offsets
float yawOffset = 0.0, pitchOffset = 0.0, rollOffset = 0.0;
bool recalibrate = false;

//MPU6050 Data
float accelerationX = 0.0;
float accelerationY = 0.0;
float accelerationZ = 0.0;
float temperature = 0.0;

float velocityX = 0;
float velocityY = 0;
float distanceX = 0;
float distanceY = 0;
float lastTime = 0;

float correctedYaw = 0.0;
float correctedPitch = 0.0;
float correctedRoll = 0.0;

void update_motion(float deltaTime) {

  if (fabs(accelerationX) > 0.02) accelerationX = 0.0;
  if (fabs(accelerationY) > 0.02) accelerationY = 0.0;
  
  // Update Velocity
  velocityX += accelerationX * deltaTime;
  velocityY += accelerationY * deltaTime;
  
  // Update Distance
  distanceX += velocityX * deltaTime;
  distanceY += velocityY * deltaTime;
}

void recalibrate_sensor() {
  recalibrate = true;
}

// Motor Control Section

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
  Serial.print("Going forward at speed: ");
    Serial.println(speed);
    ledcWrite(RPWM1, 0);
    ledcWrite(LPWM1, speed);
    ledcWrite(RPWM2, 0);
    ledcWrite(LPWM2, speed);
}

void backward() {
  Serial.print("Going backward at speed: ");
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

void movement(String direction, int speed_m, int angle, int distance, int move_time) { // direction,speed_m,angle,distance,move_time
  if (speed_m == 10005) { // Faster
    if (speed <= 205) speed += 50;
  } else if (speed_m == 10000) { // Slower
    if (speed >= 50) speed -= 50;
  } else {
    speed = (speed_m * 255) / MAX_SPEED; // Converting KMPH to pwm signals. Actual speeds need to be calculated before release.
  }

  speed = constrain(speed, 0, 255); // Limit speed between the pwm range 0-255

  if (speed < 0 || speed > 255) {
    Serial.println("Invalid speed! Speed range between 0 and 255.");
    return;
  } 
  if (direction == "backward") {
    stop();
    backward();

  } else if (direction == "forward") {
    //stop();
    forward();
  } else if (direction == "stop") {
    stop();
  } else if (direction == "right") {
    stop();
    int current_angle = correctedYaw;
    int destination_angle = current_angle + angle;
    right();
    while (current_angle > destination_angle) {
      current_angle = correctedYaw;
      delay(10);
    }
    stop();
    forward();
  } else if (direction == "left") {
    stop();
    int current_angle = correctedYaw;
    int destination_angle = current_angle - angle;
    left();
    while (current_angle < destination_angle) {
      current_angle = correctedYaw;
      delay(10);
    }
    stop();
    forward();
  } else {
    Serial.println("Invalid Input.");
  }
}

void setup() {
  pinMode(LED_BLUE, OUTPUT);
  digitalWrite(LED_BLUE, LOW);

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

  // MPU initialization
  Wire.begin(4, 5); // Initialize I2C
  delay(100);
  mpu68.initialize();

  if (!mpu68.testConnection()) {
    Serial.println("MPU6050 connection failed!");
    while (1);
  }
  digitalWrite(LED_BLUE, HIGH);

  devStatus = mpu68.dmpInitialize();
  if (devStatus == 0) {
    mpu68.setDMPEnabled(true);
    dmpReady = true;
    packetSize = mpu68.dmpGetFIFOPacketSize();
  } else {
    Serial.println("DMP Initialization Failed!");
  }

  prevTime = millis();

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

      if (angle == 0) angle = 90;
      if (distance == 0) distance = -1;
      if (move_time == 0) move_time = -1;
      if (speed_m == 0) speed_m = 80;

      movement(direction,speed_m,angle,distance,move_time);
    } else {
      Serial.println("Not available");
    }
    Serial.println("F --> Front R --> Reverse R --> Right L --> Left with speed (0-255): ");
  }

  if (!dmpReady) return;

  if (mpu68.dmpGetCurrentFIFOPacket(fifoBuffer)) {
    unsigned long currentTime = millis();
    float dt = (currentTime - prevTime) / 1000.0;
    prevTime = currentTime;

    mpu68.dmpGetQuaternion(&q, fifoBuffer);
    mpu68.dmpGetGravity(&gravity, &q);
    mpu68.dmpGetYawPitchRoll(ypr, &q, &gravity);

    int16_t ax, ay, az, gx, gy, gz;
    mpu68.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);
    accelerationX = ax / 16384.0 * 9.81;
    accelerationY = ay / 16384.0 * 9.81;
    accelerationZ = az / 16384.0 * 9.81;

    float roll = ypr[2] * 180 / M_PI;
    float pitch = ypr[1] * 180 / M_PI;
    float yaw = ypr[0] * 180 / M_PI;

    gyroYaw += (gx / 131.0) * dt;
    // accelYaw = yaw;
    fusedYaw = alpha * (fusedYaw + gyroYaw * dt) + (1 - alpha) * yaw;
    gyroYaw = fusedYaw;

    if (recalibrate) {
      yawOffset = fusedYaw;
      pitchOffset = pitch;
      rollOffset = roll;
      recalibrate = false;
    }

    correctedYaw = fusedYaw - yawOffset;
    correctedPitch = pitch - pitchOffset;
    correctedRoll = roll - rollOffset;

    Serial.print("Yaw: "); Serial.print(correctedYaw);
    Serial.print("\tPitch: "); Serial.print(correctedPitch);
    Serial.print("\tRoll: "); Serial.println(correctedRoll);

    update_motion(dt);

    Serial.print("VelocityX: "); Serial.println(velocityX);
    Serial.print("VelocityY: "); Serial.println(velocityY);
    Serial.print("DistanceX: "); Serial.println(distanceX);
    Serial.print("DistanceY: "); Serial.println(distanceY);
  }  
}