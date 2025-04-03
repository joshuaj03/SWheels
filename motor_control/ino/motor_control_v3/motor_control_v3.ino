#include <Arduino.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_MPU6050.h>
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
Adafruit_MPU6050 mpu;
bool mpuDetected = false;

// MPU Control/Status 
bool dmpReady = false;
uint8_t devStatus;
uint16_t packetSize;
uint16_t fifoCount;
uint8_t  fifoBuffer[64];

// Orientation/Motion
Quaternion q; // [w, x, y, z]
VectorFloat gravity;
float ypr[3];

// MPU6050 Data
float accelerationX = 0.0;
float accelerationY = 0.0;
float accelerationZ = 0.0;
float rotationX = 0.0;
float rotationY = 0.0;
float rotationZ = 0.0;
float temperature = 0.0;

// Initial Data
float initialX = 0.0;
float initialY = 0.0;
float initialZ = 0.0;
float initialGX = 0.0;
float initialGY = 0.0;
float initialGZ = 0.0;
float initialTemp = 0.0;

float velocityX = 0;
float velocityY = 0;
float distanceX = 0;
float distanceY = 0;
float lastTime = 0;

const float movementThreshold = 3.5;
bool isMotion = false;
bool recalibrate = false;
unsigned long prevTime = 0;

void measurePosition() {
  sensors_event_t a, g, temp;
  mpu.getEvent(&a, &g, &temp);
  if (!dmpReady) return;
  
  if (mpu68.dmpGetCurrentFIFOPacket(fifoBuffer)) {
  mpu68.dmpGetQuaternion(&q, fifoBuffer);
  mpu68.dmpGetGravity(&gravity, &q);
  accelerationX = a.acceleration.x;
  accelerationY = a.acceleration.y;
  accelerationZ = a.acceleration.z - 9.8;

  // Measure Readable Yaw Pitch Roll 
  mpu68.dmpGetYawPitchRoll(ypr, &q, &gravity);
  rotationX = ypr[2] * 180; // Roll 
  rotationY = ypr[1] * 180; // Pitch
  rotationZ = ypr[0] * 180; // Yaw

  // temperature = mpu.getTemperature();
  temperature = temp.temperature;
  }
}

void update_motion(float deltaTime) {

  if (fabs(accelerationX) < 0.02) accelerationX = 0.0;
  if (fabs(accelerationY) < 0.02) accelerationY = 0.0;
  
  // Update Velocity
  velocityX += accelerationX * deltaTime;
  velocityY += accelerationY * deltaTime;

  // Apply damping (simulating friction or air resistance
  float damping = 0.95; // Adjust this value (0.90 - 0.99 for fine-tuning)
  velocityX *= damping;
  velocityY *= damping;
  
  // Update Distance
  distanceX += velocityX * deltaTime;
  distanceY += velocityY * deltaTime;
}

void recalibrate_sensor() {
  recalibrate = true;
}

// Motor Control Section

void right() {
  Serial.print("Turning left at speed: ");
  Serial.println(speed);
  ledcWrite(RPWM1, 0);
  ledcWrite(LPWM1, 50);
  ledcWrite(RPWM2, 100);
  ledcWrite(LPWM2, 0);
}

