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


/// @mainpage
///
/// @section intro_sec Introduction
///
/// This library contains a class for a simple access to the AS1130 chip. 
/// The goal behind the library is to create a compact layer which will
/// generate simple and easy to understand code without adding too much
/// complexity and size to the final project.
///
/// @section requirements_sec Requirements
///
/// This library is writte for Arduino compatible chips. It requires a 
/// modern C++ compiler (C++11). The code also uses the "Wire" library 
/// from the Arduino project for the I2C communication.
///
/// @section classes_sec Classes
///
/// There is only the lr::AS1130 class. Read the documentation of this class
/// for all details.
///


/// @brief The namespace for all Lucky Resistor classes and types.
///
namespace lr {


namespace {


/// The address for the register selection.
///
const uint8_t cRegisterSelectionAddress = 0xfd;

  
}


AS1130::AS1130(ChipAddress chipAddress)
  : _chipAddress(chipAddress)
{
}


bool AS1130::isChipConnected()
{
  Wire.beginTransmission(_chipAddress); 
  Wire.write(cRegisterSelectionAddress); 
  Wire.write(RS_NOP); 
  return Wire.endTransmission() == 0; 
}


void AS1130::setRamConfiguration(RamConfiguration ramConfiguration)
{
  writeControlRegisterBits(CR_Config, CF_MemoryConfigMask, ramConfiguration);
}


void AS1130::setOnOffFrame(uint8_t frameIndex, const AS1130Picture12x11 &picture, uint8_t pwmSetIndex)
{
  // Prepare all register bytes.
  const uint8_t registerDataSize = 0x18;
  uint8_t registerData[registerDataSize];
  AS1130Picture12x11::writeRegisters(registerData, picture.getData(), pwmSetIndex);
  // Write the bytes
  const uint8_t frameAddress = (RS_OnOffFrame + frameIndex);
  writeToMemory(frameAddress, 0x00, registerData, registerDataSize);
}


void AS1130::setOnOffFrame(uint8_t frameIndex, const AS1130Picture24x5 &picture, uint8_t pwmSetIndex)
{
  // Prepare all register bytes.
  const uint8_t registerDataSize = 0x18;
  uint8_t registerData[registerDataSize];
  AS1130Picture24x5::writeRegisters(registerData, picture.getData(), pwmSetIndex);
  // Write the bytes
  const uint8_t frameAddress = (RS_OnOffFrame + frameIndex);
  writeToMemory(frameAddress, 0x00, registerData, registerDataSize);
}


void AS1130::setOnOffFrame24x5(uint8_t frameIndex, const uint8_t *data, uint8_t pwmSetIndex)
{
  // Prepare all register bytes.
  const uint8_t registerDataSize = 0x18;
  uint8_t registerData[registerDataSize];
  AS1130Picture24x5::writeRegisters(registerData, data, pwmSetIndex);
  // Write the bytes
  const uint8_t frameAddress = (RS_OnOffFrame + frameIndex);
  writeToMemory(frameAddress, 0x00, registerData, registerDataSize);
}


void AS1130::setOnOffFrame12x11(uint8_t frameIndex, const uint8_t *data, uint8_t pwmSetIndex)
{
  // Prepare all register bytes.
  const uint8_t registerDataSize = 0x18;
  uint8_t registerData[registerDataSize];
  AS1130Picture12x11::writeRegisters(registerData, data, pwmSetIndex);
  // Write the bytes
  const uint8_t frameAddress = (RS_OnOffFrame + frameIndex);
  writeToMemory(frameAddress, 0x00, registerData, registerDataSize);
}


void AS1130::setOnOffFrameAllOff(uint8_t frameIndex, uint8_t pwmSetIndex)
{
  const uint8_t frameAddress = (RS_OnOffFrame + frameIndex);
  // Prepare all frame bytes.
  const uint8_t registerDataSize = 0x18;
  uint8_t registerData[registerDataSize];
  std::memset(registerData, 0, registerDataSize);
  // Write the first segment with the PWM set index.
  registerData[1] = (pwmSetIndex<<5);
  // Send to chip.
  writeToMemory(frameAddress, 0x00, registerData, registerDataSize);
}


void AS1130::setOnOffFrameAllOn(uint8_t frameIndex, uint8_t pwmSetIndex)
{
  const uint8_t frameAddress = (RS_OnOffFrame + frameIndex);
  // Prepare all frame bytes.
  const uint8_t registerDataSize = 0x18;
  uint8_t registerData[registerDataSize];
  // Write the first segment with the PWM set index.
  registerData[0] = 0xff;
  registerData[1] = (pwmSetIndex<<5)|0x03;
  // Write all other segments 
  for (uint8_t i = 1; i < 12; ++i) {
    registerData[i*2] = 0xff;
    registerData[i*2+1] = 0x07;
  }
  // Send to chip.
  writeToMemory(frameAddress, 0x00, registerData, registerDataSize);
}


void AS1130::setBlinkAndPwmSetAll(uint8_t setIndex, bool doesBlink, uint8_t pwmValue)
{
  const uint8_t setAddress = (RS_BlinkAndPwmSet + setIndex);
  if (doesBlink) {
    fillMemory(setAddress, 0x00, 0xff, 24);
  } else {
    fillMemory(setAddress, 0x00, 0x00, 24);
  }
  // Set all PWM values to the maximum.
  fillMemory(setAddress, 0x18, pwmValue, 132);
}


void AS1130::setPwmValue(uint8_t setIndex, uint8_t ledIndex, uint8_t value)
{
  const uint8_t setAddress = (RS_BlinkAndPwmSet + setIndex);
  const uint8_t address = 0x18 + ((ledIndex>>4)*11) + (ledIndex&0xf);
  writeToMemory(setAddress, address, value);
}


uint8_t AS1130::getLedIndex24x5(uint8_t x, uint8_t y)
{
  return ((x>>1)*0x10) + ((x&1)*5) + y; 
}


uint8_t AS1130::getLedIndex12x11(uint8_t x, uint8_t y)
{
  return y+(x*0x10);
}


void AS1130::setDotCorrection(const uint8_t *data)
{
  for (uint8_t i = 0; i < 12; ++i) {
    writeToMemory(RS_DotCorrection, i, data[i]);
  }
}


void AS1130::setInterruptMask(uint8_t mask)
{
  writeControlRegister(CR_InterruptMask, mask);
}


void AS1130::setInterruptFrame(uint8_t lastFrame)
{
  writeControlRegister(CR_InterruptFrameDefinition, lastFrame);
}


void AS1130::setInterfaceMonitoring(uint8_t timeout, bool enabled)
{
  uint8_t data = 0;
  if (enabled) {
    data = 1;
  }
  data |= ((timeout & 0x3f) << 1);
  writeControlRegister(CR_InterfaceMonitoring, data);
}


void AS1130::setClockSynchronization(Synchronization synchronization, ClockFrequency clockFrequency)
{
  writeControlRegister(CR_ClockSynchronization, synchronization|clockFrequency);
}


void AS1130::setCurrentSource(Current current)
{
  writeControlRegister(CR_CurrentSource, current);
}


void AS1130::setScanLimit(ScanLimit scanLimit)
{
  writeControlRegisterBits(CR_DisplayOption, DOF_ScanLimitMask, scanLimit);
}


void AS1130::setBlinkEnabled(bool enabled)
{
  setOrClearControlRegisterBits(CR_MovieMode, MMF_BlinkEnabled, !enabled);
}


void AS1130::startPicture(uint8_t frameIndex, bool blinkAll)
{
  uint8_t data = PF_DisplayPicture;
  data |= (frameIndex & PF_PictureAddressMask);
  if (blinkAll) {
    data |= PF_BlinkPicture;
  }
  writeControlRegister(CR_Picture, data);
}


void AS1130::stopPicture()
{
  writeControlRegister(CR_Picture, 0x00);
}


void AS1130::setMovieEndFrame(MovieEndFrame movieEndFrame)
{
  setOrClearControlRegisterBits(CR_MovieMode, MMF_EndLast, movieEndFrame == MovieEndWithLastFrame);
}


void AS1130::setMovieFrameCount(uint8_t count)
{
  writeControlRegisterBits(CR_MovieMode, MMF_MovieFramesMask, count-1);
}


void AS1130::setFrameDelayMs(uint16_t delayMs)
{
  delayMs *= 10;
  delayMs /= 325;
  if (delayMs > 0x000f) {
    delayMs = 0x000f;
  }
  writeControlRegisterBits(CR_FrameTimeScroll, FTSF_FrameDelay, static_cast<uint8_t>(delayMs));
}


void AS1130::setScrollingEnabled(bool enable)
{
  setOrClearControlRegisterBits(CR_FrameTimeScroll, FTSF_EnableScrolling, enable);
}


void AS1130::setScrollingBlockSize(ScrollingBlockSize scrollingBlockSize)
{
  setOrClearControlRegisterBits(CR_FrameTimeScroll, FTSF_BlockSize, scrollingBlockSize == ScrollIn5LedBlocks);
}


void AS1130::setScrollingDirection(ScrollingDirection scrollingDirection)
{
  setOrClearControlRegisterBits(CR_FrameTimeScroll, FTSF_ScrollDirection, scrollingDirection == ScrollingLeft);
}


void AS1130::setFrameFadingEnabled(bool enable)
{
  setOrClearControlRegisterBits(CR_FrameTimeScroll, FTSF_FrameFade, enable);
}


void AS1130::setBlinkFrequency(BlinkFrequency blinkFrequency)
{
  setOrClearControlRegisterBits(CR_DisplayOption, DOF_BlinkFrequency, blinkFrequency == BlinkFrequency3s);
}


void AS1130::setMovieLoopCount(MovieLoopCount movieLoopCount)
{
  writeControlRegisterBits(CR_DisplayOption, DOF_LoopsMask, movieLoopCount);
}


void AS1130::startMovie(uint8_t firstFrameIndex, bool blinkAll)
{
  uint8_t data = MF_DisplayMovie;
  data |= (firstFrameIndex & MF_MovieAddressMask);
  if (blinkAll) {
    data |= MF_BlinkMovie;
  }
  writeControlRegister(CR_Movie, data);
}


void AS1130::stopMovie()
{
  writeControlRegister(CR_Movie, 0x00);
}


void AS1130::setLowVddResetEnabled(bool enabled)
{
  setOrClearControlRegisterBits(CR_Config, CF_LowVddReset, enabled);
}


void AS1130::setLowVddStatusEnabled(bool enabled)
{
  setOrClearControlRegisterBits(CR_Config, CF_LowVddStatus, enabled);
}


void AS1130::setLedErrorCorrectionEnabled(bool enabled)
{
  setOrClearControlRegisterBits(CR_Config, CF_LedErrorCorrection, enabled);
}


void AS1130::setDotCorrectionEnabled(bool enabled)
{
  setOrClearControlRegisterBits(CR_Config, CF_DotCorrection, enabled);
}


void AS1130::setTestAllLedsEnabled(bool enabled)
{
  setOrClearControlRegisterBits(CR_ShutdownAndOpenShort, SOSF_TestAll, enabled);
}


void AS1130::setAutomaticTestEnabled(bool enabled)
{
  setOrClearControlRegisterBits(CR_ShutdownAndOpenShort, SOSF_AutoTest, enabled);
}


void AS1130::startChip()
{
  setControlRegisterBits(CR_ShutdownAndOpenShort, SOSF_Shutdown);
}


void AS1130::stopChip()
{
  clearControlRegisterBits(CR_ShutdownAndOpenShort, SOSF_Shutdown);
}


void AS1130::resetChip()
{
  clearControlRegisterBits(CR_ShutdownAndOpenShort, SOSF_Initialize);
  delay(100);
}


void AS1130::runManualTest()
{
  setControlRegisterBits(CR_ShutdownAndOpenShort, SOSF_ManualTest);
  while (isLedTestRunning()) {
    delay(10);
  }
  clearControlRegisterBits(CR_ShutdownAndOpenShort, SOSF_ManualTest);
}


AS1130::LedStatus AS1130::getLedStatus(uint8_t ledIndex)
{
  if (ledIndex > 0xba) {
    return LedStatusDisabled;
  }
  if ((ledIndex & 0x0f) > 0xa) {
    return LedStatusDisabled;
  }
  const uint8_t ledBitMask = (1<<(ledIndex&0x7));
  const uint8_t registerIndex = CR_OpenLedBase + (ledIndex>>3);
  const uint8_t mask = readFromMemory(RS_Control, registerIndex);
  if ((mask & ledBitMask) == 0) {
    return LedStatusOpen;
  } else {
    return LedStatusOk;
  }
}


bool AS1130::isLedTestRunning()
{
  const uint8_t data = readControlRegister(CR_Status);
  return (data & SF_TestOn) != 0;
}


bool AS1130::isMovieRunning()
{
  const uint8_t data = readControlRegister(CR_Status);
  return (data & SF_MovieOn) != 0;
}


uint8_t AS1130::getDisplayedFrame()
{
  const uint8_t data = readControlRegister(CR_Status);
  return (data>>2);
}


uint8_t AS1130::getInterruptStatus()
{
  return readControlRegister(CR_InterruptStatus);
}


void AS1130::writeToChip(uint8_t address, uint8_t data)
{
  Wire.beginTransmission(_chipAddress); 
  Wire.write(address); 
  Wire.write(data); 
  Wire.endTransmission(); 
}


void AS1130::writeToMemory(uint8_t registerSelection, uint8_t address, uint8_t data)
{
  writeToChip(cRegisterSelectionAddress, registerSelection);
  writeToChip(address, data);
}


void AS1130::writeToMemory(uint8_t registerSelection, uint8_t address, const uint8_t *data, uint8_t size)
{
  writeToChip(cRegisterSelectionAddress, registerSelection);
  Wire.beginTransmission(_chipAddress);
  Wire.write(address); 
  for (uint8_t i = 0; i < size; ++i) {
    Wire.write(data[i]);
  }  
  Wire.endTransmission();   
}


void AS1130::fillMemory(uint8_t registerSelection, uint8_t address, uint8_t value, uint8_t size)
{
  writeToChip(cRegisterSelectionAddress, registerSelection);
  while (size > 0) {
    Wire.beginTransmission(_chipAddress);
    if (Wire.write(address) == 0) {
      // With failed address write, there is not much chance to do
      // anything useful, so this just finishes the transmission
      // and returns, leaving the error state set in the Wire
      // library.
      Wire.endTransmission();
      return;
    }

    // Send as much bytes as possible in one loop, by default the
    // Arduino Wire library has a 32 byte buffer, so we can send
    // a maximum of 31 data bytes at once (in addition to the
    // address byte).
    while (size > 0) {
      if (Wire.write(value) == 0) {
        Wire.clearWriteError();
        break;
      }
      size--;
      address++;
    }
    Wire.endTransmission();
  }
}


uint8_t AS1130::readFromMemory(uint8_t registerSelection, uint8_t address)
{
  writeToChip(cRegisterSelectionAddress, registerSelection);
  Wire.beginTransmission(_chipAddress);
  Wire.write(address);
  Wire.endTransmission();
  Wire.requestFrom(_chipAddress, 1);
  if (Wire.available() == 1) {
    const uint8_t data = Wire.read();
    return data;
  } else {
    return 0x00;
  }
}


void AS1130::writeControlRegister(ControlRegister controlRegister, uint8_t data)
{
  writeToMemory(RS_Control, controlRegister, data);
}


uint8_t AS1130::readControlRegister(ControlRegister controlRegister)
{
  return readFromMemory(RS_Control, controlRegister);
}


void AS1130::writeControlRegisterBits(ControlRegister controlRegister, uint8_t mask, uint8_t data)
{
  uint8_t registerData = readControlRegister(controlRegister);
  registerData &= (~mask);
  registerData |= (data & mask);
  writeControlRegister(controlRegister, registerData);
}


void AS1130::setControlRegisterBits(ControlRegister controlRegister, uint8_t mask)
{
  writeControlRegisterBits(controlRegister, mask, mask);
}


void AS1130::clearControlRegisterBits(ControlRegister controlRegister, uint8_t mask)
{
  writeControlRegisterBits(controlRegister, mask, 0);
}


void AS1130::setOrClearControlRegisterBits(ControlRegister controlRegister, uint8_t mask, bool setBits)
{
  if (setBits) {
    writeControlRegisterBits(controlRegister, mask, mask);
  } else {
    writeControlRegisterBits(controlRegister, mask, 0);
  }
}


}


