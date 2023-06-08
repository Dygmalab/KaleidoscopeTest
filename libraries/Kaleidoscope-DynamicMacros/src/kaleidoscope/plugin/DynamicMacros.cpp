/* DynamicMacros - Dynamic macro support for Kaleidoscope.
 * Copyright (C) 2019, 2021  Keyboard.io, Inc.
 *
 * This program is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License as published by the Free Software
 * Foundation, version 3.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE. See the GNU General Public License for more
 * details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program. If not, see <http://www.gnu.org/licenses/>.
 */

#include "Kaleidoscope-DynamicMacros.h"
#include "Kaleidoscope-FocusSerial.h"
#include "kaleidoscope/keyswitch_state.h"
#include "kaleidoscope/key_events.h"

namespace kaleidoscope
{
  namespace plugin
  {

    uint16_t DynamicMacros::storage_base_;
    uint16_t DynamicMacros::storage_size_;
    uint16_t DynamicMacros::map_[];
    Key DynamicMacros::active_macro_keys_[];

    // =============================================================================
    // It might be possible to use Macros instead of reproducing it
    void DynamicMacros::press(Key key)
    {
      Runtime.handleKeyEvent(KeyEvent(KeyAddr::none(), IS_PRESSED | INJECTED, key));
      for (Key &mkey : active_macro_keys_)
      {
        if (mkey == Key_NoKey)
        {
          mkey = key;
          break;
        }
      }
      kaleidoscope::Runtime.hid().keyboard().sendReport();
    }

    void DynamicMacros::release(Key key)
    {
      for (Key &mkey : active_macro_keys_)
      {
        if (mkey == key)
        {
          mkey = Key_NoKey;
        }
      }
      Runtime.handleKeyEvent(KeyEvent(KeyAddr::none(), WAS_PRESSED | INJECTED, key));
      kaleidoscope::Runtime.hid().keyboard().sendReport();
    }

    void DynamicMacros::tap(Key key)
    {
      Runtime.handleKeyEvent(KeyEvent(KeyAddr::none(), IS_PRESSED | INJECTED, key));
      Runtime.handleKeyEvent(KeyEvent(KeyAddr::none(), WAS_PRESSED | INJECTED, key));
    }

void customDelay(uint16_t time)
    {
      int c = (int)time / 1000;
      int d = time % 1000;
      while (c >= 0)
      {
        if (c == 0)
        {
          delay(d);
          c -= 1;
        }
        else
        {
          delay(1000);
          c -= 1;
        }
#ifdef ARDUINO_SAMD_RAISE
        if (!WDT->STATUS.bit.SYNCBUSY) // Check if the WDT registers are synchronized
        {
          REG_WDT_CLEAR = WDT_CLEAR_CLEAR_KEY; // Clear the watchdog timer
        }
#endif
      }
    }

    void randomDelay(uint16_t randomA, uint16_t randomB)
    {
     int rnd = random(randomA, randomB);
      int c = (int)rnd / 1000;
      int d = rnd % 1000;
      while (c >= 0)
      {
        if (c == 0)
        {
          delay(d);
          c -= 1;
        }
        else
        {
          delay(1000);
          c -= 1;
        }
#ifdef ARDUINO_SAMD_RAISE
        if (!WDT->STATUS.bit.SYNCBUSY) // Check if the WDT registers are synchronized
        {
          REG_WDT_CLEAR = WDT_CLEAR_CLEAR_KEY; // Clear the watchdog timer
        }
#endif
      }
    }

