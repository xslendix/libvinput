#include "libvinput.h"

extern VInputError _Listener_init(Listener *);

VInputError Listener_create(Listener *listener, bool listen_keyboard)
{
	listener->listen_keyboard = listen_keyboard;
	return _Listener_init(listener);
}
