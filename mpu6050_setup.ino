#include <Adafruit_MPU6050.h>
#include <Wire.h>
#include <math.h>

Adafruit_MPU6050 mpu;

void setup() {
  Serial.begin(115200);

  // Initialize MPU6050
  if (!mpu.begin()) {
    Serial.println("Failed to find MPU6050 chip");
    while (1) {
      delay(10);
    }
  }

  Serial.println("Found MPU6050");

}

void loop() {
  
  sensors_event_t a, g, temp;
  mpu.getEvent(&a, &g, &temp);

  float accelX = a.acceleration.x;
  float accelY = a.acceleration.y;
  float accelZ = a.acceleration.z;
  float gyroX = g.gyro.x;
  float gyroY = g.gyro.y;
  float gyroZ = g.gyro.z;

  // Serial.print("Acceleration X: ");
  // Serial.print(accelX);
  // Serial.print(", Y: ");
  // Serial.print(accelY);
  // Serial.print(", Z: ");
  // Serial.print(accelZ);
  // Serial.println(" m/s^2");

  // Serial.print("Rotation X: ");
  // Serial.print(gyroZ);
  // Serial.print(", Y: ");
  // Serial.print(gyroY);
  // Serial.print(", Z: ");
  // Serial.print(gyroZ);
  // Serial.println(" rad/s");
  float tlt_X = atan2(accelY, accelZ) * 180 / PI;
  float tlt_Y = atan2(accelX, accelZ) * 180 / PI;

  float tot_acc = sqrt(accelX * accelX + accelY * accelY + accelZ * accelZ);
  float tot_gyr = sqrt(gyroX * gyroX + gyroY * gyroY + gyroZ * gyroZ);

  Serial.print("Acceleration change: ");
  Serial.print(tot_acc);
  Serial.print("\tGyro change: ");
  Serial.print(tot_gyr);

  Serial.print("Tilt Angle Change X: ");
  Serial.print(tlt_X);
  Serial.print("\t Tilt Angle Change Y: ");
  Serial.println(tlt_Y);

  delay(100);
}