    void DynamicMacros::updateDynamicMacroCache()
    {
      uint16_t pos = storage_base_;
      uint8_t current_id = 0;
      macro_t macro = MACRO_ACTION_END;
      bool previous_macro_ended = false;

      map_[0] = 0;

      while (pos < storage_base_ + storage_size_)
      {
        macro = Runtime.storage().read(pos++);
        switch (macro)
        {
        case MACRO_ACTION_STEP_EXPLICIT_REPORT:
        case MACRO_ACTION_STEP_IMPLICIT_REPORT:
        case MACRO_ACTION_STEP_SEND_REPORT:
          previous_macro_ended = false;
          break;

        case MACRO_ACTION_STEP_INTERVAL:
        case MACRO_ACTION_STEP_WAIT:
        case MACRO_ACTION_STEP_KEYCODEDOWN:
        case MACRO_ACTION_STEP_KEYCODEUP:
        case MACRO_ACTION_STEP_TAPCODE:
          previous_macro_ended = false;
          pos++;
          break;

        case MACRO_ACTION_STEP_KEYDOWN:
        case MACRO_ACTION_STEP_KEYUP:
        case MACRO_ACTION_STEP_TAP:
          previous_macro_ended = false;
          pos += 2;
          break;

        case MACRO_ACTION_STEP_TAP_SEQUENCE:
        {
          previous_macro_ended = false;
          uint8_t keyCode, flags;
          do
          {
            flags = Runtime.storage().read(pos++);
            keyCode = Runtime.storage().read(pos++);
          } while (!(flags == 0 && keyCode == 0));
          break;
        }

        case MACRO_ACTION_STEP_TAP_CODE_SEQUENCE:
        {
          previous_macro_ended = false;
          uint8_t keyCode, flags;
          do
          {
            keyCode = Runtime.storage().read(pos++);
          } while (keyCode != 0);
          break;
        }

        case MACRO_ACTION_END:
          map_[++current_id] = pos - storage_base_;

          if (previous_macro_ended)
            return;

          previous_macro_ended = true;
          break;
        }
      }
    }

    // public
    void DynamicMacros::play(uint8_t macro_id)
    {
      macro_t macro = MACRO_ACTION_END;
      uint16_t randomA, randomB;
      uint16_t pos, interval = 0;
      Key key;

      pos = storage_base_ + map_[macro_id];

      while (true)
      {
        switch (macro = Runtime.storage().read(pos++))
        {
        case MACRO_ACTION_STEP_EXPLICIT_REPORT:
        case MACRO_ACTION_STEP_IMPLICIT_REPORT:
        case MACRO_ACTION_STEP_SEND_REPORT:
          break;

        case MACRO_ACTION_STEP_INTERVAL:
        {
          uint8_t rnd1 = Runtime.storage().read(pos++);
          uint8_t rnd2 = Runtime.storage().read(pos++);
          uint8_t rnd3 = Runtime.storage().read(pos++);
          uint8_t rnd4 = Runtime.storage().read(pos++);
          randomA = (rnd1 << 8) | rnd2;
          randomB = (rnd3 << 8) | rnd4;
          randomDelay(randomA, randomB);
          break;
        }
        case MACRO_ACTION_STEP_WAIT:
        {
          uint8_t d2 = Runtime.storage().read(pos++);
          uint8_t d1 = Runtime.storage().read(pos++);
          uint16_t wait = (d2 << 8) | d1;
          customDelay(wait);
          break;
        }

        case MACRO_ACTION_STEP_KEYDOWN:
          key.setFlags(Runtime.storage().read(pos++));
          key.setKeyCode(Runtime.storage().read(pos++));
          press(key);
          break;
        case MACRO_ACTION_STEP_KEYUP:
          key.setFlags(Runtime.storage().read(pos++));
          key.setKeyCode(Runtime.storage().read(pos++));
          release(key);
          break;
        case MACRO_ACTION_STEP_TAP:
          key.setFlags(Runtime.storage().read(pos++));
          key.setKeyCode(Runtime.storage().read(pos++));
          tap(key);
          break;

        case MACRO_ACTION_STEP_KEYCODEDOWN:
          key.setFlags(0);
          key.setKeyCode(Runtime.storage().read(pos++));
          press(key);
          break;
        case MACRO_ACTION_STEP_KEYCODEUP:
          key.setFlags(0);
          key.setKeyCode(Runtime.storage().read(pos++));
          release(key);
          break;
        case MACRO_ACTION_STEP_TAPCODE:
          key.setFlags(0);
          key.setKeyCode(Runtime.storage().read(pos++));
          tap(key);
          break;

        case MACRO_ACTION_STEP_TAP_SEQUENCE:
        {
          while (true)
          {
            key.setFlags(0);
            key.setKeyCode(pgm_read_byte(pos++));
            if (key == Key_NoKey)
              break;
            tap(key);
            customDelay(interval);
          }
          break;
        }
        case MACRO_ACTION_STEP_TAP_CODE_SEQUENCE:
        {
          while (true)
          {
            key.setFlags(0);
            key.setKeyCode(pgm_read_byte(pos++));
            if (key.getKeyCode() == 0)
              break;
            tap(key);
            customDelay(interval);
          }
          break;
        }

        case MACRO_ACTION_END:
        {
          return;
        }
        default:
        {
            return;
        }
        }

        customDelay(interval);
#ifdef ARDUINO_SAMD_RAISE
        if (!WDT->STATUS.bit.SYNCBUSY) // Check if the WDT registers are synchronized
        {
          REG_WDT_CLEAR = WDT_CLEAR_CLEAR_KEY; // Clear the watchdog timer
        }
#endif
      }
    }

