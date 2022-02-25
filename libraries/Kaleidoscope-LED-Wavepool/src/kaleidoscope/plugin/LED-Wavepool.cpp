/* -*- mode: c++ -*-
 * Kaleidoscope-LED-Wavepool
 * Copyright (C) 2017 Selene Scriven
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <Kaleidoscope-LED-Wavepool.h>
#include "kaleidoscope/keyswitch_state.h"

namespace kaleidoscope {
namespace plugin {

#define INTERPOLATE 1 // smoother, slower animation
#define MS_PER_FRAME 40  // 40 = 25 fps
#define FRAMES_PER_DROP 120  // max time between raindrops during idle animation

uint16_t WavepoolEffect::idle_timeout = 5000;  // 5 seconds
int16_t WavepoolEffect::ripple_hue = WavepoolEffect::rainbow_hue; // automatic hue

// map native keyboard coordinates (16x4) into geometric space (14x5)
PROGMEM const uint8_t WavepoolEffect::TransientLEDMode::rc2pos[144] = {
  71, 71, 72, 73, 74, 75, 76,   76,  76, 107,  107, 106, 105, 104, 103, 102, 101, 101,
  70,  0,  1,  2,  3,  4,  5,    6,  77, 108,   33,  34,  35,  36,  37,  38,  39, 100,
  69,  7,  8,  9, 10, 11, 12,   78, 109,  47,   46,  45,  44,  43,  42,  41,  40,  99,
  98, 13, 14, 15, 16, 17, 18,   79, 110,  54,   53,  52,  51,  50,  49,  48, 130, 130,
  97, 19, 20, 21, 22, 23, 24,   25,  80, 111,   60,  59,  58,  57,  56,  55, 129, 129,
  96, 26, 27, 28, 29, 30, 31,   32,  81, 112,   68,  67,  66,  65,  64,  63,  62,  61,
  95, 94, 93, 92, 86, 85, 84,   83,  82, 113,  114, 115, 123, 124, 125, 126, 127, 128,
  91, 90, 89, 88, 87, 86, 85,   84,  83, 114,  115, 116, 117, 118, 119, 120, 121, 122
};

WavepoolEffect::TransientLEDMode::TransientLEDMode(const WavepoolEffect *parent)
  : frames_since_event_(0),
    surface_{},
    page_(0)
{}

EventHandlerResult WavepoolEffect::onKeyEvent(KeyEvent &event) {
  if (!event.addr.isValid())
    return EventHandlerResult::OK;

  if (::LEDControl.get_mode_index() != led_mode_id_)
    return EventHandlerResult::OK;

  return ::LEDControl.get_mode<TransientLEDMode>()->onKeyEvent(event);
}

EventHandlerResult WavepoolEffect::TransientLEDMode::onKeyEvent(KeyEvent &event) {
  // It might be better to trigger on both toggle-on and toggle-off, but maybe
  // just the former.
  if (keyIsPressed(event.state)) {
    surface_[page_][pgm_read_byte(rc2pos + event.addr.toInt())] = 0x7f;
    frames_since_event_ = 0;
  }

  return EventHandlerResult::OK;
}

void WavepoolEffect::TransientLEDMode::raindrop(uint8_t x, uint8_t y, int8_t *page_) {
  uint8_t rainspot = (y * WP_WID) + x;

  page_[rainspot] = 0x7f;
  if (y > 0) page_[rainspot - WP_WID] = 0x60;
  if (y < (WP_HGT - 1)) page_[rainspot + WP_WID] = 0x60;
  if (x > 0) page_[rainspot - 1] = 0x60;
  if (x < (WP_WID - 1)) page_[rainspot + 1] = 0x60;
}

// this is a lot smaller than the standard library's rand(),
// and still looks random-ish
uint8_t WavepoolEffect::TransientLEDMode::wp_rand() {
  static intptr_t offset = 0x400;
  offset = ((offset + 1) & 0x4fff) | 0x400;
  return (Runtime.millisAtCycleStart() / MS_PER_FRAME) + pgm_read_byte((const uint8_t *)offset);
}

void WavepoolEffect::TransientLEDMode::update(void) {

  // limit the frame rate; one frame every 64 ms
  static uint8_t prev_time = 0;
  uint8_t now = Runtime.millisAtCycleStart() / MS_PER_FRAME;
  if (now != prev_time) {
    prev_time = now;
  } else {
    return;
  }

  // rotate the colors over time
  // (side note: it's weird that this is a 16-bit int instead of 8-bit,
  //  but that's what the library function wants)
  static uint8_t current_hue = 0;
  current_hue ++;

  frames_since_event_ ++;

  // needs two pages of height map to do the calculations
  int8_t *newpg = &surface_[page_ ^ 1][0];
  int8_t *oldpg = &surface_[page_][0];

  // rain a bit while idle
  static uint8_t frames_till_next_drop = 0;
  static int8_t prev_x = -1;
  static int8_t prev_y = -1;
#ifdef INTERPOLATE
  // even frames: water movement and page flipping
  // odd frames: raindrops and tweening
  // (this arrangement seems to look best overall)
  if (((now & 1)) && (idle_timeout > 0)) {
#else
  if (idle_timeout > 0) {
#endif
    // repeat previous raindrop to give it a slightly better effect
    if (prev_x >= 0) {
      raindrop(prev_x, prev_y, oldpg);
      prev_x = prev_y = -1;
    }
    if (frames_since_event_
        >= (frames_till_next_drop
            + (idle_timeout / MS_PER_FRAME))) {
      frames_till_next_drop = 4 + (wp_rand() % FRAMES_PER_DROP);
      frames_since_event_ = idle_timeout / MS_PER_FRAME;

      uint8_t x = wp_rand() % WP_WID;
      uint8_t y = wp_rand() % WP_HGT;
      raindrop(x, y, oldpg);

      prev_x = x;
      prev_y = y;
    }
  }

  // calculate water movement
  // (originally skipped edges, but this keyboard is too small for that)
  //for (uint8_t y = 1; y < WP_HGT-1; y++) {
  //  for (uint8_t x = 1; x < WP_WID-1; x++) {
#ifdef INTERPOLATE
  if (!(now & 1)) {  // even frames only
#endif
    for (uint8_t y = 0; y < WP_HGT; y++) {
      for (uint8_t x = 0; x < WP_WID; x++) {
        uint8_t offset = (y * WP_WID) + x;

        int16_t value;
        int8_t offsets[] = { -WP_WID,    WP_WID,
                             -1,         1,
                             -WP_WID - 1, -WP_WID + 1,
                             WP_WID - 1,  WP_WID + 1
                           };
        // don't wrap around edges or go out of bounds
        if (y == 0) {
          offsets[0] = 0;
          offsets[4] += WP_WID;
          offsets[5] += WP_WID;
        } else if (y == WP_HGT - 1) {
          offsets[1] = 0;
          offsets[6] -= WP_WID;
          offsets[7] -= WP_WID;
        }
        if (x == 0) {
          offsets[2] = 0;
          offsets[4] += 1;
          offsets[6] += 1;
        } else if (x == WP_WID - 1) {
          offsets[3] = 0;
          offsets[5] -= 1;
          offsets[7] -= 1;
        }

        // add up all samples, divide, subtract prev frame's center
        int8_t *p;
        for (p = offsets, value = 0; p < offsets + 8; p++)
          value += oldpg[offset + (*p)];
        value = (value >> 2) - newpg[offset];

        // reduce intensity gradually over time
        newpg[offset] = value - (value >> 3);
      }
    }
#ifdef INTERPOLATE
  }
#endif

  // draw the water on the keys
  for (auto key_addr : rc2pos) {
    int8_t height = oldpg[key_addr];
#ifdef INTERPOLATE
    if (now & 1) {  // odd frames only
      // average height with other frame
      height = ((int16_t)height + newpg[key_addr]) >> 1;
    }
#endif

    uint8_t intensity = abs(height) * 2;
    uint8_t saturation = 0xff - intensity;
    uint8_t value = (intensity >= 128) ? 255 : intensity << 1;
    int16_t hue = ripple_hue;

    if (ripple_hue == WavepoolEffect::rainbow_hue) {
      // color starts white but gets dimmer and more saturated as it fades,
      // with hue wobbling according to height map
      hue = (current_hue + height + (height >> 1)) & 0xff;
    }

    cRGB color = hsvToRgb(hue, saturation, value);

    ::LEDControl.setCrgbAt(key_addr, color);
  }

#ifdef INTERPOLATE
  // swap pages every other frame
  if (!(now & 1)) page_ ^= 1;
#else
  // swap pages every frame
  page_ ^= 1;
#endif

}

}
}

kaleidoscope::plugin::WavepoolEffect WavepoolEffect;