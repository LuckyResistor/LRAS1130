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
#pragma once


#ifdef ARDUINO_ARCH_AVR
#include <inttypes.h>
#else
#include <cinttypes>
#endif


namespace lr {


/// @brief One single bitmap in 12x11 layout for manual modification or storage.
///
class AS1130Picture12x11
{
public:
  /// @brief Create a empty bitmap.
  ///
  AS1130Picture12x11();
  
  /// @brief Create a bitmap using an existing bitmask.
  ///
  /// The bits from the data are copied to the local structure.
  /// All bits in this bitmask has to be in high to low order.
  ///
  /// Bits: 01234567 89AB0123 456789AB ...
  ///
  /// The last 4 bits are ignored.
  /// 
  /// @param data A bitmask with 17 bytes.
  ///
  AS1130Picture12x11(const uint8_t *data);
  
public:
  /// @brief Set a pixel in this bitmap.
  ///
  /// The coordinates are bounds checked.
  ///
  /// @param x The X coordinate of the pixel.
  /// @param y The Y coordinate of the pixel.
  /// @param enabled `true` to set the pixel and `false` to clear it.
  ///
  void setPixel(uint8_t x, uint8_t y, bool enabled);
  
  /// @brief Get the state of a single pixel.
  ///
  /// @param x The X coordinate of the pixel.
  /// @param y The Y coordinate of the pixel.
  ///
  bool getPixel(uint8_t x, uint8_t y);

  /// @brief Get the width of this bitmap.
  ///
  /// @return The width of the bitmap in pixels.
  ///
  inline static uint8_t getWidth() { return 12; }
  
  /// @brief Get the height of this bitmap.
  ///
  /// @return The height of the bitmap in pixels.
  ///
  inline static uint8_t getHeight() { return 11; }
  
  /// @brief Get the number of raw byte for this bitmap.
  ///
  /// @return The number of raw bytes used to store this bitmap.
  ///
  inline static uint8_t getDataByteCount() { return 17; }

  /// @brief Access the raw bit data.
  ///
  /// @return A pointer to the raw bit data.
  ///
  inline const uint8_t* getData() const { return _data; }
  
  /// @brief Get the bitmask for a given coordinate.
  ///
  /// @param x The X coordinate of the pixel.
  /// @param y The Y coordinate of the pixel.
  /// @return A bitmask with the addressed bit for the given coordinate.
  ///
  inline static uint8_t getDataBit(uint8_t x, uint8_t y) {
    if ((y&1)!=0) {
      x += 4;
    }
    return (1<<(7-(x&7)));
  }
  
  /// @brief Get the data index for a given coordinate.
  ///
  /// @param x The X coordinate of the pixel.
  /// @param y The Y coordinate of the pixel.
  /// @return The index of the byte for a given coordinate.
  ///
  inline static uint8_t getDataIndex(uint8_t x, uint8_t y) {
    if ((y&1)!=0) {
      x += 12;
    }
    return ((y/2)*3) + (x/8);
  }
  
  /// @brief Write the registers for this bitmap data.
  ///
  /// @param registerData A pointer to an array of 24 bytes for all frame registers.
  /// @param rawData A pointer to the raw bit data.
  /// @param pwmSetIndex The PWM index to write to the register data.
  ///
  static void writeRegisters(uint8_t *registerData, const uint8_t *rawData, uint8_t pwmSetIndex);
    
private:
  uint8_t _data[17]; ///< The raw bit data.
};


}


