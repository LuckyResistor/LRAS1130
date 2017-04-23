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

/// @example MovingPixel.ino
/// This is an example how to scroll multiple frames.

using namespace lr;
AS1130 ledDriver;


typedef AS1130Picture24x5 Picture;
Picture picture;
uint8_t positionX = 0;
uint8_t positionY = 0;


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
  picture.setPixel(positionX, positionY, true);
  ledDriver.setOnOffFrame(0, picture);
  delay(50);
  picture.setPixel(positionX, positionY, false);
  const uint8_t direction = random(4);
  switch (direction) {
  case 0:
    if (positionX < Picture::getWidth()-1) {
      ++positionX;
    }
    break;
  case 1:
    if (positionY < Picture::getHeight()-1) {
      ++positionY;
    }
    break;
  case 2:
    if (positionX > 0) {
      --positionX;
    }
    break;
  case 3:
    if (positionY > 0) {
      --positionY;
    }
    break;
  }
}


