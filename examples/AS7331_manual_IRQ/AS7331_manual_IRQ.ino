//
//    FILE: AS7331_manual_IRQ.ino
//  AUTHOR: Rob Tillaart
// PURPOSE: test basic behaviour (verbose)
//     URL: https://github.com/RobTillaart/AS7331

//  Connect the Ready Pin to an IRQ Pin e.g. 2
//  to detect the conversion is ready.


#include "AS7331.h"


AS7331 mySensor(0x74);

volatile bool AS7331_READY = false;
const uint8_t IRQ_PIN = 2;

void RDY_IRQ()
{
  AS7331_READY = true;
}


void setup()
{
  Serial.begin(115200);
  Serial.println();
  Serial.println(__FILE__);
  Serial.print(F("AS7331_LIB_VERSION: "));
  Serial.println(AS7331_LIB_VERSION);
  Serial.println();

  // READY Interrupt.
  pinMode(2, INPUT);
  attachInterrupt(digitalPinToInterrupt(IRQ_PIN), RDY_IRQ, RISING);

  Wire.begin();

  if (mySensor.begin() == false)
  {
    Serial.print("invalid address: ");
    Serial.println(mySensor.getAddress(), HEX);
    while (1);
  }

  //  explicit reset
  mySensor.softwareReset();
  Serial.println("softwareReset: ");

  //  select configuration mode
  mySensor.setConfigurationMode();
  Serial.print("setConfigurationMode: ");
  Serial.println(mySensor.readOSR(), HEX);

  mySensor.powerUp();
  Serial.print("powerUP: ");
  Serial.println(mySensor.readOSR(), HEX);

  Serial.print("getDeviceID: ");
  Serial.println(mySensor.getDeviceID(), HEX);
  Serial.println();

  //  select manual mode,
  mySensor.setMode(AS7331_MODE_MANUAL);
  Serial.print("setMode: ");
  Serial.println(mySensor.getMode());

  mySensor.setStandByOff();
  Serial.print("GetStandByMode: ");
  Serial.println(mySensor.getStandByMode(), HEX);

  mySensor.setGain(AS7331_GAIN_256x);
  Serial.print("setGain: ");
  Serial.println(mySensor.getGain());

  mySensor.setConversionTime(AS7331_CONV_4096);
  Serial.print("setConversionTime: ");
  Serial.println(mySensor.getConversionTime());

  mySensor.startMeasurement();
  Serial.print("startMeasurement: ");
  Serial.println(mySensor.readStatus(), HEX);
}


void loop()
{
  if (AS7331_READY == true)
  {
    AS7331_READY = false;
    //  Serial.print("STAT:\t");
    //  Serial.println(mySensor.readStatus(), HEX);
    Serial.print("UVA:\t");
    Serial.println(mySensor.getUVA());
    Serial.print("UVB:\t");
    Serial.println(mySensor.getUVB());
    Serial.print("UVC:\t");
    Serial.println(mySensor.getUVC());
    Serial.print("TEMP:\t");
    Serial.println(mySensor.getCelsius());
    Serial.print("ERR:\t");
    Serial.println(mySensor.getLastError());
    Serial.println();

    //  start new measurement.
    mySensor.startMeasurement();
  }

  //  other things.
}


//  -- END OF FILE --
