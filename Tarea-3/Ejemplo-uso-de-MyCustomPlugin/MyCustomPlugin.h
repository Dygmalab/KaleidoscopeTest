#include <Kaleidoscope-FocusSerial.h>
#include "kaleidoscope/keyswitch_state.h"
#include "kaleidoscope/LiveKeys.h"

namespace kaleidoscope {
namespace plugin {

class MyCustomPlugin : public Plugin 
{
	public:
		MyCustomPlugin() {}

		typedef struct {
			Key releasedKey;
			Key replacementKey;
		} dictionary_t;

		const dictionary_t *dictionary = nullptr;
		
		EventHandlerResult onNameQuery(void);
		EventHandlerResult onKeyEvent(KeyEvent &event);

		void setDictionary(dictionary_t *dictionary);	// El diccionario debe terminar en {Key_NoKey, Key_NoKey}
};

}
}

extern kaleidoscope::plugin::MyCustomPlugin MyCustomPlugin;
