#ifndef LIBVINPUT_H
#define LIBVINPUT_H

// If you wish to use the old names, you need to define "#define LIBVINPUT_OLD_NAMES"
// before including this header.

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

// Currently, only used by the EventListener.
typedef struct _KeyboardModifiers
{
	bool left_control : 1;
	bool right_control : 1;
	bool left_shift : 1;
	bool right_shift : 1;
	bool left_alt : 1;
	bool right_alt : 1;
	bool left_meta : 1;
	bool right_meta : 1;
	bool left_super : 1;
	bool right_super : 1;
	bool left_hyper : 1;
	bool right_hyper : 1;
} KeyboardModifiers;

typedef struct _KeyboardEvent
{
	bool pressed;
	char keychar; // The ASCII character of the key, 0 if unavailable
	uint16_t keycode;
	uint16_t keysym; // On X11, the KeySym, on windows, the Virtual Key code

	KeyboardModifiers modifiers;

	size_t timestamp; // Timestamp of event, in milliseconds
} KeyboardEvent;

typedef struct _EventListener
{
	bool listen_keyboard;
	bool initialized;
	void *data; // Internal data, do not use
} EventListener;

typedef struct _EventEmulator
{
	bool initialized;
	void *data; // Internal data, do not use
} EventEmulator;

typedef enum _VInputError
{
	VINPUT_OK = 0,

	VINPUT_UNINITIALIZED,
	VINPUT_MALLOC,

	// WINAPI
	VINPUT_WINAPI_MODULE,
	VINPUT_WINAPI_HOOK,
	VINPUT_WINAPI_SENDINPUT,
	VINPUT_WINAPI_VKKEYSCANA,
	VINPUT_WINAPI_GETKEYBOARDSTATE,

	// X11
	VINPUT_X11_DISPLAY,
	VINPUT_X11_XKB_GET_MAP,
	VINPUT_X11_RANGE_ALLOC,
	VINPUT_X11_XRECORD_CONTEXT,
	VINPUT_X11_DISPLAY_DATALINK,
	VINPUT_X11_ENABLE_XRECORD,

	VINPUT_XDO_NEW,

	// MACOS
	VINPUT_MAC_TAP,

} VInputError;

typedef void (*KeyboardCallback)(KeyboardEvent);

#ifdef LIBVINPUT_OLD_NAMES
typedef EventListener Listener;
typedef EventEmulator Emulator;
#endif

bool VInput_modifier_pressed_except_shift(KeyboardModifiers modifiers);

char const *VInput_error_get_message(VInputError error);

// Create a EventListener, does not allocate memory for the listener.
VInputError EventListener_create(EventListener *listener, bool listen_keyboard);
// Make a Listener start listening. This is a blocking call.
VInputError EventListener_start(EventListener *listener, KeyboardCallback callback);
// Free up internal data in the Listener.
VInputError EventListener_free(EventListener *listener);

// Create an EventEmulator, does not allocate memory for the emulator.
VInputError EventEmulator_create(EventEmulator *emulator);
// Get current keyboard state, modifiers only, allocates memory
VInputError EventEmulator_keyboard_state_get(
    EventEmulator *emulator, int **state, int *nstate);
// Clear current keyboard state, modifiers only
VInputError EventEmulator_keyboard_state_clear(EventEmulator *emulator);
// Set current keyboard state, modifiers only, clears current keyboard state
VInputError EventEmulator_keyboard_state_set(
    EventEmulator *emulator, int *state, int nstate);
VInputError EventEmulator_press(EventEmulator *emulator, uint16_t keysym);
VInputError EventEmulator_release(EventEmulator *emulator, uint16_t keysym);
// Type out an ASCII character.
VInputError EventEmulator_typec(EventEmulator *emulator, char ch);
// Type out a string of ASCII characters.
VInputError EventEmulator_types(EventEmulator *emulator, char *buf, size_t len);
// Free up internal data in the Emulator.
VInputError EventEmulator_free(EventEmulator *emulator);

#ifdef LIBVINPUT_OLD_NAMES
#	define Listener_create EventListener_create
#	define Listener_start EventListener_start
#	define Listener_free EventListener_free

#	define Emulator_create EventEmulator_create
#	define Emulator_keyboard_state_get EventEmulator_keyboard_state_get
#	define Emulator_keyboard_state_clear EventEmulator_keyboard_state_clear
#	define Emulator_keyboard_state_set EventEmulator_keyboard_state_set
#	define Emulator_press EventEmulator_press
#	define Emulator_release EventEmulator_release
#	define Emulator_typec EventEmulator_typec
#	define Emulator_types EventEmulator_types
#	define Emulator_free EventEmulator_free
#endif

#ifdef __cplusplus
}
#endif

#endif // LIBVINPUT_H
