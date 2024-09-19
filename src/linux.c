#include "libvinput.h"

#include <stdlib.h>
#include <string.h>

bool is_x11(void)
{
	return strcmp(getenv("XDG_SESSION_TYPE"), "x11") == 0;
}

extern VInputError x11_EventListener2_start(EventListener *listener,
    KeyboardCallback callback, MouseButtonCallback button_callback,
    MouseMoveCallback move_callback);
extern VInputError x11_EventListener_free(EventListener *listener);

extern VInputError evdev_EventListener2_start(EventListener *listener,
    KeyboardCallback callback, MouseButtonCallback button_callback,
    MouseMoveCallback move_callback);
extern VInputError evdev_EventListener_free(EventListener *listener);

VInputError EventListener2_start(EventListener *listener, KeyboardCallback callback,
    MouseButtonCallback button_callback, MouseMoveCallback move_callback)
{
	if (is_x11())
		return x11_EventListener2_start(listener, callback, button_callback, move_callback);
	else
		return evdev_EventListener2_start(listener, callback, button_callback, move_callback);
}

VInputError EventListener_free(EventListener *listener)
{
	if (is_x11())
		return x11_EventListener_free(listener);
	else
		return evdev_EventListener_free(listener);
}
