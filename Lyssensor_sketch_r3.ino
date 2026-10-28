#include <Wire.h>

// VEML7700 uses the 7-bit I2C address 0x10.
const uint8_t VEML7700_ADDRESS = 0x10;
const uint8_t ALS_CONF_REGISTER = 0x00;
const uint8_t ALS_DATA_REGISTER = 0x04;
// Gain x1 and 100 ms integration time; the matching lux scale is below.
const uint16_t ALS_CONFIGURATION = 0x1800;
const float LUX_PER_COUNT = 0.0576f;
const uint8_t GROVE_LIGHT_SENSOR_PIN = A0;

bool sensorReady = false;
unsigned long nextInitAttempt = 0;

// VEML7700 register words are transferred least-significant byte first.
bool writeRegister(uint8_t reg, uint16_t value)
{
  Wire.beginTransmission(VEML7700_ADDRESS);
  Wire.write(reg);
  Wire.write(value & 0xFF);
  Wire.write(value >> 8);
  return Wire.endTransmission() == 0;
}

bool readRegister(uint8_t reg, uint16_t &value)
{
  Wire.beginTransmission(VEML7700_ADDRESS);
  Wire.write(reg);
  // Keep the I2C bus active while switching from register selection to read.
  if (Wire.endTransmission(false) != 0)
    return false;

  if (Wire.requestFrom(VEML7700_ADDRESS, (uint8_t)2) != 2)
    return false;

  const uint8_t lowByte = Wire.read();
  const uint8_t highByte = Wire.read();
  value = ((uint16_t)highByte << 8) | lowByte;
  return true;
}

bool initializeSensor()
{
  if (!writeRegister(ALS_CONF_REGISTER, ALS_CONFIGURATION))
    return false;

  // Allow the sensor to complete its first integration after configuration.
  delay(120);
  return true;
}

void setup()
{
  Serial.begin(115200);
  Wire.begin();
  pinMode(GROVE_LIGHT_SENSOR_PIN, INPUT);

  sensorReady = initializeSensor();
  if (!sensorReady)
  {
    Serial.println("# ERROR: VEML7700 initialization failed at I2C address 0x10");
    nextInitAttempt = millis() + 1000;
  }
}

void loop()
{
  // Retry initialization periodically, while still reporting the analog sensor.
  if (!sensorReady)
  {
    if ((long)(millis() - nextInitAttempt) >= 0)
    {
      sensorReady = initializeSensor();
      nextInitAttempt = millis() + 1000;
      if (!sensorReady)
        Serial.println("# ERROR: VEML7700 initialization failed at I2C address 0x10");
    }
  }

  if (sensorReady)
  {
    uint16_t rawLight;
    if (!readRegister(ALS_DATA_REGISTER, rawLight))
    {
      Serial.println("# ERROR: failed to read VEML7700");
      sensorReady = false;
      nextInitAttempt = millis() + 1000;
    }
    else
    {
      // Convert the VEML7700 raw count to lux for gain x1 / 100 ms.
      Serial.print("/lys/sensor1 ");
      Serial.println(rawLight * LUX_PER_COUNT, 2);
    }
  }

  // Grove analog sensor is reported as the board's raw ADC reading.
  Serial.print("/lys/sensor2 ");
  Serial.println(analogRead(GROVE_LIGHT_SENSOR_PIN));
  delay(250);
}
