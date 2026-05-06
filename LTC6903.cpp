/*!
LTC6903: 1kHz to 68MHz Serial Port Programmable Oscillator

@verbatim

The LTC6903/LTC6904 are low power self contained digital
frequency sources providing a precision frequency from
1kHz to 68MHz, set through a serial port. The LTC6903/
LTC6904 require no external components other than a
power supply bypass capacitor, and they operate over a
single wide supply range of 2.7V to 5.5V.

@endverbatim

http://www.linear.com/product/LTC6903

http://www.linear.com/product/LTC6903#demoboards


Copyright 2018(c) Analog Devices, Inc.

All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:
 - Redistributions of source code must retain the above copyright
   notice, this list of conditions and the following disclaimer.
 - Redistributions in binary form must reproduce the above copyright
   notice, this list of conditions and the following disclaimer in
   the documentation and/or other materials provided with the
   distribution.
 - Neither the name of Analog Devices, Inc. nor the names of its
   contributors may be used to endorse or promote products derived
   from this software without specific prior written permission.
 - The use of this software may or may not infringe the patent rights
   of one or more patent holders.  This license does not release you
   from the requirement that you obtain separate licenses from these
   patent holders to use this software.
 - Use of the software either in source or binary form, must be run
   on or directly connected to an Analog Devices Inc. component.

THIS SOFTWARE IS PROVIDED BY ANALOG DEVICES "AS IS" AND ANY EXPRESS OR
IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, NON-INFRINGEMENT,
MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
IN NO EVENT SHALL ANALOG DEVICES BE LIABLE FOR ANY DIRECT, INDIRECT,
INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
LIMITED TO, INTELLECTUAL PROPERTY RIGHTS, PROCUREMENT OF SUBSTITUTE GOODS OR
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

/** @ingroup RF_Timing
 *  @{
 *  @defgroup LTC6903 LTC6903: 1kHz to 68MHz Serial Port Programmable Oscillator
 *  @}
 */

/** @file
 *  @ingroup LTC6903
 *  Library for LTC6903: 1kHz to 68MHz Serial Port Programmable Oscillator
 */
#include "LTC6903.h"

#include <Arduino.h>
#include <SPI.h>

#include <cmath>
using std::round;
#include <stdint.h>

LTC6903::LTC6903(
   const uint8_t csPin, SPIClass &spiInstance, const uint32_t spiClockSpeed)
   : CS_PIN(csPin),
     SPI_CLOCK_SPEED(spiClockSpeed),
     spi(spiInstance)
{
}

void LTC6903::Begin(const double frequency, const OutputCfg_E outputCfg) const
{
   pinMode(CS_PIN, OUTPUT);
   digitalWrite(CS_PIN, HIGH);

   // Initialize SPI in case it is not done externally
   // WARNING: SPI implementation should handle double initialization gracefully
   spi.begin();

   const uint16_t configCode = calculateConfigCode(frequency, outputCfg);
   writeConfigRegister(configCode);
}

void LTC6903::writeConfigRegister(const uint16_t registerValue) const
{
   SPI.beginTransaction(SPISettings(SPI_CLOCK_SPEED, MSBFIRST, SPI_MODE3));
   digitalWrite(CS_PIN, LOW);
   SPI.transfer16(registerValue);
   digitalWrite(CS_PIN, HIGH);
   SPI.endTransaction();
}

uint16_t LTC6903::calculateConfigCode(
   const double frequency, const OutputCfg_E outputCfg) const
{
   // Calculate OCT
   const double oct_double = 3.322 * std::log10(frequency / 1039.0);

   // Keep oct within range and round down
   const uint8_t oct =
      oct_double > 15.0
         ? UINT8_C(15)
         : (oct_double < 0.0 ? UINT8_C(0) : static_cast<uint8_t>(oct_double));

   // Calculate DAC code
   const double dac_double =
      2048.0 - (2078.0 * std::pow(2, (10 + oct)) / frequency);

   // Keep DAC within range and round to nearest integer
   const uint16_t dac =
      dac_double > 1023.0
         ? UINT16_C(1023)
         : (dac_double < 0.0 ? UINT16_C(0)
                             : static_cast<uint16_t>(std::round(dac_double)));

   const uint8_t cfg = static_cast<uint8_t>(outputCfg);

   return static_cast<uint16_t>(
      (static_cast<uint16_t>(oct) << 12) | (dac << 2) | cfg);
}