//
// Lucky Resistor's AS1130 Library
// ---------------------------------------------------------------------------
// (c)2016 by Lucky Resistor. See LICENSE for details.
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
#pragma once


#include <Wire.h>

#include <cinttypes>


namespace lr {


/// A low-level AS1130 chip access library.
///
/// You have to initialize the chip in exact this order:
/// 1. Set the RAM configuration.
/// 2. Set the On/Off frames with your data.
/// 3. Set the Blink&PWM sets with your data.
/// 4. Set the dot correction data (if required).
/// 5. Set control registers for interrupt masks, interface and clock synchronization.
/// 6. Set the current source level.
/// 7. Now set the display options like picture/movie etc.
/// 8. Start the display
///
class AS1130
{
public:
  /// The chip address.
  ///
  enum ChipAddress : uint8_t {
    ChipBaseAddress = 0b0110000,
    ChipAddress0 = ChipBaseAddress,
    ChipAddress1 = ChipBaseAddress+1,
    ChipAddress2 = ChipBaseAddress+2,
    ChipAddress3 = ChipBaseAddress+3,
    ChipAddress4 = ChipBaseAddress+4,
    ChipAddress5 = ChipBaseAddress+5,    
    ChipAddress6 = ChipBaseAddress+6,    
    ChipAddress7 = ChipBaseAddress+7,    
  };

  /// The RAM configuration.
  ///
  enum RamConfiguration : uint8_t {
    RamConfiguration1 = 1,
    RamConfiguration2 = 2,
    RamConfiguration3 = 3,
    RamConfiguration4 = 4,
    RamConfiguration5 = 5,
    RamConfiguration6 = 6,
  };

  /// The interrupt mask flags
  ///
  enum InterruptMaskFlag : uint8_t {
    IMF_MovieFinished   = 0b00000001,
    IMF_ShortTestError  = 0b00000010,
    IMF_OpenTestError   = 0b00000100,
    IMF_LowVdd          = 0b00001000,
    IMF_OverTemperature = 0b00010000,
    IMF_POR             = 0b00100000,
    IMF_WatchDog        = 0b01000000,
    IMF_SelectedPicture = 0b10000000,
  };

  /// The synchronization mode.
  ///
  enum Synchronization : uint8_t {
    SynchronizationOff      = 0b00,
    SynchronizationIn       = 0b01,
    SynchronizationOut      = 0b10,
  };

  /// The clock frequency.
  ///
  enum ClockFrequency : uint8_t {
    Clock1MHz   = 0b0000,
    Clock500kHz = 0b0100,
    Clock125kHz = 0b1000,
    Clock32kHz  = 0b1100
  };

  /// The current for the LEDs.
  ///
  enum Current : uint8_t {
    Current0mA  = 0x00,
    Current5mA  = 0x2b,
    Current10mA = 0x55,
    Current15mA = 0x80,
    Current20mA = 0xaa,
    Current25mA = 0xd5,
    Current30mA = 0xff
  };

  /// The scan limit.
  ///
  enum ScanLimit : uint8_t {
    ScanLimit1 = 0x0,
    ScanLimit2 = 0x1,
    ScanLimit3 = 0x2,
    ScanLimit4 = 0x3,
    ScanLimit5 = 0x4,
    ScanLimit6 = 0x5,
    ScanLimit7 = 0x6,
    ScanLimit8 = 0x7,
    ScanLimit9 = 0x8,
    ScanLimit10 = 0x9,
    ScanLimit11 = 0xa,
    ScanLimit12 = 0xb,
    ScanLimitFull = ScanLimit12,
  };

  /// The movie end frame
  ///
  enum MovieEndFrame : uint8_t {
    MovieEndWithFirstFrame,
    MovieEndWithLastFrame
  };

  /// The scrolling block size.
  ///
  enum ScrollingBlockSize : uint8_t {
    ScrollInFullMatrix,
    ScrollIn5LedBlocks
  };

  /// The scrolling direction.
  ///
  enum ScrollingDirection : uint8_t {
    ScrollingLeft,
    ScrollingRight
  };

  /// The blink frequency.
  ///
  enum BlinkFrequency : uint8_t {
    BlinkFrequency1_5s,
    BlinkFrequency3s
  };

