#include <Arduino.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_MPU6050.h>
#include <Wire.h>
#include "I2Cdev.h"
#include "MPU6050_6Axis_MotionApps612.h"

Adafruit_MPU6050 mpu;
MPU6050 mpu68;
bool mpuDetected = false;

const int LED_BUILTIN = 2;

// MPU Control/Status 
bool dmpReady = false;
uint8_t devStatus;
uint16_t packetSize;
uint16_t fifoCount;
uint8_t fifoBuffer[64];

// Orientation/Motion
Quaternion q; // [w, x, y, z]
VectorInt16 aa; // Acceleration data
VectorInt16 aaReal; // Gravity-free acceleration reading
VectorInt16 aaWorld; // World-frame acceleration. Gravity free accel with orientaiton
VectorFloat gravity; // Gravity Vector
float ypr[3]; //[yaw, pitch, roll]

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

const float movementThreshold = 3.5;
bool isMotion = false;

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

void setup() {
  #if I2CDEV_IMPLEMENTATION == I2CDEV_ARDUINO_WIRE
      Wire.begin(4,5);
      Wire.setClock(400000); // 400kHz I2C clock. Comment this line if having compilation difficulties
  #elif I2CDEV_IMPLEMENTATION == I2CDEV_BUILTIN_FASTWIRE
      Fastwire::setup(400, true);
  #endif

  Serial.begin(115200);
  while (!Serial)
    delay(10);

  pinMode(LED_BUILTIN, OUTPUT);
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
    mpuDetected = false;
  } else {
    Serial.println("MPU6050 connection successful");
    mpuDetected = true;
    digitalWrite(LED_BUILTIN, HIGH);
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
}

void loop() {
  if (!mpu68.testConnection())
  {
    Serial.println(F("MPU6050 connection failed"));
    mpuDetected = false;
  } else {
    mpuDetected = true;
  }

  // Get MPU6050 Data
  measurePosition();
  Serial.print("ypr\t");
  Serial.print(ypr[0] * 180/M_PI);
  Serial.print("\t");
  Serial.print(ypr[1] * 180/M_PI);
  Serial.print("\t");
  Serial.println(ypr[2] * 180/M_PI);
  //Serial.print("X = ");
  // Serial.print(accelerationX);
  // Serial.print(", Y = ");
  // Serial.print(accelerationY);
  // Serial.print(", Z = ");
  // Serial.println(accelerationZ);
}
