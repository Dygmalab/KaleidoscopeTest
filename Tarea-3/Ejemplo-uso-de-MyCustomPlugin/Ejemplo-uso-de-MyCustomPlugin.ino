#include "Kaleidoscope.h"
#include "Kaleidoscope-FocusSerial.h"
#include "MyCustomPlugin.h"


// El diccionario debe terminar con la combinación {Key_NoKey, Key_NoKey} para indicar su fin.
kaleidoscope::plugin::MyCustomPlugin::dictionary_t onKeyToggleOff_dictionary[] PROGMEM = {
 {Key_A, Key_B},
 {Key_B, Key_C},
 {Key_C, Key_D},
 {Key_D, Key_E},
 {Key_E, Key_F},
 {Key_F, Key_G},
 {Key_G, Key_H},
 {Key_H, Key_I},
 {Key_I, Key_J},
 {Key_J, Key_K},
 {Key_K, Key_L},
 {Key_L, Key_M},
 {Key_M, Key_N},
 {Key_N, Key_O},
 {Key_O, Key_P},
 {Key_P, Key_Q},
 {Key_Q, Key_R},
 {Key_R, Key_S},
 {Key_S, Key_T},
 {Key_T, Key_U},
 {Key_U, Key_V},
 {Key_V, Key_W},
 {Key_W, Key_X},
 {Key_X, Key_Y},
 {Key_Y, Key_Z},
 {Key_Z, Key_A},
 {Key_0, Key_1},
 {Key_1, Key_2},
 {Key_2, Key_3},
 {Key_3, Key_4},
 {Key_4, Key_5},
 {Key_5, Key_6},
 {Key_6, Key_7},
 {Key_7, Key_8},
 {Key_8, Key_9},
 {Key_9, Key_0},
 {Key_NoKey, Key_NoKey}
};

KALEIDOSCOPE_INIT_PLUGINS(
	Focus,
	MyCustomPlugin
);

void setup() {
	Kaleidoscope.setup();

	MyCustomPlugin.setDictionary(onKeyToggleOff_dictionary);
}

void loop() {
	Kaleidoscope.loop();
}
