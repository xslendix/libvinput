#include "libvinput.h"

#include <stdlib.h>

typedef struct _EventListenerInternal
{
} EventListenerInternal;

VInputError _EventListener_init(EventListener *listener)
{
	listener->data = malloc(sizeof(EventListenerInternal));
	listener->initialized = true;
	return VINPUT_OK;
}

VInputError EventListener_start(EventListener *listener, KeyboardCallback callback)
{
	if (!listener->initialized) return VINPUT_UNINITIALIZED;
	KeyboardEvent ev = {
		.pressed = 1,
		.keychar = 'a',
		.keycode = 'a',
		.keysym = 'a',
		.modifiers = { 0 },
		.timestamp = 0,
	};
	callback(ev);
	ev = (KeyboardEvent) {
		.pressed = 1,
		.keychar = ' ',
		.keycode = ' ',
		.keysym = ' ',
		.modifiers = { 0 },
		.timestamp = 0,
	};
	callback(ev);
	return VINPUT_OK;
}

VInputError EventListener_free(EventListener *listener)
{
	if (!listener->initialized) return VINPUT_UNINITIALIZED;
	return VINPUT_OK;
}
