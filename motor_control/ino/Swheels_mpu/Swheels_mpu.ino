#include <Arduino.h>
#include "Wire.h"
#include "I2Cdev.h"
#include "MPU6050_6Axis_MotionApps612.h"

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

float* update_velocity(float deltaTime) {
  static float velocity[2];
  velocityX += accelerationX * deltaTime;
  velocityY += accelerationY * deltaTime;
  velocity[0] = velocityX;
  velocity[1] = velocityY;
  return velocity;
}

float* update_distance(float deltaTime) {
  static float distance[2];
  distanceX += velocityX * deltaTime;
  distanceY += velocityY * deltaTime;
  distance[0] = distanceX;
  distance[1] = distanceY;
  return distance;
}

void setup() {
  Serial.begin(115200);
  Wire.begin(4, 5); // Initialize I2C
  delay(100);
  mpu68.initialize();

  if (!mpu68.testConnection()) {
    Serial.println("MPU6050 connection failed!");
    while (1);
  }

  devStatus = mpu68.dmpInitialize();
  if (devStatus == 0) {
    mpu68.setDMPEnabled(true);
    dmpReady = true;
    packetSize = mpu68.dmpGetFIFOPacketSize();
  } else {
    Serial.println("DMP Initialization Failed!");
  }

  prevTime = millis();
}

void loop() {
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
    accelYaw = yaw;
    fusedYaw = alpha * (fusedYaw + gyroYaw * dt) + (1 - alpha) * accelYaw;

    if (recalibrate) {
      yawOffset = fusedYaw;
      pitchOffset = pitch;
      rollOffset = roll;
      recalibrate = false;
    }

    float correctedYaw = fusedYaw - yawOffset;
    float correctedPitch = pitch - pitchOffset;
    float correctedRoll = roll - rollOffset;

    Serial.print("Yaw: "); Serial.print(correctedYaw);
    Serial.print("\tPitch: "); Serial.print(correctedPitch);
    Serial.print("\tRoll: "); Serial.println(correctedRoll);

    float* speed = update_velocity(dt);
    float* distance = update_distance(dt);

    Serial.print("VelocityX: "); Serial.println(speed[0]);
    Serial.print("VelocityY: "); Serial.println(speed[1]);
    Serial.print("DistanceX: "); Serial.println(distance[0]);
    Serial.print("DistanceY: "); Serial.println(distance[1]);

    delay(50);
  }
}
