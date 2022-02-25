/* Kaleidoscope-MouseKeys - Mouse keys for Kaleidoscope.
 * Copyright (C) 2017-2021  Keyboard.io, Inc.
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

#include <Arduino.h>

#include "kaleidoscope/Runtime.h"
#include "Kaleidoscope-MouseKeys.h"
#include "Kaleidoscope-FocusSerial.h"
#include "kaleidoscope/keyswitch_state.h"

namespace kaleidoscope {
namespace plugin {

uint8_t MouseKeys_::speed = 1;
uint16_t MouseKeys_::speedDelay = 1;

uint8_t MouseKeys_::accelSpeed = 1;
uint16_t MouseKeys_::accelDelay = 64;

uint8_t MouseKeys_::wheelSpeed = 1;
uint16_t MouseKeys_::wheelDelay = 50;

uint16_t MouseKeys_::move_start_time_;
uint16_t MouseKeys_::accel_start_time_;
uint16_t MouseKeys_::wheel_start_time_;
uint16_t MouseKeys_::storage_base_;

// =============================================================================
// Configuration functions

void MouseKeys_::setWarpGridSize(uint8_t grid_size) {
  MouseWrapper.warp_grid_size = grid_size;
}

void MouseKeys_::setSpeedLimit(uint8_t speed_limit) {
  MouseWrapper.speedLimit = speed_limit;
}

// =============================================================================
// Key variant tests

bool MouseKeys_::isMouseKey(const Key& key) const {
  return (key.getFlags() == (SYNTHETIC | IS_MOUSE_KEY));
}

bool MouseKeys_::isMouseButtonKey(const Key& key) const {
  uint8_t variant = key.getKeyCode() & (KEY_MOUSE_BUTTON | KEY_MOUSE_WARP);
  return variant == KEY_MOUSE_BUTTON;
}

bool MouseKeys_::isMouseMoveKey(const Key& key) const {
  uint8_t mask = (KEY_MOUSE_BUTTON | KEY_MOUSE_WARP | KEY_MOUSE_WHEEL);
  uint8_t variant = key.getKeyCode() & mask;
  return variant == 0;
}

bool MouseKeys_::isMouseWarpKey(const Key& key) const {
  return (key.getKeyCode() & KEY_MOUSE_WARP) != 0;
}

bool MouseKeys_::isMouseWheelKey(const Key& key) const {
  uint8_t mask = (KEY_MOUSE_BUTTON | KEY_MOUSE_WARP | KEY_MOUSE_WHEEL);
  uint8_t variant = key.getKeyCode() & mask;
  return variant == KEY_MOUSE_WHEEL;
}

// =============================================================================
// Event Handlers

// -----------------------------------------------------------------------------
EventHandlerResult MouseKeys_::onNameQuery() {
  return ::Focus.sendName(F("MouseKeys"));
}

// -----------------------------------------------------------------------------
EventHandlerResult MouseKeys_::onSetup(void) {
  Runtime.hid().mouse().setup();
  Runtime.hid().absoluteMouse().setup();

  uint8_t size = 10 * sizeof(uint8_t);
  MouseKeys_::storage_base_ = ::EEPROMSettings.requestSlice(size);
  uint8_t spd;
  uint16_t spdDelay;
  uint8_t accspd;
  uint16_t accDelay;
  uint8_t wheelspd;
  uint16_t wheelDlay;
  uint8_t spdLimit;

  Runtime.storage().get(storage_base_, spd);
  if(spd < 255){
    MouseKeys_::speed = spd;
  }else{
    Runtime.storage().update(storage_base_, speed);
  }

  Runtime.storage().get(storage_base_ + 1, spdDelay);
  if(spdDelay < 65535){
    speedDelay = spdDelay;
  }else{
    Runtime.storage().update(storage_base_ + 1, speedDelay);
  }

  Runtime.storage().get(storage_base_ + 3, accspd);
  if(accspd < 255){
    accelSpeed = accspd;
  }else{
    Runtime.storage().update(storage_base_ + 3, accelSpeed);
  }

  Runtime.storage().get(storage_base_ + 4, accDelay);
  if(accDelay < 65535){
    accelDelay = accDelay;
  }else{
    Runtime.storage().update(storage_base_ + 4, accelDelay);
  }

    Runtime.storage().get(storage_base_ + 6, wheelspd);
  if(wheelspd < 255){
    wheelSpeed = wheelspd;
  }else{
    Runtime.storage().update(storage_base_ + 6, wheelSpeed);
  }

  Runtime.storage().get(storage_base_ + 7, wheelDlay);
  if(wheelDlay != 65535){
    wheelDelay = wheelDlay;
  }else{
    Runtime.storage().update(storage_base_ + 7, wheelDelay);
  }

  Runtime.storage().get(storage_base_ + 9, spdLimit);
  if(spd < 255){
    setSpeedLimit(spdLimit);
  }else{
    Runtime.storage().update(storage_base_ + 9, MouseWrapper.speedLimit);
  }

  return EventHandlerResult::OK;
}

// -----------------------------------------------------------------------------
EventHandlerResult MouseKeys_::afterEachCycle() {
  // Check timeout for accel update interval.
  if (Runtime.hasTimeExpired(accel_start_time_, accelDelay)) {
    accel_start_time_ = Runtime.millisAtCycleStart();
    // `accelStep` determines the movement speed of the mouse pointer, and gets
    // reset to zero when no mouse movement keys is pressed (see below).
    if (MouseWrapper.accelStep < 255 - accelSpeed) {
      MouseWrapper.accelStep += accelSpeed;
    }
  }

  // Check timeout for position update interval.
  bool update_position = Runtime.hasTimeExpired(move_start_time_, speedDelay);
  if (update_position) {
    move_start_time_ = Runtime.millisAtCycleStart();
    // Determine which mouse movement directions are active by searching through
    // all the currently active keys for mouse movement keys, and adding them to
    // a bitfield (`directions`).
    uint8_t directions = 0;
    int8_t vx = 0;
    int8_t vy = 0;
    for (Key key : live_keys.all()) {
      if (isMouseKey(key) && isMouseMoveKey(key)) {
        directions |= key.getKeyCode();
      }
    }

    if (directions == 0) {
      // If there are no mouse movement keys held, reset speed to zero.
      MouseWrapper.accelStep = 0;
    } else {
      // For each active direction, add the mouse movement speed.
      if (directions & KEY_MOUSE_LEFT)
        vx -= speed;
      if (directions & KEY_MOUSE_RIGHT)
        vx += speed;
      if (directions & KEY_MOUSE_UP)
        vy -= speed;
      if (directions & KEY_MOUSE_DOWN)
        vy += speed;

      // Prepare the mouse report.
      MouseWrapper.move(vx, vy);
      // Send the report.
      Runtime.hid().mouse().sendReport();
    }
  }

  // Check timeout for scroll report interval.
  bool update_wheel = Runtime.hasTimeExpired(wheel_start_time_, wheelDelay);
  if (update_wheel) {
    wheel_start_time_ = Runtime.millisAtCycleStart();
    // Determine which scroll wheel keys are active, and add their directions to
    // a bitfield (`directions`).
    uint8_t directions = 0;
    int8_t vx = 0;
    int8_t vy = 0;
    for (Key key : live_keys.all()) {
      if (isMouseKey(key) && isMouseWheelKey(key)) {
        directions |= key.getKeyCode();
      }
    }

    if (directions != 0) {
      // Horizontal scroll wheel:
      if (directions & KEY_MOUSE_LEFT)
        vx -= wheelSpeed;
      if (directions & KEY_MOUSE_RIGHT)
        vx += wheelSpeed;
      // Vertical scroll wheel (note coordinates are opposite movement):
      if (directions & KEY_MOUSE_UP)
        vy += wheelSpeed;
      if (directions & KEY_MOUSE_DOWN)
        vy -= wheelSpeed;

      // Add scroll wheel changes to HID report.
      Runtime.hid().mouse().move(0, 0, vy, vx);
      // Send the report.
      Runtime.hid().mouse().sendReport();
    }
  }

  return EventHandlerResult::OK;
}

// -----------------------------------------------------------------------------
EventHandlerResult MouseKeys_::onKeyEvent(KeyEvent &event) {
  if (!isMouseKey(event.key))
    return EventHandlerResult::OK;

  if (isMouseButtonKey(event.key)) {
    sendMouseButtonReport(event);

  } else if (isMouseWarpKey(event.key)) {
    if (keyToggledOn(event.state)) {
      sendMouseWarpReport(event);
    }

  } else if (isMouseMoveKey(event.key)) {
    // No report is sent here; that's handled in `afterEachCycle()`.
    move_start_time_ = Runtime.millisAtCycleStart() - speedDelay;
    accel_start_time_ = Runtime.millisAtCycleStart();

  } else if (isMouseWheelKey(event.key)) {
    // No report is sent here; that's handled in `afterEachCycle()`.
    wheel_start_time_ = Runtime.millisAtCycleStart() - wheelDelay;
  }

  return EventHandlerResult::EVENT_CONSUMED;
}

// =============================================================================
// HID report helper functions

// -----------------------------------------------------------------------------
void MouseKeys_::sendMouseButtonReport(const KeyEvent &event) const {
  // Get ready to send a new mouse report by building it from live_keys. Note
  // that this also clears the movement and scroll values, but since those are
  // relative, that's what we want.
  Runtime.hid().mouse().releaseAllButtons();

  uint8_t buttons = 0;
  for (KeyAddr key_addr : KeyAddr::all()) {
    if (key_addr == event.addr)
      continue;
    Key key = live_keys[key_addr];
    if (isMouseKey(key) && isMouseButtonKey(key)) {
      buttons |= key.getKeyCode();
    }
  }
  if (keyToggledOn(event.state))
    buttons |= event.key.getKeyCode();
  buttons &= ~KEY_MOUSE_BUTTON;
  Runtime.hid().mouse().pressButtons(buttons);
  Runtime.hid().mouse().sendReport();
}

// -----------------------------------------------------------------------------
void MouseKeys_::sendMouseWarpReport(const KeyEvent &event) const {
  MouseWrapper.warp(
    ((event.key.getKeyCode() & KEY_MOUSE_WARP_END) ? WARP_END : 0x00) |
    ((event.key.getKeyCode() & KEY_MOUSE_UP) ? WARP_UP : 0x00)        |
    ((event.key.getKeyCode() & KEY_MOUSE_DOWN) ? WARP_DOWN : 0x00)    |
    ((event.key.getKeyCode() & KEY_MOUSE_LEFT) ? WARP_LEFT : 0x00)    |
    ((event.key.getKeyCode() & KEY_MOUSE_RIGHT) ? WARP_RIGHT : 0x00));
}

EventHandlerResult MouseKeys_::onFocusEvent(const char *command)
{
  if (::Focus.handleHelp(command, PSTR("mouse.speed\nmouse.speedDelay\nmouse.accelSpeed\nmouse.accelDelay\nmouse.wheelSpeed\nmouse.wheelDelay\nmouse.speedLimit")))
    return EventHandlerResult::OK;

  if (strncmp_P(command, PSTR("mouse."), 6) != 0)
    return EventHandlerResult::OK;

  if (strcmp_P(command + 6, PSTR("speed")) == 0)
  {
    if (::Focus.isEOL())
    {
      ::Focus.send(speed);
    }
    else
    {
      uint8_t auxspeed;
      ::Focus.read(auxspeed);
      speed = auxspeed;

      Runtime.storage().update(storage_base_ + 0, auxspeed);
      Runtime.storage().commit();
    }
  }

  if (strcmp_P(command + 6, PSTR("speedDelay")) == 0)
  {
    if (::Focus.isEOL())
    {
      ::Focus.send(speedDelay);
    }
    else
    {
      uint16_t auxspeedDelay = 0;
      uint8_t a{0};
      uint8_t b{0};
      ::Focus.read(a);
      while (!::Focus.isEOL())
      {
        ::Focus.read(b);
      }
      auxspeedDelay = ((b << 8) | a);
      speedDelay = auxspeedDelay;

      Runtime.storage().update(storage_base_ + 1, a);
      Runtime.storage().update(storage_base_ + 2, b);
      Runtime.storage().commit();
    }
  }

  if (strcmp_P(command + 6, PSTR("accelSpeed")) == 0)
  {
    if (::Focus.isEOL())
    {
      ::Focus.send(accelSpeed);
    }
    else
    {
      uint8_t auxaccelSpeed;
      ::Focus.read(auxaccelSpeed);
      accelSpeed = auxaccelSpeed;

      Runtime.storage().update(storage_base_ + 3, auxaccelSpeed);
      Runtime.storage().commit();
    }
  }

  if (strcmp_P(command + 6, PSTR("accelDelay")) == 0)
  {
    if (::Focus.isEOL())
    {
      ::Focus.send(accelDelay);
    }
    else
    {
      uint16_t auxaccelDelay = 0;
      uint8_t a{0};
      uint8_t b{0};
      ::Focus.read(a);
      while (!::Focus.isEOL())
      {
        ::Focus.read(b);
      }
      auxaccelDelay = ((b << 8) | a);
      accelDelay = auxaccelDelay;

      Runtime.storage().update(storage_base_ + 4, a);
      Runtime.storage().update(storage_base_ + 5, b);
      Runtime.storage().commit();
    }
  }

  if (strcmp_P(command + 6, PSTR("wheelSpeed")) == 0)
  {
    if (::Focus.isEOL())
    {
      ::Focus.send(wheelSpeed);
    }
    else
    {
      uint8_t auxwheelSpeed;
      ::Focus.read(auxwheelSpeed);
      wheelSpeed = auxwheelSpeed;

      Runtime.storage().update(storage_base_ + 6, auxwheelSpeed);
      Runtime.storage().commit();
    }
  }

  if (strcmp_P(command + 6, PSTR("wheelDelay")) == 0)
  {
    if (::Focus.isEOL())
    {
      ::Focus.send(wheelDelay);
    }
    else
    {
      uint16_t auxwheelDelay = 0;
      uint8_t a{0};
      uint8_t b{0};
      ::Focus.read(a);
      while (!::Focus.isEOL())
      {
        ::Focus.read(b);
      }
      auxwheelDelay = ((b << 8) | a);
      wheelDelay = auxwheelDelay;

      Runtime.storage().update(storage_base_ + 7, a);
      Runtime.storage().update(storage_base_ + 8, b);
      Runtime.storage().commit();
    }
  }

  if (strcmp_P(command + 6, PSTR("speedLimit")) == 0)
  {
    if (::Focus.isEOL())
    {
      ::Focus.send(MouseWrapper.speedLimit);
    }
    else
    {
      uint8_t auxspeedLimit;
      ::Focus.read(auxspeedLimit);
      setSpeedLimit(auxspeedLimit);

      Runtime.storage().update(storage_base_ + 9, auxspeedLimit);
      Runtime.storage().commit();
    }
  }

  return EventHandlerResult::EVENT_CONSUMED;
}

} // namespace plugin
} // namespace kaleidoscope

kaleidoscope::plugin::MouseKeys_ MouseKeys;
