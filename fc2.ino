#include <SPI.h>
#include <SD.h>
#include <Adafruit_BMP085.h>
#include <MPU6050_tockn.h>

#define SEPERATION_VELOCITY 0.3
#define PARACHUTE_ALTITUDE 25

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
    mpu.calcGyroOffsets(true);
    mpu.update();

    basePressure = bmp.readPressure();
    baseAcceleration = pow(pow(mpu.getAccX(), 2) + pow(mpu.getAccY(), 2) + pow(mpu.getAccZ(), 2), 0.5);

    logFile = SD.open("FLT_LOG.BIN", FILE_WRITE);
    logFile.print("\n--------------------\nFlight data:\nbasePressure = ");
    logFile.print(basePressure);
    logFile.print("\nbaseAcceleration = ");
    logFile.print(baseAcceleration);
    logFile.print("\ngyroOffset = [");
    logFile.print(mpu.getGyroXoffset());
    logFile.print(", ");
    logFile.print(mpu.getGyroYoffset());
    logFile.print(", ");
    logFile.print(mpu.getGyroZoffset());
    logFile.print("]\n");
    logFile.print("Everything after this is binary data.\n--------------------\n");
    logFile.flush();
}

bool stageSeperated = false;
bool parachuteDeployed = false;
bool inFlight = false;
float lastAltitudes[] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

void loop()
{
    float altitude = bmp.readAltitude(basePressure);
    if (altitude < 15 && !inFlight)
    {
        return;
    }
    inFlight = true;

    mpu.update();
    float accX = mpu.getAccX();
    float accY = mpu.getAccY();
    float accZ = mpu.getAccZ();
    float acceleration = pow(square(accX) + square(accY) + square(accZ), 0.5) - baseAcceleration;
    Serial.print("altitude=");
    Serial.print(altitude);
    Serial.print("\t\tacceleration=");
    Serial.print(acceleration);
    Serial.print("\n");

    // TODO: Get lastAltitudes avg and compare it to lastAltitude
    if (!stageSeperated && acceleration < SEPERATION_VELOCITY && lastAltitude > altitude)
    {
        Serial.println("Seperating stage 2");
        // TODO: Seperate stage 2
        stageSeperated = true;
    }
    // TODO: Push & shift to lastAltitudes
    lastAltitude = altitude;

    if (!parachuteDeployed && stageSeperated && altitude < PARACHUTE_ALTITUDE)
    {
        Serial.println("Deploying parachute");
        // TODO: Deploy parachute
        parachuteDeployed = true;
    }

    unsigned long currentTime = millis();
    float angleX = mpu.getAngleX();
    float angleY = mpu.getAngleY();
    float angleZ = mpu.getAngleZ();

    uint8_t buf[34];
    memcpy(&buf[0], &currentTime, 4);
    memcpy(&buf[4], &altitude, 4);
    memcpy(&buf[8], &accX, 4);
    memcpy(&buf[12], &accY, 4);
    memcpy(&buf[16], &accZ, 4);
    memcpy(&buf[20], &angleX, 4);
    memcpy(&buf[24], &angleY, 4);
    memcpy(&buf[28], &angleZ, 4);
    buf[32] = stageSeperated;
    buf[33] = parachuteDeployed;
    logFile.write(buf, 34);
    logFile.flush();
}