void left() {
  Serial.print("Turning right at speed: ");
  Serial.println(speed);
  ledcWrite(RPWM1, 100);
  ledcWrite(LPWM1, 0);
  ledcWrite(RPWM2, 0);
  ledcWrite(LPWM2, 50);
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

void movement(String direction, int speed_m, int angle, int distance, int move_time, int rotationZ) { // direction,speed_m,angle,distance,move_time
  if (speed_m == 10005) { // Faster
    if (speed <= 205) speed += 50;
  } else if (speed_m == 10000) { // Slower
    if (speed >= 50) speed -= 50;
  } else {
    speed = (speed_m * 255) / MAX_SPEED; // Converting KMPH to pwm signals. Actual speeds need to be calculated before release.
  }

  //speed = constrain(speed, 0, 255); // Limit speed between the pwm range 0-255
  if (direction == "stop") {
    stop();
  }
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
  } else if (direction == "right") {
    stop();
    int current_angle = rotationZ;
    int destination_angle = current_angle + angle;
    right();
    while (current_angle < destination_angle) {
      delay(10);
      measurePosition();
      current_angle = ypr[0] * 180 / M_PI;
      Serial.println(current_angle);
    }
    stop();
    forward();
  } else if (direction == "left") {
    stop();
    int current_angle = rotationZ;
    int destination_angle = current_angle - angle;
    left();
    while (current_angle > destination_angle - 2) {
      delay(10);
      measurePosition();
      current_angle = ypr[0] * 180 / M_PI;
      Serial.println(current_angle);
    }
    stop();
    delay(100);
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
  #if I2CDEV_IMPLEMENTATION == I2CDEV_ARDUINO_WIRE
      Wire.begin(4,5);
      Wire.setClock(400000); // 400kHz I2C clock. Comment this line if having compilation difficulties
  #elif I2CDEV_IMPLEMENTATION == I2CDEV_BUILTIN_FASTWIRE
      Fastwire::setup(400, true);
  #endif
  delay(100);

  measurePosition();
    initialX = accelerationX;
    initialY = accelerationY;
    initialZ = accelerationZ;
    initialGX = rotationX;
    initialGY = rotationY;
    initialGZ = rotationZ;
    initialTemp = temperature;
    accelerationX = 0.0;
    accelerationY = 0.0;
    accelerationZ = 0.0;

    rotationX = 0.0;
    rotationY = 0.0;
    rotationZ = 0.0;
    Serial.println();
    Serial.print("InitalX = ");
    Serial.print(initialX);
    Serial.print(", InitalY = ");
    Serial.print(initialY);
    Serial.print(", InitalZ = ");
    Serial.println(initialZ);

    // Initialize device
  mpu.begin();
  Serial.println(F("Initializing I2C devices..."));
  mpu68.initialize();
  // Verify Connection
  Serial.println(F("Checking MPU6050 connections..."));
  Serial.println(mpu68.testConnection()); 
   if (!mpu68.testConnection())
  {
    Serial.println(F("MPU6050 connection failed"));
    mpu68.initialize();
    mpuDetected = false;
  } else {
    Serial.println("MPU6050 connection successful");
    mpuDetected = true;
    digitalWrite(LED_BLUE, HIGH);
  }

  // load and configure
  Serial.println("Initializing DMP...");
  devStatus = mpu68.dmpInitialize();

  //Calibrate MPU
  mpu68.setXAccelOffset(-1508);
  mpu68.setYAccelOffset(-889);
  mpu68.setZAccelOffset(1288);

  mpu68.setXGyroOffset(203);
  mpu68.setYGyroOffset(93);
  mpu68.setZGyroOffset(5);

  if(devStatus == 0) {
    mpu68.CalibrateAccel(6);
    mpu68.CalibrateGyro(6);
    mpu68.PrintActiveOffsets();
    Serial.println(F("Enabling DMP..."));
    mpu68.setDMPEnabled(true);
    dmpReady = true;
    packetSize = mpu68.dmpGetFIFOPacketSize(); //get expected dmp packet size for later comparison
  } else {
    Serial.println(F("DMP Initialization failed(code "));
    Serial.println(devStatus); // 1 = Initial memory load failed, 2 = DMP configuration failed
    mpu68.dmpInitialize();
    Serial.println(F(")"));
  }
    delay(500);
  // Current Position
  measurePosition();
  initialX = accelerationX;
  initialY = accelerationY;
  initialZ = accelerationZ;
  initialGX = rotationX;
  initialGY = rotationY;
  initialGZ = rotationZ;
  initialTemp = temperature;
  Serial.println();
  Serial.print("InitalX = ");
  Serial.print(initialX);
  Serial.print(", InitalY = ");
  Serial.print(initialY);
  Serial.print(", InitalZ = ");
  Serial.println(initialZ);

  prevTime = millis();

  Serial.println("F --> Front R --> Reverse R --> Right L --> Left with speed (0-255): ");
}

void loop() {
  if (!mpu68.testConnection())
  {
    Serial.println(F("MPU6050 connection failed"));
    setup();
    mpuDetected = false;
  } else {
    mpuDetected = true;
  }

  unsigned long currentTime = millis();
  float dt = (currentTime - prevTime) / 1000.0;
  prevTime = currentTime;

  measurePosition();
  float roll = ypr[2] * 180 / M_PI;
  float pitch = ypr[1] * 180 / M_PI;
  float yaw = ypr[0] * 180 / M_PI;

  if (mpuDetected == true) {
    Serial.print("Yaw: "); Serial.println(yaw);
    // Serial.print("\tPitch: "); Serial.print(pitch);
    // Serial.print("\tRoll: "); Serial.println(roll);

    update_motion(dt);

    // Serial.print("VelocityX: "); Serial.println(velocityX);
    // Serial.print("VelocityY: "); Serial.println(velocityY);
    // Serial.print("DistanceX: "); Serial.println(distanceX);
    // Serial.print("DistanceY: "); Serial.println(distanceY);  
  }

  if (Serial.available() > 0) {
    String jsonString = Serial.readStringUntil('\n');
    Serial.println("received JSON: " + jsonString);

    // Parse JSON
    StaticJsonDocument<200> doc;
    DeserializationError error = deserializeJson(doc, jsonString);

    if (!error) { 
      direction = doc["direction"].as<String>();
      distance = doc["distance"].as<int>();
      move_time = doc["time"].as<int>();
      angle = doc["angle"].as<int>();
      int speed_m = doc["speed"].as<int>();

      if (angle == 0) angle = 90;
      if (distance == 0) distance = -1;
      if (move_time == 0) move_time = -1;
      if (speed_m == 0) speed = 80;

      movement(direction,speed_m,angle,distance,move_time,yaw);
    } else {
      Serial.println("Not available");
    }
    Serial.println("F --> Front R --> Reverse R --> Right L --> Left with speed (0-255): ");
  }
}