  /// The movie loop count.
  ///
  enum MovieLoopCount : uint8_t {
    MovieLoopInvalid   = 0b00000000,
    MovieLoop1         = 0b00100000,
    MovieLoop2         = 0b01000000,
    MovieLoop3         = 0b01100000,
    MovieLoop4         = 0b10000000,
    MovieLoop5         = 0b10100000,
    MovieLoop6         = 0b11000000,
    MovieLoopEndless   = 0b11100000,
  };

public: // Low-level definitions.
  /// The register selection (base) value.
  ///
  enum RegisterSelection : uint8_t {
    RS_NOP = 0x00,
    RS_OnOffFrame = 0x01,
    RS_BlinkAndPwmSet = 0x40,
    RS_DotCorrection = 0x80,
    RS_Control = 0xc0
  };

  /// The control register address.
  ///
  enum ControlRegister : uint8_t {
    CR_Picture = 0x00,
    CR_Movie = 0x01,
    CR_MovieMode = 0x02,
    CR_FrameTimeScroll = 0x03,
    CR_DisplayOption = 0x04,
    CR_CurrentSource = 0x05,
    CR_Config = 0x06,
    CR_InterruptMask = 0x07,
    CR_InterruptFrameDefinition = 0x08,
    CR_ShutdownAndOpenShort = 0x09,
    CR_InterfaceMonitoring = 0x0a,
    CR_ClockSynchronization = 0x0b,
    CR_InterruptStatus = 0x0e,
    CR_Status = 0x0f,
    CR_OpenLedBase = 0x20
  };

  /// The flags and masks for the picture register.
  ///
  enum PictureFlag : uint8_t {
    PF_PictureAddressMask = 0b00111111,
    PF_DisplayPicture     = 0b01000000,
    PF_BlinkPicture       = 0b10000000,
  };

  /// The flags and masks for the movie register.
  ///
  enum MovieFlag : uint8_t {
    MF_MovieAddressMask   = 0b00111111,
    MF_DisplayMovie       = 0b01000000,
    MF_BlinkMovie         = 0b10000000,
  };

  /// The flags and masks for the movie mode register.
  ///
  enum MovieModeFlag : uint8_t {
    MMF_MovieFramesMask   = 0b00111111,
    MMF_EndLast           = 0b01000000,
    MMF_BlinkEnabled      = 0b10000000,
  };

  /// The flags and masks for the frame time/scroll register.
  ///
  enum FrameTimeScrollFlag : uint8_t {
    FTSF_FrameDelay       = 0b00001111,
    FTSF_EnableScrolling  = 0b00010000,
    FTSF_BlockSize        = 0b00100000,
    FTSF_ScrollDirection  = 0b01000000,
    FTSF_FrameFade        = 0b10000000,
  };

  /// The flags and masks for the display option register.
  ///
  enum DisplayOptionFlag : uint8_t {
    DOF_ScanLimitMask     = 0b00001111,
    DOF_BlinkFrequency    = 0b00010000,
    DOF_LoopsMask         = 0b11100000,
  };

  /// The flags and masks for the config register.
  ///
  enum ConfigFlag : uint8_t {
    CF_MemoryConfigMask   = 0b00000111,
    CF_CommonAddress      = 0b00001000,
    CF_DotCorrection      = 0b00010000,
    CF_LedErrorCorrection = 0b00100000,
    CF_LowVddStatus       = 0b01000000,
    CF_LowVddReset        = 0b10000000,
  };

  /// Flags/masks for the shutdown & open/short register
  ///
  enum ShutdownAndOpenShortFlag : uint8_t {
    SOSF_Shutdown   = 0b00000001,
    SOSF_Initialize = 0b00000010,
    SOSF_ManualTest = 0b00000100,
    SOSF_AutoTest   = 0b00001000,
    SOSF_TestAll    = 0b00010000, 
  };

public:
  /// Create a new driver instance
  ///
  /// @param chipAddress The address of the chip.
  ///
  AS1130(ChipAddress chipAddress = ChipAddress0);

public: // High-level functions.
  /// Check the chip communication.
  ///
  /// This function checks if the chip aknowledges a command on the I2C bus.
  /// If it does, this function returns true, otherwise it returns false.
  /// 
  bool isChipConnected();

