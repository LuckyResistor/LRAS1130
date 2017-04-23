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

/// @example DrawPixels.ino
/// This is an example how to scroll multiple frames.

using namespace lr;
AS1130 ledDriver;


AS1130Picture24x5 picture1;
AS1130Picture24x5 picture2;


void setup() {
  Wire.begin();
  Serial.begin(115200);
    
  // Wait until the chip is ready.
  delay(100); 
  Serial.println(F("Initialize chip"));
  
  // Check if the chip is addressable.
  if (!ledDriver.isChipConnected()) {
    Serial.println(F("Communication problem with chip."));
    Serial.flush();
    return;
  }

  picture1.setPixel(0, 0, true);
  picture1.setPixel(1, 1, true);
  picture1.setPixel(2, 2, true);

  picture2.setPixel(10, 0, true);
  picture2.setPixel(9, 1, true);
  picture2.setPixel(8, 2, true);

  // Set-up everything.
  ledDriver.setRamConfiguration(AS1130::RamConfiguration1);
  ledDriver.setOnOffFrameAllOff(0);
  ledDriver.setBlinkAndPwmSetAll(0);
  ledDriver.setCurrentSource(AS1130::Current30mA);
  ledDriver.setScanLimit(AS1130::ScanLimitFull);
  ledDriver.setMovieEndFrame(AS1130::MovieEndWithFirstFrame);
  ledDriver.setMovieFrameCount(4);
  ledDriver.setFrameDelayMs(100);
  ledDriver.setMovieLoopCount(AS1130::MovieLoop6);
  ledDriver.setScrollingEnabled(true);
  ledDriver.startPicture(0);
  
  // Enable the chip
  ledDriver.startChip();
}


void loop() {
  ledDriver.setOnOffFrame(0, picture1);
  delay(800);
  ledDriver.setOnOffFrame(0, picture2);
  delay(800);
}


