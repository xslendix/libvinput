#include "libvinput.h"

extern VInputError _Listener_init(Listener *);
extern VInputError _Emulator_init(Emulator *);

VInputError Listener_create(Listener *listener, bool listen_keyboard)
{
	listener->listen_keyboard = listen_keyboard;
	return _Listener_init(listener);
}

VInputError Emulator_create(Emulator *emulator)
{
	return _Emulator_init(emulator);
}
