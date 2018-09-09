# fc2
Flight Controller 2 - The brains of the RockIEEE model rocket.

## Wiring:

### GY-87 Module:
- VCC to 3.3V
- GND to GND
- SCL to Analog 5
- SDA to Analog 4

### SD Module:
- VCC to 5V
- GND to GND
- CS to Digital 10
- MOSI to Digital 11
- MISO to Digital 12
- SCK to Digital 13

### Needed libraries:
- Adafruit BMP085 Library (Adafruit_BMP085)
- MPU6050_tockn (MPU6050_tockn)
