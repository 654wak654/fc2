/*
 * Copyright (C) 2018 Ozan Egitmen
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *  http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <SPI.h>
#include <SD.h>
#include <Adafruit_BMP085.h>
#include <MPU6050_tockn.h>

#define SEPERATION_ACCELERATION 0.2
#define PARACHUTE_ALTITUDE 25

#define DEBUG 1
#define BAUD_RATE 9600
#define SD_PORT 10

Adafruit_BMP085 bmp;
MPU6050 mpu(Wire);
int32_t basePressure;
float baseAcceleration;
File logFile;

void setup()
{
    Serial.begin(BAUD_RATE);
    if (!SD.begin(SD_PORT))
    {
        Serial.println("ERROR: Couldn't init SD!");
        return;
    }
    if (!bmp.begin(BMP085_HIGHRES))
    {
        Serial.println("ERROR: Couldn't init bmp!");
        return;
    }
    mpu.begin();
#ifdef DEBUG
    mpu.calcGyroOffsets(true);
#else
    mpu.calcGyroOffsets(false);
#endif
    mpu.update();

    basePressure = bmp.readPressure();
    baseAcceleration = pow(pow(mpu.getAccX(), 2) + pow(mpu.getAccY(), 2) + pow(mpu.getAccZ(), 2), 0.5);

    float gyroXOffset = mpu.getGyroXoffset();
    float gyroYOffset = mpu.getGyroYoffset();
    float gyroZOffset = mpu.getGyroZoffset();

#ifdef DEBUG
    Serial.print("basePressure = ");
    Serial.println(basePressure);
    Serial.print("baseAcceleration = ");
    Serial.println(baseAcceleration);
    Serial.print("gyroOffset = [");
    Serial.print(gyroXOffset);
    Serial.print(", ");
    Serial.print(gyroYOffset);
    Serial.print(", ");
    Serial.print(gyroZOffset);
    Serial.println("]");
#endif

    logFile = SD.open("FLT_LOG.BIN", FILE_WRITE);
    uint8_t buf[28] = {0, 0, 0, 0, 0, 0, 0, 0};
    memcpy(&buf[8], &basePressure, 4);
    memcpy(&buf[12], &baseAcceleration, 4);
    memcpy(&buf[16], &gyroXOffset, 4);
    memcpy(&buf[20], &gyroYOffset, 4);
    memcpy(&buf[24], &gyroZOffset, 4);
    logFile.write(buf, 28);
    logFile.flush();
}

bool stageSeperated = false;
bool parachuteDeployed = false;
float lastAltitudes[] = {0, 0, 0, 0, 0};

void loop()
{
    unsigned long curTime = millis();
    float altitude = bmp.readAltitude(basePressure);

    mpu.update();
    float accX = mpu.getAccX();
    float accY = mpu.getAccY();
    float accZ = mpu.getAccZ();
    float acceleration = pow(square(accX) + square(accY) + square(accZ), 0.5) - baseAcceleration;

#ifdef DEBUG
    Serial.print("altitude=");
    Serial.print(altitude);
    Serial.print("\t\tacceleration=");
    Serial.println(acceleration);
#endif

    float angleX = mpu.getAngleX();
    float angleY = mpu.getAngleY();
    float angleZ = mpu.getAngleZ();
    float bmpTemperature = bmp.readTemperature();
    float mpuTemperature = mpu.getTemp();

    uint8_t buf[42];
    memcpy(&buf[0], &curTime, 4);
    memcpy(&buf[4], &altitude, 4);
    memcpy(&buf[8], &accX, 4);
    memcpy(&buf[12], &accY, 4);
    memcpy(&buf[16], &accZ, 4);
    memcpy(&buf[20], &angleX, 4);
    memcpy(&buf[24], &angleY, 4);
    memcpy(&buf[28], &angleZ, 4);
    memcpy(&buf[32], &bmpTemperature, 4);
    memcpy(&buf[36], &mpuTemperature, 4);
    buf[40] = stageSeperated;
    buf[41] = parachuteDeployed;
    logFile.write(buf, 42);
    logFile.flush();

    if (altitude < 10)
    {
        return;
    }

    float avgOfLastAltitudes = 0;
    for (int i = 0; i < 5; i++)
    {
        avgOfLastAltitudes += lastAltitudes[i];
    }
    avgOfLastAltitudes /= 5;

    if (!stageSeperated && acceleration < SEPERATION_ACCELERATION && altitude < avgOfLastAltitudes)
    {
#ifdef DEBUG
        Serial.println("Seperating stage 2");
#endif
        // TODO: Seperate stage 2
        stageSeperated = true;
    }

    for (int i = 0; i < 4; i++)
    {
        lastAltitudes[i] = lastAltitudes[i + 1];
    }
    lastAltitudes[4] = altitude;

    if (!parachuteDeployed && stageSeperated && altitude < PARACHUTE_ALTITUDE)
    {
#ifdef DEBUG
        Serial.println("Deploying parachute");
#endif
        // TODO: Deploy parachute
        parachuteDeployed = true;
    }

#ifdef DEBUG
    unsigned long lastTime = millis();
    Serial.print("loop time: ");
    Serial.println(lastTime - curTime);
#endif
}
