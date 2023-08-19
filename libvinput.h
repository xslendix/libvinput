#ifndef LIBVINPUT_H
#define LIBVINPUT_H

#include <stdbool.h>
#include <stdint.h>

typedef struct _KeyboardEvent
{
	bool pressed;
	char keychar;
	uint16_t keycode;
	uint16_t keysym;

	unsigned long long timestamp;
} KeyboardEvent;

typedef struct _Listener
{
	bool listen_keyboard;
	bool initialized;
	void *data;
} Listener;

typedef enum _VInputError
{
	VINPUT_OK = 0,

	VINPUT_UNINITIALIZED,

	// WINAPI
	VINPUT_WINAPI_MODULE,
	VINPUT_WINAPI_HOOK,

	// X11
	VINPUT_X11_DISPLAY,
	VINPUT_X11_XKB_GET_MAP,
	VINPUT_X11_RANGE_ALLOC,
	VINPUT_X11_XRECORD_CONTEXT,
	VINPUT_X11_DISPLAY_DATALINK,
	VINPUT_X11_ENABLE_XRECORD,
} VInputError;

typedef void (*KeyboardCallback)(KeyboardEvent);

VInputError Listener_create(Listener *listener, bool listen_keyboard);
VInputError Listener_start(Listener *listener, KeyboardCallback callback); // blocking
VInputError Listener_free(Listener *listener);

#endif // LIBVINPUT_H
