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

/// @example DisplayPicture.ino
/// This is an example how to display a static picture.

using namespace lr;
AS1130 ledDriver;


const uint8_t exampleFrame1[] = {
  0b11111011, 0b11101111, 0b11111111,
  0b10001010, 0b00101000, 0b00000001,
  0b10001010, 0b00101000, 0b00000001,
  0b10001010, 0b00101000, 0b00000001,
  0b11111011, 0b11101111, 0b11111111};

void setup() {
  Wire.begin();
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
  ledDriver.setOnOffFrame24x5(0, exampleFrame1);
  ledDriver.setBlinkAndPwmSetAll(0);
  ledDriver.setCurrentSource(AS1130::Current30mA);
  ledDriver.setScanLimit(AS1130::ScanLimitFull);
  ledDriver.startPicture(0);
  
  // Enable the chip
  ledDriver.startChip();
}


void loop() {
}


