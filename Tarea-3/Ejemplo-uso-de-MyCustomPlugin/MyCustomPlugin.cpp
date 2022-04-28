#include "MyCustomPlugin.h"

namespace kaleidoscope {
namespace plugin {

EventHandlerResult MyCustomPlugin::onNameQuery() {
  return ::Focus.sendName(F("MyCustomPlugin"));
}

EventHandlerResult MyCustomPlugin::onKeyEvent(KeyEvent &event) {
	if (keyToggledOff(event.state)) {								// Al soltar la tecla.
		if (dictionary == nullptr) return EventHandlerResult::OK;	// Si no se seteó el diccionario, regresa.

		if (event.key == Key_NoKey) return EventHandlerResult::OK;

		/*
		 * Busca la tecla actual en el diccionario y si la encuentra
		 * la reemplaza por la tecla de reemplazo definida en el
		 * diccionario.
		 */
		uint8_t i = 0;
		Key released;
		Key replacement;
		do {
			released = dictionary[i].releasedKey.readFromProgmem();
			replacement = dictionary[i].replacementKey.readFromProgmem();
			
			if (event.key == released) {	// Si la encuentra termina el bucle.
				event.key = replacement;
				break;
			}
			
			i++;
		} while ( !(released == Key_NoKey && replacement == Key_NoKey) );  // Flag de fin de diccionario = {Key_NoKey, Key_NoKey}.
	}
	
	::Focus.send(event.key);
	
	return EventHandlerResult::OK;
}

void MyCustomPlugin::setDictionary(dictionary_t *dictionary) {
	this->dictionary = dictionary;
}

}
}

kaleidoscope::plugin::MyCustomPlugin MyCustomPlugin;
