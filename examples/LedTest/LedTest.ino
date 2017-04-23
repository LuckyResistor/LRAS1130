//
// Lucky Resistor's AS1130 Library
// ---------------------------------------------------------------------------
// (c)2017 by Lucky Resistor. See LICENSE for details.
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>
//
#include "LRAS1130.h"

/// @example LedTest.ino
/// This example tests all connected LEDs.

using namespace lr;
AS1130 ledDriver;


void setup() {
  // Initialize the Wire library.
  Wire.begin();
  
  // Initialize the serial port.
  Serial.begin(9600);
    
  // Wait until the chip is ready.
  delay(100); 
  Serial.println(F("Initialize chip"));
  
  // Check if the chip is addressable.
  if (!ledDriver.isChipConnected()) {
    Serial.println(F("Communication problem with chip."));
    Serial.flush();
    return;
  }

  // Set-up everything.
  ledDriver.setRamConfiguration(AS1130::RamConfiguration1);
  ledDriver.setOnOffFrameAllOn(0);
  ledDriver.setBlinkAndPwmSetAll(0);
  ledDriver.setCurrentSource(AS1130::Current30mA);
  ledDriver.setScanLimit(AS1130::ScanLimitFull);
  ledDriver.startPicture(0);
  
  // Enable the chip
  ledDriver.startChip();

  // Check for dead LEDs
  Serial.println(F("Run the LED test"));
  ledDriver.runManualTest();

  // Display the status of all leds.
  for (uint8_t ledIndex = 0x00; ledIndex < 0xbb; ++ledIndex) {
    Serial.print(F("LED 0x"));
    Serial.print(ledIndex, HEX);
    Serial.print(F(": "));
    switch (ledDriver.getLedStatus(ledIndex)) {
      case AS1130::LedStatusOk:
        Serial.println(F(" OK"));
        break;
      case AS1130::LedStatusOpen:
        Serial.println(F(" open"));
        break;
      case AS1130::LedStatusDisabled:
        Serial.println(F(" disabled"));
        break;      
    }
  }
}


void loop() {
}

