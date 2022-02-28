## Issue description

The problem is related to the adaptation of an Arduino library that handles HID events to emulate a keyboard with multiple functions to a different board.

The library is OpenSoftware, its implemented by a company called Keyboardio for AVR chips and its directed to custom keyboards built to be modular and mod-able by the users, but we use it with a SAMD board. They updated the whole library structure and to adapt our custom modifications we need to make changes to the arduino compiler similar to the ones they made.

The exact problem is happening on the code from the library posted in the following repo (main branch):

[https://github.com/Dygmalab/KaleidoscopeTest](https://github.com/Dygmalab/KaleidoscopeTest) 

when used from the following Arduino IDE project (stableRelease Branch):

[https://github.com/Dygmalab/Raise-Firmware](https://github.com/Dygmalab/Raise-Firmware)

The error is related to the usage of the KaleidoscopeHID library, as it’s wrapped in a wrapper class to be optimized out in case of not being needed, this is the part that’s not working properly. it’s being always left out and thus the firmware doesn’t work properly afterward (or it doesn't even compile, with the error below).

```bash
Linking everything together...
"C:\\Users\\Dygma\\AppData\\Local\\Arduino15\\packages\\arduino\\tools\\arm-none-eabi-gcc\\7-2017q4/bin/arm-none-eabi-g++" "-LC:\\Users\\Dygma\\AppData\\Local\\Temp\\arduino_build_573511" -Os -Wl,--gc-sections -save-temps "-TC:\\Users\\Dygma\\Documents\\Arduino\\hardware\\dygmaTest\\samd\\variants\\arduino_zero/linker_scripts/gcc/flash_with_bootloader.ld" "-Wl,-Map,C:\\Users\\Dygma\\AppData\\Local\\Temp\\arduino_build_573511/Raise-Firmware.ino.map" --specs=nano.specs --specs=nosys.specs -mcpu=cortex-m0plus -mthumb -Wl,--cref -Wl,--check-sections -Wl,--gc-sections -Wl,--unresolved-symbols=report-all -Wl,--warn-common -Wl,--warn-section-align -o "C:\\Users\\Dygma\\AppData\\Local\\Temp\\arduino_build_573511/Raise-Firmware.ino.elf" "C:\\Users\\Dygma\\AppData\\Local\\Temp\\arduino_build_573511\\sketch\\EEPROMPadding.cpp.o" "C:\\Users\\Dygma\\AppData\\Local\\Temp\\arduino_build_573511\\sketch\\EEPROMUpgrade.cpp.o" "C:\\Users\\Dygma\\AppData\\Local\\Temp\\arduino_build_573511\\sketch\\LED-CapsLockLight.cpp.o" "C:\\Users\\Dygma\\AppData\\Local\\Temp\\arduino_build_573511\\sketch\\Raise-Firmware.ino.cpp.o" "C:\\Users\\Dygma\\AppData\\Local\\Temp\\arduino_build_573511\\sketch\\RaiseFirmwareVersion.cpp.o" "C:\\Users\\Dygma\\AppData\\Local\\Temp\\arduino_build_573511\\sketch\\RaiseIdleLEDs.cpp.o" "C:\\Users\\Dygma\\AppData\\Local\\Temp\\arduino_build_573511\\sketch\\attiny_firmware.cpp.o" "C:\\Users\\Dygma\\AppData\\Local\\Temp\\arduino_build_573511\\libraries\\Kaleidoscope\\Kaleidoscope.a" "C:\\Users\\Dygma\\AppData\\Local\\Temp\\arduino_build_573511\\libraries\\Kaleidoscope-Hardware-Dygma-Raise\\kaleidoscope\\device\\dygma\\Raise.cpp.o" "C:\\Users\\Dygma\\AppData\\Local\\Temp\\arduino_build_573511\\libraries\\Kaleidoscope-Hardware-Dygma-Raise\\kaleidoscope\\device\\dygma\\raise\\Focus.cpp.o" "C:\\Users\\Dygma\\AppData\\Local\\Temp\\arduino_build_573511\\libraries\\Kaleidoscope-Hardware-Dygma-Raise\\kaleidoscope\\device\\dygma\\raise\\RaiseSide.cpp.o" "C:\\Users\\Dygma\\AppData\\Local\\Temp\\arduino_build_573511\\libraries\\Kaleidoscope-Hardware-Dygma-Raise\\kaleidoscope\\device\\dygma\\raise\\TWI.cpp.o" "C:\\Users\\Dygma\\AppData\\Local\\Temp\\arduino_build_573511\\libraries\\FlashStorage\\FlashAsEEPROM.cpp.o" "C:\\Users\\Dygma\\AppData\\Local\\Temp\\arduino_build_573511\\libraries\\FlashStorage\\FlashStorage.cpp.o" "C:\\Users\\Dygma\\AppData\\Local\\Temp\\arduino_build_573511\\libraries\\KeyboardioHID\\KeyboardioHID.a" "C:\\Users\\Dygma\\AppData\\Local\\Temp\\arduino_build_573511\\libraries\\Wire\\Wire.cpp.o" "C:\\Users\\Dygma\\AppData\\Local\\Temp\\arduino_build_573511\\libraries\\Kaleidoscope-MouseKeys\\kaleidoscope\\plugin\\MouseKeys.cpp.o" "C:\\Users\\Dygma\\AppData\\Local\\Temp\\arduino_build_573511\\libraries\\Kaleidoscope-MouseKeys\\kaleidoscope\\plugin\\MouseKeys\\MouseWrapper.cpp.o" "C:\\Users\\Dygma\\AppData\\Local\\Temp\\arduino_build_573511\\libraries\\Kaleidoscope-EEPROM-Settings\\kaleidoscope\\plugin\\EEPROM-Settings.cpp.o" "C:\\Users\\Dygma\\AppData\\Local\\Temp\\arduino_build_573511\\libraries\\Kaleidoscope-EEPROM-Settings\\kaleidoscope\\plugin\\EEPROM-Settings\\crc.cpp.o" "C:\\Users\\Dygma\\AppData\\Local\\Temp\\arduino_build_573511\\libraries\\Kaleidoscope-PersistentLEDMode\\kaleidoscope\\plugin\\PersistentLEDMode.cpp.o" "C:\\Users\\Dygma\\AppData\\Local\\Temp\\arduino_build_573511\\libraries\\Kaleidoscope-FocusSerial\\kaleidoscope\\plugin\\FocusSerial.cpp.o" "C:\\Users\\Dygma\\AppData\\Local\\Temp\\arduino_build_573511\\libraries\\Kaleidoscope-EEPROM-Keymap\\kaleidoscope\\plugin\\EEPROM-Keymap.cpp.o" "C:\\Users\\Dygma\\AppData\\Local\\Temp\\arduino_build_573511\\libraries\\Kaleidoscope-IdleLEDs\\kaleidoscope\\plugin\\IdleLEDs.cpp.o" "C:\\Users\\Dygma\\AppData\\Local\\Temp\\arduino_build_573511\\libraries\\Kaleidoscope-Colormap\\kaleidoscope\\plugin\\Colormap.cpp.o" "C:\\Users\\Dygma\\AppData\\Local\\Temp\\arduino_build_573511\\libraries\\Kaleidoscope-LED-Palette-Theme\\kaleidoscope\\plugin\\LED-Palette-Theme.cpp.o" "C:\\Users\\Dygma\\AppData\\Local\\Temp\\arduino_build_573511\\libraries\\Kaleidoscope-LEDEffect-Rainbow\\kaleidoscope\\plugin\\LEDEffect-Rainbow.cpp.o" "C:\\Users\\Dygma\\AppData\\Local\\Temp\\arduino_build_573511\\libraries\\Kaleidoscope-LED-Stalker\\kaleidoscope\\plugin\\LED-Stalker.cpp.o" "C:\\Users\\Dygma\\AppData\\Local\\Temp\\arduino_build_573511\\libraries\\Kaleidoscope-LED-Wavepool\\kaleidoscope\\plugin\\LED-Wavepool.cpp.o" "C:\\Users\\Dygma\\AppData\\Local\\Temp\\arduino_build_573511\\libraries\\Kaleidoscope-Heatmap\\kaleidoscope\\plugin\\Heatmap.cpp.o" "C:\\Users\\Dygma\\AppData\\Local\\Temp\\arduino_build_573511\\libraries\\Kaleidoscope-LEDEffect-DigitalRain\\Kaleidoscope-LEDEffect-DigitalRain.a" "C:\\Users\\Dygma\\AppData\\Local\\Temp\\arduino_build_573511\\libraries\\Kaleidoscope-DynamicMacros\\kaleidoscope\\plugin\\DynamicMacros.cpp.o" "C:\\Users\\Dygma\\AppData\\Local\\Temp\\arduino_build_573511\\libraries\\Kaleidoscope-Macros\\kaleidoscope\\plugin\\Macros.cpp.o" "C:\\Users\\Dygma\\AppData\\Local\\Temp\\arduino_build_573511\\libraries\\Kaleidoscope-MagicCombo\\kaleidoscope\\plugin\\MagicCombo.cpp.o" "C:\\Users\\Dygma\\AppData\\Local\\Temp\\arduino_build_573511\\libraries\\Kaleidoscope-USB-Quirks\\kaleidoscope\\plugin\\USB-Quirks.cpp.o" "C:\\Users\\Dygma\\AppData\\Local\\Temp\\arduino_build_573511\\libraries\\Kaleidoscope-LayerFocus\\kaleidoscope\\plugin\\LayerFocus.cpp.o" "C:\\Users\\Dygma\\AppData\\Local\\Temp\\arduino_build_573511\\libraries\\Kaleidoscope-LEDEffect-BootGreeting\\kaleidoscope\\plugin\\LEDEffect-BootGreeting.cpp.o" "C:\\Users\\Dygma\\AppData\\Local\\Temp\\arduino_build_573511\\libraries\\Kaleidoscope-HostPowerManagement\\kaleidoscope\\plugin\\HostPowerManagement.cpp.o" "C:\\Users\\Dygma\\AppData\\Local\\Temp\\arduino_build_573511\\libraries\\Kaleidoscope-OneShot\\kaleidoscope\\plugin\\OneShot.cpp.o" "C:\\Users\\Dygma\\AppData\\Local\\Temp\\arduino_build_573511\\libraries\\Kaleidoscope-Qukeys\\kaleidoscope\\plugin\\Qukeys.cpp.o" "C:\\Users\\Dygma\\AppData\\Local\\Temp\\arduino_build_573511\\libraries\\Kaleidoscope-Escape-OneShot\\kaleidoscope\\plugin\\Escape-OneShot-Config.cpp.o" "C:\\Users\\Dygma\\AppData\\Local\\Temp\\arduino_build_573511\\libraries\\Kaleidoscope-Escape-OneShot\\kaleidoscope\\plugin\\Escape-OneShot.cpp.o" "C:\\Users\\Dygma\\AppData\\Local\\Temp\\arduino_build_573511\\core\\variant.cpp.o" -Wl,--start-group "-LC:\\Users\\Dygma\\AppData\\Local\\Arduino15\\packages\\arduino\\tools\\CMSIS\\4.5.0/CMSIS/Lib/GCC/" -larm_cortexM0l_math -lm "C:\\Users\\Dygma\\AppData\\Local\\Temp\\arduino_build_573511/core\\core.a" -Wl,--end-group
C:\Users\Dygma\AppData\Local\Temp\arduino_build_573511\libraries\Kaleidoscope-MouseKeys\kaleidoscope\plugin\MouseKeys.cpp.o: In function `kaleidoscope::plugin::MouseKeys_::onSetup()':
C:\Users\Dygma\Documents\Arduino\hardware\dygmaTest\samd\libraries\Kaleidoscope\src/kaleidoscope/driver/hid/keyboardio/Mouse.h:45: undefined reference to `Mouse_::begin()'
C:\Users\Dygma\AppData\Local\Temp\arduino_build_573511\libraries\Kaleidoscope-MouseKeys\kaleidoscope\plugin\MouseKeys.cpp.o: In function `kaleidoscope::plugin::MouseKeys_::onSetup()':
C:\Users\Dygma\Documents\Arduino\hardware\dygmaTest\samd\libraries\Kaleidoscope-MouseKeys\src\kaleidoscope\plugin/MouseKeys.cpp:149: undefined reference to `Mouse'
C:\Users\Dygma\Documents\Arduino\hardware\dygmaTest\samd\libraries\Kaleidoscope-MouseKeys\src\kaleidoscope\plugin/MouseKeys.cpp:149: undefined reference to `SingleAbsoluteMouse'
C:\Users\Dygma\Documents\Arduino\hardware\dygmaTest\samd\libraries\Kaleidoscope-MouseKeys\src\kaleidoscope\plugin/MouseKeys.cpp:149: undefined reference to `SingleAbsoluteMouse'
C:\Users\Dygma\AppData\Local\Temp\arduino_build_573511\libraries\Kaleidoscope-MouseKeys\kaleidoscope\plugin\MouseKeys.cpp.o: In function `kaleidoscope::plugin::MouseKeys_::afterEachCycle()':
C:\Users\Dygma\Documents\Arduino\hardware\dygmaTest\samd\libraries\Kaleidoscope\src/kaleidoscope/driver/hid/keyboardio/Mouse.h:51: undefined reference to `Mouse_::move(signed char, signed char, signed char, signed char)'
C:\Users\Dygma\Documents\Arduino\hardware\dygmaTest\samd\libraries\Kaleidoscope\src/kaleidoscope/driver/hid/keyboardio/Mouse.h:48: undefined reference to `Mouse_::sendReport()'
C:\Users\Dygma\Documents\Arduino\hardware\dygmaTest\samd\libraries\Kaleidoscope\src/kaleidoscope/driver/hid/keyboardio/Mouse.h:48: undefined reference to `Mouse_::sendReport()'
C:\Users\Dygma\Documents\Arduino\hardware\dygmaTest\samd\libraries\Kaleidoscope\src/kaleidoscope/driver/hid/keyboardio/Mouse.h:48: undefined reference to `Mouse'
C:\Users\Dygma\AppData\Local\Temp\arduino_build_573511\libraries\Kaleidoscope-MouseKeys\kaleidoscope\plugin\MouseKeys.cpp.o: In function `kaleidoscope::plugin::MouseKeys_::sendMouseButtonReport(kaleidoscope::KeyEvent const&) const':
C:\Users\Dygma\Documents\Arduino\hardware\dygmaTest\samd\libraries\Kaleidoscope\src/kaleidoscope/driver/hid/keyboardio/Mouse.h:68: undefined reference to `Mouse_::releaseAll()'
C:\Users\Dygma\Documents\Arduino\hardware\dygmaTest\samd\libraries\Kaleidoscope\src/kaleidoscope/driver/hid/keyboardio/Mouse.h:71: undefined reference to `Mouse_::press(unsigned char)'
C:\Users\Dygma\Documents\Arduino\hardware\dygmaTest\samd\libraries\Kaleidoscope\src/kaleidoscope/driver/hid/keyboardio/Mouse.h:48: undefined reference to `Mouse_::sendReport()'
C:\Users\Dygma\AppData\Local\Temp\arduino_build_573511\libraries\Kaleidoscope-MouseKeys\kaleidoscope\plugin\MouseKeys.cpp.o: In function `kaleidoscope::plugin::MouseKeys_::sendMouseButtonReport(kaleidoscope::KeyEvent const&) const':
C:\Users\Dygma\Documents\Arduino\hardware\dygmaTest\samd\libraries\Kaleidoscope-MouseKeys\src\kaleidoscope\plugin/MouseKeys.cpp:291: undefined reference to `Mouse'
C:\Users\Dygma\AppData\Local\Temp\arduino_build_573511\libraries\Kaleidoscope-MouseKeys\kaleidoscope\plugin\MouseKeys\MouseWrapper.cpp.o: In function `kaleidoscope::plugin::MouseWrapper_::warp_jump(unsigned short, unsigned short, unsigned short, unsigned short)':
C:\Users\Dygma\Documents\Arduino\hardware\dygmaTest\samd\libraries\Kaleidoscope-MouseKeys\src\kaleidoscope\plugin\MouseKeys/MouseWrapper.cpp:42: undefined reference to `SingleAbsoluteMouse'
C:\Users\Dygma\Documents\Arduino\hardware\dygmaTest\samd\libraries\Kaleidoscope-MouseKeys\src\kaleidoscope\plugin\MouseKeys/MouseWrapper.cpp:42: undefined reference to `SingleAbsoluteMouse'
C:\Users\Dygma\AppData\Local\Temp\arduino_build_573511\libraries\Kaleidoscope-MouseKeys\kaleidoscope\plugin\MouseKeys\MouseWrapper.cpp.o: In function `kaleidoscope::plugin::MouseWrapper_::move(signed char, signed char)':
C:\Users\Dygma\Documents\Arduino\hardware\dygmaTest\samd\libraries\Kaleidoscope\src/kaleidoscope/driver/hid/keyboardio/Mouse.h:51: undefined reference to `Mouse_::move(signed char, signed char, signed char, signed char)'
C:\Users\Dygma\AppData\Local\Temp\arduino_build_573511\libraries\Kaleidoscope-MouseKeys\kaleidoscope\plugin\MouseKeys\MouseWrapper.cpp.o: In function `kaleidoscope::plugin::MouseWrapper_::move(signed char, signed char)':
C:\Users\Dygma\Documents\Arduino\hardware\dygmaTest\samd\libraries\Kaleidoscope-MouseKeys\src\kaleidoscope\plugin\MouseKeys/MouseWrapper.cpp:138: undefined reference to `Mouse'
collect2.exe: error: ld returned 1 exit status
```

## Task ahead

Configure the compiler so that all the files are properly linked and the wrappers do their job.

You have specific comments from the authors of the library in the platforms.txt file, recipe section, on which you can find the modifications they made to the AVR Arduino compiler configuration to make it work with this file structure

```bash
################################################################################
## PLEASE NOTE: The way we link our elf binary significantly differs from the 
##              way that Arduino handles things by default. 
##              The original linker command has been split up into three 
##              steps (recipe.c.combine.pattern, recipe.hooks.linking.postlink.1.pattern
##              and recipe.hooks.linking.postlink.2.pattern).
##              This is necessary to prevent link errors reporting unresolved symbols
##              due to order dependencies of libraries and objects appearing
##              in the linker command line.
##
## CHANGED WRT DEFAULT: Remove a pre-existing joined library. This is necessary if the library was not
##                    removed at the end of a previous build, because the build process
##                    terminated early after a failed attempt to link (e.g. because of missing symbols).
recipe.hooks.linking.prelink.1.pattern={tools.rm_start.cmd} "{build.path}/{build.project_name}_joined.a" {tools.rm_end.cmd}
##              
## CHANGED WRT DEFAULT: Generate a large .a archive to prevent link order issues with garbage collection
recipe.c.combine.pattern="{compiler.path}/{compiler.ar.cmd}" {compiler.c.elf.flags_join_archives} "{build.path}/{build.project_name}_joined.a" {object_files} "{build.path}/{archive_file}"
##
## NEWLY INTRODUCED: Link with global garbage collection (considering all
##                   objects and libraries together).
recipe.hooks.linking.postlink.1.pattern={compiler.wrapper.cmd} "{compiler.path}{compiler.c.elf.cmd}" {compiler.c.elf.flags} -mcpu={build.mcu} {compiler.c.elf.extra_flags} -o "{build.path}/{build.project_name}.elf" "{build.path}/{build.project_name}_joined.a" "-L{build.path}" -lm
##
## NEWLY INTRODUCED: Removing the joined library is required to avoid malformed archives that
##                   would otherwise result when updating a pre-existing
##                   joined archive file during a subsequent firmware build.
##
recipe.hooks.linking.postlink.2.pattern={tools.rm_start.cmd} "{build.path}/{build.project_name}_joined.a" {tools.rm_end.cmd}
################################################################################
```

What we need is a proper compilation mechanism that works for SAMD MCU’s (which is our use case) following the procedure they do for AVR (creating a linked file first, then using it afterwards, etc... )
