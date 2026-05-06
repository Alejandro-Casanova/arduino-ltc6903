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

/** @file
 *  @ingroup LTC6903
 *  Library Header File for LTC6903: 1kHz to 68MHz Serial Port Programmable 
 *  Oscillator
 */
#pragma once

#include <SPI.h>

#include <cstdint>

class LTC6903
{
public:
   static constexpr uint32_t DEFAULT_SPI_CLK_SPEED = UINT32_C(1'000'000);

   // Output Configuration
   enum class OutputCfg_E : uint8_t
   {
      CLOCK_ON_INVERTED_ON = 0b00,  // Clock on, inverted clock on
      CLOCK_OFF_INVERTED_ON = 0b01, // Clock off, inverted clock on
      CLOCK_ON_INVERTED_OFF = 0b10, // Clock on, inverted clock off
      POWERS_DOWN_CLOCKS = 0b11     // Powers down clocks
   };

   /**
    * @brief Constructs an LTC6903 object with the specified SPI settings.
    * @param csPin The chip select pin for the SPI interface.
    * @param spiInstance The SPI instance to use.
    * @param spiClockSpeed The SPI clock speed in Hz.
    */
   LTC6903(
      const uint8_t csPin, SPIClass &spiInstance,
      const uint32_t spiClockSpeed = DEFAULT_SPI_CLK_SPEED);
   ~LTC6903() = default;
   
   /**
    * @brief Initializes the LTC6903 with the specified frequency and output 
    *    configuration. Should be called from setup().
    * @param frequency The desired output frequency in Hz.
    * @param outputCfg The output configuration.
    */
   void Begin(const double frequency, const OutputCfg_E outputCfg) const;

private:
   void writeConfigRegister(const uint16_t registerValue) const;
   uint16_t calculateConfigCode(
      const double frequency, const OutputCfg_E outputCfg) const;

   const uint8_t CS_PIN;
   const uint32_t SPI_CLOCK_SPEED;

   SPIClass &spi;
};
