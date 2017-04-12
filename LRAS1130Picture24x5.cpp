//
// Lucky Resistor's AS1130 Library
// ---------------------------------------------------------------------------
// (c)2017 by Lucky Resistor. See LICENSE for details.
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>
//
#include "LRAS1130.h"


#include <Arduino.h>


#ifdef ARDUINO_ARCH_AVR
// Make it compatible with the standart
#include <string.h>
namespace std { using ::memset; } 
namespace std { using ::memcpy; }
#else
#include <cstring>
#endif


namespace lr {


AS1130Picture24x5::AS1130Picture24x5()
{
  std::memset(_data, 0, getDataByteCount());
}


AS1130Picture24x5::AS1130Picture24x5(const uint8_t *data)
{
  std::memcpy(_data, data, getDataByteCount());
}


void AS1130Picture24x5::setPixel(uint8_t x, uint8_t y, bool enabled)
{
  if (x < getWidth() && y < getHeight()) {
    const uint8_t bitMask = getDataBit(x, y);
    if (enabled) {
      _data[getDataIndex(x,y)] |= bitMask;
    } else {
      _data[getDataIndex(x,y)] &= ~bitMask;
    }
  }
}


bool AS1130Picture24x5::getPixel(uint8_t x, uint8_t y)
{
  if (x < getWidth() && y < getHeight()) {
    const uint8_t bitMask = getDataBit(x, y);
    return (_data[getDataIndex(x,y)]&bitMask)!=0;
  } else {
    return false;
  }
}


void AS1130Picture24x5::writeRegisters(uint8_t *registerData, const uint8_t *rawData, uint8_t pwmSetIndex)
{
  std::memset(registerData, 0, 0x18);
  for (uint8_t x = 0; x < getWidth(); ++x) {
    for (uint8_t y = 0; y < getHeight(); ++y) {
      if ((rawData[getDataIndex(x, y)] & getDataBit(x,y)) != 0) {
        const uint8_t ledIndex = (x*5+y);
        const uint8_t registerBitIndex = ledIndex%10;
        uint8_t registerIndex = (ledIndex/10)*2+(registerBitIndex/8);
        registerData[registerIndex] |= (1<<(registerBitIndex&7));
      }
    }
  }
  registerData[1] |= (pwmSetIndex<<5);
}


}

