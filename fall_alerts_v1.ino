#include <Adafruit_MPU6050.h>
#include <Wire.h>
#include <math.h>

Adafruit_MPU6050 mpu;

const int ledPin = 2;
const float FALL_THRESHOLD = 10;
const float TILT_ANGLE_THRESHOLD = 20.0;
const float GYRO_CHANGE_THRESHOLD = 3.0;
long fallStartTime = 0;
bool fallConfirmed = false;

float accX_avg = 0, accY_avg = 0, accZ_avg = 0;
float gyroX_avg = 0, gyroY_avg = 0, gyroZ_avg = 0;
const int WS = 5;
float accX_window[WS], accY_window[WS], accZ_window[WS];
float gyroX_window[WS], gyroY_window[WS], gyroZ_window[WS];
int WI = 0;

void setup() {
  Serial.begin(115200);

  if (!mpu.begin()) {
    Serial.println("Failed to find MPU6050 chip");
    while (1) {
      delay(10);
    }
  }

  Serial.println("Found MPU6050");
  pinMode(ledPin, OUTPUT);
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

  float tlt_X = atan2(accY, accZ) * 180 / PI;
  float tlt_Y = atan2(accX, accZ) * 180 / PI;
  float tot_acc = sqrt(accX * accX + accY * accY + accZ * accZ);
  float tot_gyr = sqrt(gyroX * gyroX + gyroY * gyroY + gyroZ * gyroZ);

  // Serial.print("Acceleration X: "); Serial.print(accX);
  // Serial.print(", Y: "); Serial.print(accY);
  // Serial.print(", Z: "); Serial.print(accZ);
  // Serial.print(" | Gyro X: "); Serial.print(gyroX);
  // Serial.print(", Y: "); Serial.print(gyroY);
  // Serial.print(", Z: "); Serial.print(gyroZ);
  // Serial.print(" | Total Acc: "); Serial.print(tot_acc);
  // Serial.print(" | Total Gyro: "); Serial.print(tot_gyr);
  // Serial.print(" | Roll Avg Acc: "); Serial.print(accX_avg);
  // Serial.print(", "); Serial.print(accY_avg);
  // Serial.print(", "); Serial.print(accZ_avg);
  // Serial.print(" | Roll Avg Gyro: "); Serial.print(gyroX_avg);
  // Serial.print(", "); Serial.print(gyroY_avg);
  // Serial.print(", "); Serial.print(gyroZ_avg);
  // Serial.print(" | Tilt X: "); Serial.print(tlt_X);
  // Serial.print(" | Tilt Y: "); Serial.print(tlt_Y);
  // Serial.print(" | Still Time: "); Serial.println(millis() - fallStartTime);

  // Serial.print(accX);
  // Serial.print(","); Serial.print(accY);
  // Serial.print(","); Serial.print(accZ);
  // Serial.print(","); Serial.print(gyroX);
  // Serial.print(","); Serial.print(gyroY);
  // Serial.print(","); Serial.print(gyroZ);
  Serial.print(","); Serial.print(tot_acc);
  Serial.print(","); Serial.print(tot_gyr);
  // Serial.print(","); Serial.print(accX_avg);
  // Serial.print(","); Serial.print(accY_avg);
  // Serial.print(","); Serial.print(accZ_avg);
  // Serial.print(","); Serial.print(gyroX_avg);
  // Serial.print(","); Serial.print(gyroY_avg);
  // Serial.print(","); Serial.print(gyroZ_avg);
  Serial.print(","); Serial.print(tlt_X);
  Serial.print(","); Serial.print(tlt_Y);
  Serial.print(","); Serial.println(millis() - fallStartTime);
  
  filterData(accX, accY, accZ, gyroX, gyroY, gyroZ);
  detectFall(accX_avg, accY_avg, accZ_avg, gyroX_avg, gyroY_avg, gyroZ_avg, tlt_X, tlt_Y);
  delay(200);
  fallStartTime = millis();
}

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

  WI = (WI + 1) % WS;
}

void detectFall(float accX, float accY, float accZ, float gyroX, float gyroY, float gyroZ, float tlt_X, float tlt_Y) {
  float tot_acc = sqrt(accX * accX + accY * accY + accZ * accZ);
  float tot_gyr = sqrt(gyroX * gyroX + gyroY * gyroY + gyroZ * gyroZ);

  if (tot_acc > FALL_THRESHOLD || tot_gyr > GYRO_CHANGE_THRESHOLD) {
    if (!fallConfirmed) {
      fallStartTime = millis();
      fallConfirmed = true;
      Serial.println("Possible fall detected, calling confirmation function");
    }
  }

  if (fallConfirmed && (millis() - fallStartTime > 5000)) {
    if (abs(tlt_X) > TILT_ANGLE_THRESHOLD || abs(tlt_Y) > TILT_ANGLE_THRESHOLD) {
      Serial.println("Fall confirmed!");
      Serial.println("fall:1");
      alertFall();
    } else {
      Serial.println("False alarm, no fall.");
      Serial.println("fall:0");
    }
    fallConfirmed = false;
  }
}

void alertFall() {
  for (int i = 0; i < 5; i++) {
    digitalWrite(ledPin, HIGH);
    delay(200);
    digitalWrite(ledPin, LOW);
    delay(200);
  }
}
