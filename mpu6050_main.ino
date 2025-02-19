#include <Adafruit_MPU6050.h>
#include <Wire.h>
#include <math.h>

Adafruit_MPU6050 mpu;

const int ledPin = 2;
const float FALL_THRESHOLD = 1.5;                                     // Acceleration Threshold for fall detection
const float TILT_ANGLE_THRESHOLD = 45.0;
const float GYRO_CHANGE_THRESHOLD = 3.0;                              // Threshold for angular velocity to detect falls, detected by gyroscope
long fallStartTime = 0;
bool fallConfirmed = false;

// Variables for filtering noisy data
float accX_avg = 0, accY_avg = 0, accZ_avg = 0;
float gyroX_avg = 0, gyroY_avg = 0, gyroZ_avg = 0;
const int WS = 5;                                                     // window size, number of readings to be consider for moving average
float accX_window[WS], accY_window[WS], accZ_window[WS];
float gyroX_window[WS], gyroY_window[WS], gyroZ_window[WS];
int WI = 0;                                                           // Window Index


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

  float accX = a.acceleration.x;
  float accY = a.acceleration.y;
  float accZ = a.acceleration.z;
  float gyroX = g.gyro.x;
  float gyroY = g.gyro.y;
  float gyroZ = g.gyro.z;

  // Serial.print("Acceleration X: ");
  // Serial.print(accX);
  // Serial.print(", Y: ");
  // Serial.print(accY);
  // Serial.print(", Z: ");
  // Serial.print(accZ);
  // Serial.println(" m/s^2");

  // Serial.print("Rotation X: ");
  // Serial.print(gyroZ);
  // Serial.print(", Y: ");
  // Serial.print(gyroY);
  // Serial.print(", Z: ");
  // Serial.print(gyroZ);
  // Serial.println(" rad/s");
  float tlt_X = atan2(accY, accZ) * 180 / PI;
  float tlt_Y = atan2(accX, accZ) * 180 / PI;

  float tot_acc = sqrt(accX * accX + accY * accY + accZ * accZ);
  float tot_gyr = sqrt(gyroX * gyroX + gyroY * gyroY + gyroZ * gyroZ);

  

  Serial.print("Acceleration change: ");
  Serial.print(tot_acc);
  Serial.print("\tGyro change: ");
  Serial.print(tot_gyr);

  Serial.print("Tilt Angle Change X: ");
  Serial.print(tlt_X);
  Serial.print("\t Tilt Angle Change Y: ");
  Serial.println(tlt_Y);

  filterData(accX, accY, accZ, gyroX, gyroY, gyroZ);
  detectFall(accX_avg, accY_avg, accZ_avg, gyroX_avg, gyroY_avg, gyroZ_avg, tlt_X, tlt_Y);

  delay(100);

}


// Function to filter data using moving average
// This is used to reduce noise from the sensor data, minimizing fall alerts

void filterData(float accX, float accY, float accZ, float gyroX, float gyroY, float gyroZ) {
  accX_window[WI] = accX;
  accY_window[WI] = accY;
  accZ_window[WI] = accZ;
  gyroX_window[WI] = gyroX;
  gyroY_window[WI] = gyroY;
  gyroZ_window[WI] = gyroZ;

  accX_avg = (accX_avg * (WS - 1) + accX) / WS;
  accY_avg = (accY_avg * (WS - 1) + accY) / WS;
  accZ_avg = (accZ_avg * (WS - 1) + accZ) / WS;
  gyroX_avg = (gyroX_avg * (WS - 1) + gyroX) / WS;
  gyroY_avg = (gyroY_avg * (WS - 1) + gyroY) / WS;
  gyroZ_avg = (gyroZ_avg * (WS - 1) + gyroZ) / WS;

  WI = (WI + 1) % WS;                                                                      // Moves to the next slot in the buffer

}


void detectFall(float accX, float accY, float accZ, float gyroX, float gyroY, float gyroZ, float tlt_X, float tlt_Y) {
  float tot_acc = sqrt(accX * accX + accY * accY + accZ * accZ);
  float tot_gyr = sqrt(gyroX * gyroX + gyroY * gyroY + gyroZ * gyroZ);

  if (tot_acc < FALL_THRESHOLD || tot_gyr > GYRO_CHANGE_THRESHOLD) {
    if (!fallConfirmed) {
      fallStartTime = millis();                                                           // Get current timestamp for fall start
      fallConfirmed = true;
      Serial.println("Possible fall detected, calling confirmation function");
    }
  }

  if (fallConfirmed && (millis() - fallStartTime > 5000)) {                               // 5-second confirmation
    if (abs(tlt_X) > TILT_ANGLE_THRESHOLD || abs(tlt_Y) > TILT_ANGLE_THRESHOLD) {
      Serial.println("Fall confirmed!");
      // Serial.println(1);                                                                  // Fall Confirmation
      alertFall();                                                                        // Trigger LED
    } else {
      Serial.println("False alarm, no fall.");
      // Serial.println(0);                                                                  // PM 5 = 0 (False Alert)     
    }
    fallConfirmed = false;
  }
}


// Alert function to trigger built-in LED

void alertFall() {
  Serial.println("LED Triggered");
  for (int i = 0; i < 5; i++) {
    digitalWrite(ledPin, HIGH);
    delay(200);
    digitalWrite(ledPin, LOW);
    delay(200);
  }
}