    bool isDynamicMacrosKey(Key key)
    {
      return (key.getRaw() >= ranges::DYNAMIC_MACRO_FIRST &&
              key.getRaw() <= ranges::DYNAMIC_MACRO_LAST);
    }

    EventHandlerResult DynamicMacros::beforeReportingState(const KeyEvent &event) {
      // Do this in beforeReportingState(), instead of `onAddToReport()` because
      // `live_keys` won't get updated until after the macro sequence is played from
      // the keypress. This could be changed by either updating `live_keys` manually
      // ahead of time, or by executing the macro sequence on key release instead of
      // key press. This is probably the simplest solution.
      for (Key key : active_macro_keys_) {
        if (key != Key_NoKey)
          Runtime.addToReport(key);
      }
      return EventHandlerResult::OK;
    }

    // -----------------------------------------------------------------------------
    EventHandlerResult DynamicMacros::onKeyEvent(KeyEvent &event)
    {
      if (!isDynamicMacrosKey(event.key))
        return EventHandlerResult::OK;

      if (keyToggledOn(event.state))
      {
        uint8_t macro_id = event.key.getRaw() - ranges::DYNAMIC_MACRO_FIRST;
        play(macro_id);
      }
      else
      {
        for (Key key : active_macro_keys_)
        {
          release(key);
        }
      }

      return EventHandlerResult::EVENT_CONSUMED;
    }

    EventHandlerResult DynamicMacros::onNameQuery()
    {
      return ::Focus.sendName(F("DynamicMacros"));
    }

    EventHandlerResult DynamicMacros::onFocusEvent(const char *command)
    {
      if (::Focus.handleHelp(command, PSTR("macros.map\nmacros.trigger\nmacros.memory")))
        return EventHandlerResult::OK;

      if (strncmp_P(command, PSTR("macros."), 7) != 0)
        return EventHandlerResult::OK;

      if (strcmp_P(command + 7, PSTR("map")) == 0)
      {
        if (::Focus.isEOL())
        {
          for (uint16_t i = 0; i < storage_size_; i++)
          {
            uint8_t b;
            b = Runtime.storage().read(storage_base_ + i);
            ::Focus.send(b);
          }
        }
        else
        {
          uint16_t pos = 0;

          while (!::Focus.isEOL())
          {
            uint8_t b;
            ::Focus.read(b);

            Runtime.storage().update(storage_base_ + pos++, b);
          }
          Runtime.storage().commit();
          updateDynamicMacroCache();
        }
      }

      if (strcmp_P(command + 7, PSTR("trigger")) == 0)
      {
        uint8_t id = 0;
        ::Focus.read(id);
        play(id);
      }

      if (strcmp_P(command + 7, PSTR("memory")) == 0)
      {
        if (::Focus.isEOL())
        {
          ::Focus.send(storage_size_);
        }
      }

      return EventHandlerResult::EVENT_CONSUMED;
    }

    // public
    void DynamicMacros::reserve_storage(uint16_t size)
    {
      storage_base_ = ::EEPROMSettings.requestSlice(size);
      storage_size_ = size;
      updateDynamicMacroCache();
    }

  } // namespace plugin
} // namespace kaleidoscope

kaleidoscope::plugin::DynamicMacros DynamicMacros;
