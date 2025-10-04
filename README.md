
[![Arduino CI](https://github.com/RobTillaart/AS7331/workflows/Arduino%20CI/badge.svg)](https://github.com/marketplace/actions/arduino_ci)
[![Arduino-lint](https://github.com/RobTillaart/AS7331/actions/workflows/arduino-lint.yml/badge.svg)](https://github.com/RobTillaart/AS7331/actions/workflows/arduino-lint.yml)
[![JSON check](https://github.com/RobTillaart/AS7331/actions/workflows/jsoncheck.yml/badge.svg)](https://github.com/RobTillaart/AS7331/actions/workflows/jsoncheck.yml)
[![GitHub issues](https://img.shields.io/github/issues/RobTillaart/AS7331.svg)](https://github.com/RobTillaart/AS7331/issues)

[![License: MIT](https://img.shields.io/badge/license-MIT-green.svg)](https://github.com/RobTillaart/AS7331/blob/master/LICENSE)
[![GitHub release](https://img.shields.io/github/release/RobTillaart/AS7331.svg?maxAge=3600)](https://github.com/RobTillaart/AS7331/releases)
[![PlatformIO Registry](https://badges.registry.platformio.org/packages/robtillaart/library/AS7331.svg)](https://registry.platformio.org/libraries/robtillaart/AS7331)


# AS7331

Arduino library for the I2C AS7331 UV sensor. UV-A, UV-B, UV-C 


## Warning

**Always take precautions as UV radiation can cause sunburn, eye damage and other severe problems**.

Do not expose yourself to the sun or any other UV source too long.

When working with UV light, natural or artificial (TL LED laser a.o.) use appropriate shielding. Do not look right into UV light sources (e.g. the sun). Do not expose yourself to any UV source too long.


## Description

**Experimental**

This Arduino library is to use the AS7331 UV sensor.
The AS7331 is a device that measures UV-A, UV-B and UV-C and internal temperature (Celsius) simultaneously.

|  Channel  |  peak wavelength  |  Range            |
|:---------:|:-----------------:|:------------------|
|    UVC    |  λ = 260 nm       |  240 nm – 280 nm  |
|    UVB    |  λ = 300 nm       |  280 nm – 315 nm  |
|    UVA    |  λ = 360 nm       |  315 nm – 410 nm  |

The library allows the user to set the **GAIN** from 1 to 2048 and an **exposure time** from 1 millisecond to 16384 milliseconds (16+ seconds).

The device has more options to configure but these are not implemented yet.


Check datasheet for details about sensitivity etc.

Feedback as always is welcome.


### Breaking change 0.2.0

The 0.1.0 version is obsolete as it just did not work. 
Version 0.2.0 has been verified to work in MANUAL and CONTINUOUS mode.


### Datasheet warning

_Do not apply this product to safety protection devices or emergency stop equipment,
and any other applications that may cause personal injury due to the product's failure._


## Angle sensitivity

Angle of incidence is +-10 degrees, so Cosine law not needed(?).
Search for Lambert’s Cosine Law on Wikipedia for details.


### Special chars.

µ = ALT-230,
λ = ALT-955,
Via emoji tab: WIN .  => Ω tab => search for λ)


### Related

UV related

- https://en.wikipedia.org/wiki/Ultraviolet
- https://github.com/RobTillaart/AnalogUVSensor
- https://github.com/RobTillaart/LTR390_DFR
- https://github.com/RobTillaart/LTR390_RT
- https://github.com/RobTillaart/ML8511

Other

- https://github.com/RobTillaart/map2colour - for a continuous colour scale
- https://github.com/RobTillaart/Temperature - conversions.


### Tested (not yet)

This library is NOT tested with hardware yet (will be ordered soon).

TODO: Arduino UNO / ESP32 (there is no specific code).

https://www.tinytronics.nl/nl/sensoren/optisch/licht-en-kleur/as7331-uv-lichtsensor-module


## I2C

### I2C Performance

The datasheet states that the device support up to 400 kHz fast mode.
(not verified the actual range)
As the exposure time "blocks the device" quite some time, the only relevant 
function to test seems to be **conversionReady()** as that can be called 
many times.

|  Clock     |  time (us)  |  Notes  |
|:----------:|:-----------:|:--------|
|   100 KHz  |             |  default
|   200 KHz  |             |
|   300 KHz  |             |
|   400 KHz  |             |  max datasheet
|   500 KHz  |             |
|   600 KHz  |             |

TODO: write and run performance sketch on hardware.


### I2C address

The device has two address lines allowing for up to four I2C addresses,
on one I2C bus.

|  A0  |  A1  |  Address  |
|:----:|:----:|:---------:|
|   0  |   0  |    0x74   |
|   1  |   0  |    0x75   |
|   0  |   1  |    0x76   |
|   1  |   1  |    0x77   |


### I2C multiplexing

Sometimes you need to control more devices than possible with the default
address range the device provides.
This is possible with an I2C multiplexer e.g. TCA9548 which creates up
to eight channels (think of it as I2C subnets) which can use the complete
address range of the device.

Drawback of using a multiplexer is that it takes more administration in
your code e.g. which device is on which channel.
This will slow down the access, which must be taken into account when
deciding which devices are on which channel.
Also note that switching between channels will slow down other devices
too if they are behind the multiplexer.

- https://github.com/RobTillaart/TCA9548


## Interface

```cpp
#include "AS7331.h"
```

### Constructor

- **AS7331(uint8_t address, TwoWire \*wire = &Wire)** set the I2C address to use and optional select an I2C bus.
- **bool begin()** resets the device to the default configuration, **AS7331_MODE_MANUAL**, **AS7331_GAIN_2x** and **AS7331_CONV_064**.
Returns true if the device address can be found on I2C bus.
- **bool isConnected()** Returns true if the device address can be found on I2C bus.
- **uint8_t getAddress()** Returns the device address set in the constructor.


### Mode

Note: these functions only work in CONFIGURATION MODE.

- **void setMode(uint8_t mode = AS7331_MODE_MANUAL)** idem. Manual mode is default.
- **uint8_t getMode()** returns the set mode.

|  Mode                    |  value  |  Notes  |
|:-------------------------|:-------:|:--------|
|  AS7331_MODE_CONTINUOUS  |   0x00  |
|  AS7331_MODE_MANUAL      |   0x01  |
|  AS7331_MODE_SYNS        |   0x02  |  not supported / tested yet
|  AS7331_MODE_SYND        |   0x03  |  not supported / tested yet


### Standby

Note: these functions only work in CONFIGURATION MODE.

For temporary saving of energy one can put the device in a standby mode.
Read the datasheet for the details.

- **void setStandByOn()** idem.
- **void setStandByOff()** idem.


### Gain

Note: these functions only work in CONFIGURATION MODE.

The gain parameter is 0..11, which goes from 2048x .. 1x.
See table below.
  
- **bool setGain(uint8_t gain)** sets the gain, if value > 11 the
function returns false.
- **uint8_t getGain()** returns the set value from device.

|  define             |  value  |  Notes  |
|:--------------------|:-------:|:--------|
|  AS7331_GAIN_1x     |   11    |
|  AS7331_GAIN_2x     |   10    |  default
|  AS7331_GAIN_4x     |    9    |
|  AS7331_GAIN_8x     |    8    |
|  AS7331_GAIN_16x    |    7    |
|  AS7331_GAIN_32x    |    6    |
|  AS7331_GAIN_64x    |    5    |
|  AS7331_GAIN_128x   |    4    |
|  AS7331_GAIN_256x   |    3    |
|  AS7331_GAIN_512x   |    2    |
|  AS7331_GAIN_1024x  |    1    |
|  AS7331_GAIN_2048x  |    0    |


### Timing conversion (Tconv)

Note: these functions only work in CONFIGURATION MODE.

The convTime parameter (exposure time) is 0..15, which goes from 1 millisecond
to 16384 millisecond. See table below.

Note value 15 is 1 millisecond like value 0 and in fact uses value 0 (overruled).
The difference need to be investigated.


- **void setConversionTime(uint8_t convTime)** set Tconv, 
if the value > 15 the function returns false.
- **uint8_t getConversionTime()** returns set value.

The number in the define indicates the milliseconds.

|  define             |  value  |  Notes  |
|:--------------------|:-------:|:--------|
|  AS7331_CONV_001    |    0    |  default
|  AS7331_CONV_002    |    1    |
|  AS7331_CONV_004    |    2    |
|  AS7331_CONV_008    |    3    |
|  AS7331_CONV_016    |    4    |
|  AS7331_CONV_032    |    5    |
|  AS7331_CONV_064    |    6    |
|  AS7331_CONV_128    |    7    |
|  AS7331_CONV_256    |    8    |
|  AS7331_CONV_512    |    9    |
|  AS7331_CONV_1024   |   10    |
|  AS7331_CONV_2048   |   11    |
|  AS7331_CONV_4096   |   12    |
|  AS7331_CONV_8192   |   13    |
|  AS7331_CONV_16384  |   14    |
|  AS7331_CONV_001xx  |   15    |  1 millisecond (not supported yet).


### DeviceID

- **uint8_t getDeviceID()** expect to return 0x21.


### OSR control

TODO test / elaborate.

- **void stopMeasurement()** idem.
- **void startMeasurement()** idem.
- **void powerDown()** idem.
- **void powerUp()** idem.
- **void softwareReset()** idem.
- **void setConfigurationMode()** idem.
- **void setMeasurementMode()** idem.


### RDY ready pin

Read the datasheet.

- **void setRDYOpenDrain()**
- **void setRDYPushPull()** default


### Internal Clock

Datasheet figure 33, page 38.

The CCLK parameter is 0..3, The maximum gain can only be used
when the CCLK == 0. See datasheet page 38 for details.

- **bool setClockFrequency(uint8_t CCLK = 0)** set 0..3.
- **uint8_t getClockFrequency()**

|  define            |  value  |  Freq (MHz)  |  Notes  |
|:-------------------|:-------:|:------------:|:--------|
|  AS7331_CCLK_1024  |    0    |     1.024    |  default
|  AS7331_CCLK_2048  |    1    |     2.048    |
|  AS7331_CCLK_4096  |    2    |     4.096    |
|  AS7331_CCLK_8192  |    3    |     8.192    |


### Read

Note: these functions only work in MEASUREMENT MODE. (8.2.9)

- **float getUVA()** returns in microWatts / cm2
- **float getUVB()** returns in microWatts / cm2
- **float getUVC()** returns in microWatts / cm2
- **float getCelsius()** returns in Celsius.


### Status

Note: these functions only work in MEASUREMENT MODE. (8.2.9)

- **uint8_t readStatus()** get status back, see table below.
- **bool conversionReady()** from NOTREADY status field. (or use NDATA?)

Status table, see datasheet for details.

|  bit  |  name          |  test  |  meaning  |
|:-----:|:---------------|:------:|:----------|
|   7   |  OUTCONVOF     |    N   |  overflow 24 bit
|   6   |  MRESOF        |    N   |  overflow of one of the UV registers
|   5   |  ADCOF         |    N   |  internal overflow
|   4   |  LDATA         |    N   |  data overwritten
|   3   |  NDATA         |    Y   |  new data available - used by conversionReady().
|   2   |  NOTREADY      |    N   |  0 = ready, 1 = busy, inverted RDY pin.
|   1   |  STANDBYSTATE  |    N   |  1 = stand by
|   0   |  POWERSTATE    |    N   |  1 = power down

TODO: only NDATA is tested yet.


### RDY pin

When the AS7331 is converting the READY pin is LOW,
when the conversion is done the READY pin pulses HIGH for a short moment.
This can be used to trigger an interrupt. See examples.

This end of conversion info is also available in the status register and with:
- **bool conversionReady()**


### SYN pin

TODO, not supported yet.

Used for external trigger of start/stop measurement 


### Debug

- **uint8_t getLastError()** returns last error of low level communication.


## Future

#### Must

- test different configurations (gain Tconv)
- improve documentation
- fix TODO's in code and documentation

#### Should

- adjustGainTimeFactor(), must it include CCLK factor?
  (gain is limited with higher clocks, or should gain be leading?).
- how about 17-24 bits reads
- reorganize code
- add + test SYNS mode
- add + test SYND mode
- add examples
  - Wire1 on ESP32
  - div modi.

#### Could

- unit test
- I2C performance (as conversion time can be long not relevant?)
- implement Lambert law to compensate for incoming angle.
- check handle Tconv == 15 case (last column) correctly.
- mention VEML6070 ?


#### Wont


## Support

If you appreciate my libraries, you can support the development and maintenance.
Improve the quality of the libraries by providing issues and Pull Requests, or
donate through PayPal or GitHub sponsors.

Thank you,