  /// Set the RAM configuration.
  ///
  /// The RAM configuration defines how many On/Off frames and PWM/blink sets are available.
  /// This numbers are not checked if you define frames later, you have to make sure you
  /// only use the maximum number of frames and sets as possible for your choosen configuration.
  ///
  /// You can not change the configuration as soon as you wrote the first frame without resetting
  /// the chip.
  ///
  /// Ram Configuration | Blink & PWM Sets | On/Off Frames | On/Off Frames with Dot Connection
  /// ------------------+------------------+---------------+----------------------------------
  /// 1                 | 1                | 36            | 35
  /// 2                 | 2                | 30            | 29
  /// 3                 | 3                | 24            | 23
  /// 4                 | 4                | 18            | 17
  /// 5                 | 5                | 12            | 11
  /// 6                 | 6                | 6             | 5
  ///
  /// @param ramConfiguration The RAM configuration.
  ///
  void setRamConfiguration(RamConfiguration ramConfiguration);

  /// Set-up a on/off frame with data.
  ///
  /// This function is written for a 24x5 LED matrix. You have to specify 15 bytes of data.
  /// The bits are specified horizontally as shown in the example below.
  /// 
  /// Example array definition:
  /// const uint8_t exampleFrame[] = {
  ///   0b11111111, 0b11111111, 0b11111111,
  ///   0b10000000, 0b00000000, 0b00000001,
  ///   0b10000000, 0b00000000, 0b00000001,
  ///   0b10000000, 0b00000000, 0b00000001,
  ///   0b11111111, 0b11111111, 0b11111111};
  ///
  /// @param frameIndex The index of the frame. This has to be a value between 0 and 35. Depending 
  ///   on your RAM configuration this can be less. The frame number is not checked to be
  ///   valid. See the RAM configuration for details.
  /// @param pwmSetIndex The PWM set index for this frame. It has to a value between 0 and 7
  ///   selecting one of the PWM sets.
  /// @param data An array with 15 bytes. Each set bit will enable the corresponding LED.
  ///
  void setOnOffFrame24x5(uint8_t frameIndex, const uint8_t *data, uint8_t pwmSetIndex = 0);

  /// Set-up a on/off frame with all LEDs enabled.
  ///
  /// @param frameIndex The index of the frame. This has to be a value between 0 and 35.
  ///
  void setOnOffFrameAllOn(uint8_t frameIndex, uint8_t pwmSetIndex = 0);

  /// Set-up a blink&PWM set with values for all LEDs.
  ///
  /// This will set the given blink&PWM set and set all LEDs to the given values.
  ///
  /// @param setIndex The set index has to be a value between 0 and 5.
  /// @param doesBlink If the LEDs in the set are set to blink or not.
  /// @param pwmValue The PWM value for all LEDs.
  ///
  void setBlinkAndPwmSetAll(uint8_t setIndex, bool doesBlink = false, uint8_t pwmValue = 0xff);

  /// Set the dot correction data.
  ///
  /// This correction data is a correction factor for all 12 segments of the display.
  /// You have to pass an array with 12 byte values.
  ///
  void setDotCorrection(const uint8_t *data);

  /// Set the interrupt mask.
  ///
  /// @param mask A mask with a OR combination of the flags from enum InterruptMaskFlag.
  ///
  void setInterruptMask(uint8_t mask);

  /// Set the interrupt frame.
  ///
  /// @param lastFrame The index of the frame which triggers the inetrrupt. A value between 0 and 35.
  ///
  void setInterruptFrame(uint8_t lastFrame);

  /// Set the I2C monitoring.
  ///
  /// This sets the values for the interface monitoring.
  ///
  /// @param timeout The timeout value between 0x00 and 0x3f. The resulting timeout is calculated
  ///   with the following formula: timeout window = (value + 1) * 256 microseconds.
  /// @param enabled If set to true, the monitoring is enabled. If set to false, the monitoring is disabled.
  ///
  void setInterfaceMonitoring(uint8_t timeout, bool enabled);

  /// Set the clock synchronization.
  ///
  /// @param synchronization The synchronization mode.
  /// @param clockFrequency The clock frequency.
  ///
  void setClockSynchronization(Synchronization synchronization, ClockFrequency clockFrequency);

  /// Set the current source.
  ///
  /// This is the current source for all LEDs. It is not the actual current for each LED because of
  /// the multiplexing. See the datasheet, section "LED Current Calculation" for an exact calculation
  /// of the resulting current for each LED.
  ///
  /// @param current The current source for all LEDs.
  ///
  void setCurrentSource(Current current);

  /// Set the scan limit.
  ///
  void setScanLimit(ScanLimit scanLimit);

  /// Set if blinking is enabled or not for all modes.
  ///
  /// If you disable blinking using this flag, all blinking is disabled. It will ignore
  /// any bits set in the blink sets and the blink flags in the picture and movie modes.
  ///
  void setBlinkEnabled(bool enabled);

