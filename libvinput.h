#ifndef LIBVINPUT_H
#define LIBVINPUT_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

// A keyboard event
typedef struct _KeyboardEvent
{
	bool pressed;     // Whether or not a key has been pressed or released
	char keychar;     // The ASCII character of the key, 0 if unavailable
	uint16_t keycode; // The scan code of the key
	uint16_t keysym;  // On X11, the KeySym, on windows, the Virtual Key code

	size_t timestamp; // Timestamp of event, in milliseconds
} KeyboardEvent;

// Listener for events
typedef struct _Listener
{
	bool listen_keyboard; // Whether or not to listen for keyboard events
	bool initialized;     // Whether or not the listener is initialized
	void *data;           // Internal data, do not use
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

// Create a Listener, does not allocate memory for the listener.
VInputError Listener_create(Listener *listener, bool listen_keyboard);
// Make a Listener start listening. This is a blocking call.
VInputError Listener_start(Listener *listener, KeyboardCallback callback);
// Free up internal data in the Listener.
VInputError Listener_free(Listener *listener);

#endif // LIBVINPUT_H
