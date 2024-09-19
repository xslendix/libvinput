#include "libvinput.h"

#include <stdio.h>

VInputError evdev_EventListener2_start(EventListener *listener, KeyboardCallback callback,
    MouseButtonCallback button_callback, MouseMoveCallback move_callback)
{
	(void)listener;
	(void)callback;
	(void)button_callback;
	(void)move_callback;
	fputs("Placeholder: evdev_EventListener2_start\n", stderr);
	return VINPUT_OK;
}

VInputError evdev_EventListener_free(EventListener *listener)
{
	(void)listener;
	fputs("Placeholder: evdev_EventListener_free\n", stderr);
	return VINPUT_OK;
}
