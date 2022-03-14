#include <Kaleidoscope-LEDEffect-DigitalRain.h>
#include <stdlib.h>

namespace kaleidoscope { namespace plugin {
	uint16_t LEDDigitalRainEffect::DECAY_MS = 2000;
	uint16_t LEDDigitalRainEffect::DROP_MS = 180;
	uint8_t LEDDigitalRainEffect::NEW_DROP_PROBABILITY = 18;
	uint8_t LEDDigitalRainEffect::PURE_GREEN_INTENSITY = 0xd0;
	uint8_t LEDDigitalRainEffect::MAXIMUM_BRIGHTNESS_BOOST = 0xff;
	uint8_t LEDDigitalRainEffect::COLOR_CHANNEL = 1;

	void LEDDigitalRainEffect::update(void) {
		uint8_t col;
		uint8_t row;

		// By how much intensity should each pixel decay,
		// based on how much time has passed since we last ran?
		uint8_t decayAmount = 0xff * (Runtime.millisAtCycleStart() - previousTimestamp) / DECAY_MS;

		// Decay intensities and possibly make new raindrops
		for (col = 0; col < COLS; col++) {
			for (row = 0; row < ROWS; row++) {
				if (row == 0 && justDropped && rand() < RAND_MAX / NEW_DROP_PROBABILITY) {
					// This is the top row, pixels have just fallen,
					// and we've decided to make a new raindrop in this column
					map[col][row] = 0xff;
				} else if (map[col][row] > 0 && map[col][row] < 0xff) {
					// Pixel is neither full brightness nor totally dark;
					// decay it
					if (map[col][row] <= decayAmount) {
						map[col][row] = 0;
					} else {
						map[col][row] -= decayAmount;
					}
				}

				// Set the colour for this pixel
				::LEDControl.setCrgbAt(KeyAddr(row, col), getColorFromIntensity(map[col][row]));
			}
		}
		// Paint underglow
		for (uint8_t ld = 69; ld < 131; ld++) {
			uint16_t led_hue = 0;
			uint8_t distance = UNDERGLOW_POS + 69 - ld;
			// We want led_hue to be capped at 255, but we do not want to clip it to
			// that, because that does not result in a nice animation. Instead, when it
			// is higher than 255, we simply substract 255, and repeat that until we're
			// within cap. This lays out the rainbow in a kind of wave.
			switch (distance)
			{
			case 9:
				led_hue = 45;
				break;
			case 8:
				led_hue = 65;
				break;
			case 7:
				led_hue = 80;
				break;
			case 6:
				led_hue = 85;
				break;
			case 5:
				led_hue = 90;
				break;
			case 4:
				led_hue = 105;
				break;
			case 3:
				led_hue = 170;
				break;
			case 2:
				led_hue = 215;
				break;
			case 1:
				led_hue = 225;
				break;
			case 0:
				led_hue = 255;
				break;
			default:
				led_hue = 0;
				break;
			}

			cRGB underg = hsvToRgb(120, 255, led_hue);
			::LEDControl.setCrgbAt(ld, underg);
		}

		// Movement trigger
		if (Runtime.hasTimeExpired(underglowTimestamp, 40)) {
			UNDERGLOW_POS += 1;
			if(UNDERGLOW_POS > 60){
				UNDERGLOW_POS = 0;
			}
			underglowTimestamp = Runtime.millisAtCycleStart();
		}

		// Drop the raindrops one row periodically
		if (Runtime.hasTimeExpired(dropStartTimestamp, DROP_MS)) {
			// Remember for next time that this just happened
			justDropped = true;

			// Reset the timestamp
			dropStartTimestamp = Runtime.millisAtCycleStart();

			for (row = ROWS - 1; row > 0; row--) {
				for (col = 0; col < COLS; col++) {
					// If this pixel is on the bottom row and bright,
					// allow it to start decaying
					if (row == ROWS - 1 && map[col][row] == 0xff) {
						map[col][row]--;
					}

					// Check if the pixel above is bright
					if (map[col][row - 1] == 0xff) {
						// Allow old bright pixel to decay
						map[col][row - 1]--;

						// Make this pixel bright
						map[col][row] = 0xff;
					}
				}
			}
		} else {
			justDropped = false;
		}

		// Update previous timestamp variable to now
		// so we can tell how much time has passed next time we run
		previousTimestamp = Runtime.millisAtCycleStart();
	}

	cRGB LEDDigitalRainEffect::getColorFromIntensity(uint8_t intensity) {
		uint8_t boost;

		// At high intensities start at light green
		// but drop off very quickly to full green
		if (intensity > PURE_GREEN_INTENSITY) {
			boost = (uint8_t) ((uint16_t) MAXIMUM_BRIGHTNESS_BOOST
					* (intensity - PURE_GREEN_INTENSITY)
					/ (0xff - PURE_GREEN_INTENSITY));
			return getColorFromComponents(0xff, boost);
		}
		return getColorFromComponents((uint8_t) ((uint16_t) 0xff * intensity / PURE_GREEN_INTENSITY), 0);
	}

	cRGB LEDDigitalRainEffect::getColorFromComponents(uint8_t primary, uint8_t secondary) {
		switch (COLOR_CHANNEL) {
			case 0: return CRGB(primary, secondary, secondary);
			case 1: return CRGB(secondary, primary, secondary);
			case 2: return CRGB(secondary, secondary, primary);
			default: return CRGB(0, 0, 0);
		}
	}
}}

kaleidoscope::plugin::LEDDigitalRainEffect LEDDigitalRainEffect;