  /// Start displaying a picture
  ///
  /// @param frameIndex The index of the frame to display.
  /// @param blinkApp If all LEDs should blink while the picture is displayed.
  ///
  void startPicture(uint8_t frameIndex, bool blinkAll = false);

  /// Stop displaying a picture.
  ///
  void stopPicture();

  /// Sets at which frame the movie ends.
  ///
  /// @param movieEndFrame The frame where the movie ends.
  ///
  void setMovieEndFrame(MovieEndFrame movieEndFrame);

  /// Set the number of movie frames to play.
  ///
  /// @param count The number of movie frames to play. Minimum is 2, maximum is 36.
  ///   This value is converted into the register format.
  ///
  void setMovieFrameCount(uint8_t count);

  /// Set the frame delay.
  ///
  /// @param delayMs The frame delay in milliseconds. This value has to be between
  ///   zero and 488 milliseconds. The final value is changed to the next lower matching
  ///   value. See the datasheet for the final values.
  ///
  void setFrameDelayMs(uint16_t delayMs);

  /// Set scrolling enabled or disabled.
  ///
  /// @param enable True if the scrolling is enabled. False to disable the scrolling.
  ///
  void setScrollingEnabled(bool enable);

  /// Set the block size for scrolling.
  ///
  /// @param scrollingBlockSize If the scrolling uses the full matrix or the 5 LED block mode.
  ///
  void setScrollingBlockSize(ScrollingBlockSize scrollingBlockSize);

  /// Set the scroll direction.
  ///
  /// @param scrollingDirection The direction for the scrolling.
  ///
  void setScrollingDirection(ScrollingDirection scrollingDirection);

  /// Enable or disable frame fading.
  ///
  /// @param enable True to enable the frame fading. False to disable frame fading.
  ///
  void setFrameFadingEnabled(bool enable);

  /// Change the blink frequency.
  ///
  void setBlinkFrequency(BlinkFrequency blinkFrequency);

  /// Set the loop count for the movie.
  ///
  void setMovieLoopCount(MovieLoopCount movieLoopCount);

  /// Start displaying a movie
  ///
  /// @param firstFrameIndex The first frame of the movie to start with.
  /// @param blinkAll If all LEDs should blink while the movie is displayed.
  ///
  void startMovie(uint8_t firstFrameIndex, bool blinkAll = false);

  /// Stop displaying a movie
  ///
  void stopMovie();

  /// Start the chip.
  ///
  void startChip();

  /// Stop the chip.
  ///
  void stopChip();

public: // Low-level functions
  /// Write a two byte sequence to the chip.
  ///
  /// @param address The address byte.
  /// @param data The data byte.
  ///
  void writeToChip(uint8_t address, uint8_t data);

  /// Write a byte to a given memory location.
  ///
  /// @param registerSelection The register selection address.
  /// @param address The address of the register.
  /// @param data The data byte to write to the selected register.
  ///
  void writeToMemory(uint8_t registerSelection, uint8_t address, uint8_t data);

  /// Read a byte from a given memory location.
  ///
  /// @param registerSelection The register selection address.
  /// @param address The address of the register.
  ///
  uint8_t readFromMemory(uint8_t registerSelection, uint8_t address);  

  /// Write a byte to a control register.
  ///
  /// @param controlRegister The control register.
  /// @param data The data byte to write to the control register.
  ///
  void writeControlRegister(ControlRegister controlRegister, uint8_t data);

  /// Read a byte from a control register.
  ///
  /// @param controlRegister The control register to read a byte from.
  ///
  uint8_t readControlRegister(ControlRegister controlRegister);

  /// Write bits in a control register.
  ///
  /// @param controlRegister The control register to change.
  /// @param mask The mask for the bits. Only the bits set in this mask are changed.
  /// @param data The bits to set. The data is masked with the mask.
  ///
  void writeControlRegisterBits(ControlRegister controlRegister, uint8_t mask, uint8_t data);

  /// Set selected bits in a control register.
  ///
  /// @param controlRegister The control register to change.
  /// @param mask The mask for the bits to set.
  ///
  void setControlRegisterBits(ControlRegister controlRegister, uint8_t mask);

  /// Clear selected bits in a control register.
  ///
  /// @param controlRegister The control register to change.
  /// @param mask The mask for the bits to clear.
  ///  
  void clearControlRegisterBits(ControlRegister controlRegister, uint8_t mask);

private:
  uint8_t _chipAddress; ///< The selected address of the chip.
};

}


