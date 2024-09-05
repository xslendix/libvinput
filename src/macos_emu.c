#include "libvinput.h"

#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#include <Carbon/Carbon.h>
#include <CoreServices/CoreServices.h>

VInputError _EventEmulator_init(EventEmulator *emulator)
{
	emulator->initialized = true;
	return VINPUT_OK;
}

VInputError EventEmulator_keyboard_state_get(
    EventEmulator *emulator, int **state, int *nstate)
{
	(void)nstate;
	(void)state;
	(void)emulator;
	if (!emulator->initialized) return VINPUT_UNINITIALIZED;
	return VINPUT_OK;
}

VInputError EventEmulator_keyboard_state_clear(EventEmulator *emulator)
{
	if (!emulator->initialized) return VINPUT_UNINITIALIZED;
	return VINPUT_OK;
}

VInputError EventEmulator_keyboard_state_set(
    EventEmulator *emulator, int *state, int nstate)
{
	(void)nstate;
	(void)state;
	(void)emulator;
	if (!emulator->initialized) return VINPUT_UNINITIALIZED;
	return VINPUT_OK;
}

static VInputError send_event(CGKeyCode keysym, bool down, CGEventFlags flags)
{
	CGEventRef key_event = CGEventCreateKeyboardEvent(NULL, keysym, down);
	if (!key_event) return VINPUT_MAC_EVENT_CREATION;
	if (flags) CGEventSetFlags(key_event, flags);
	CGEventPost(kCGHIDEventTap, key_event);
	CFRelease(key_event);
	return VINPUT_OK;
}

int msleep(long msec)
{
	struct timespec ts;
	int res;

	if (msec < 0) {
		errno = EINVAL;
		return -1;
	}

	ts.tv_sec = msec / 1000;
	ts.tv_nsec = (msec % 1000) * 1000000;

	do {
		res = nanosleep(&ts, &ts);
	} while (res && errno == EINTR);

	return res;
}

VInputError EventEmulator_press(EventEmulator *emulator, uint16_t keysym)
{
	(void)keysym;
	if (!emulator->initialized) return VINPUT_UNINITIALIZED;
	msleep(2);
	return send_event((CGKeyCode)keysym, true, 0);
}

VInputError EventEmulator_release(EventEmulator *emulator, uint16_t keysym)
{
	(void)keysym;
	if (!emulator->initialized) return VINPUT_UNINITIALIZED;
	msleep(2);
	return send_event((CGKeyCode)keysym, false, 0);
}

int get_keycode_for_string(char *input_str);
VInputError EventEmulator_typec(EventEmulator *emulator, char ch)
{
	if (!ch) return VINPUT_OK;
	if (!emulator->initialized) return VINPUT_UNINITIALIZED;

	if (isupper(ch)) { EventEmulator_press(emulator, 60); }
	char buf[] = { ch, 0 };
	int keysym = get_keycode_for_string(buf);
	if (keysym == -1) return VINPUT_MAC_INVALID_KEY;
	EventEmulator_press(emulator, keysym);
	EventEmulator_release(emulator, keysym);
	if (isupper(ch)) { EventEmulator_release(emulator, 60); }

	return VINPUT_OK;
}

VInputError EventEmulator_types(EventEmulator *emulator, char *buf, size_t len)
{
	if (!buf || !len) return VINPUT_OK;
	if (!emulator->initialized) return VINPUT_UNINITIALIZED;

	for (size_t i = 0; i < len && buf[i]; i++)
		EventEmulator_typec(emulator, buf[i]);

	return VINPUT_OK;
}

VInputError EventEmulator_free(EventEmulator *emulator)
{
	if (!emulator->initialized) return VINPUT_UNINITIALIZED;
	return VINPUT_OK;
}

typedef struct kc_mapping
{
	char const *str;
	int kc;
} kc_mapping_t;

// Events.h, second enum.
kc_mapping_t const kc_to_str[] = { { "F1", 122 }, { "F2", 120 }, { "F3", 99 },
	{ "F4", 118 }, { "F5", 96 }, { "F6", 97 }, { "F7", 98 }, { "F8", 100 }, { "F9", 101 },
	{ "F10", 109 }, { "F11", 103 }, { "F12", 111 }, { "F13", 105 }, { "F14", 107 },
	{ "F15", 113 }, { "F16", 106 }, { "F17", 64 }, { "F18", 79 }, { "F19", 80 },
	{ "Space", 49 }, { "DelLeft", 51 }, { "DelRight", 117 }, { "PadClear", 71 },
	{ "LeftArrow", 123 }, { "RightArrow", 124 }, { "UpArrow", 126 }, { "DownArrow", 125 },
	{ "SEArrow", 119 }, { "NWArrow", 115 }, { "Escape", 53 }, { "PageDown", 121 },
	{ "PageUp", 116 }, { "ReturnR2L", 36 }, { "Return", 76 }, { "Tab", 48 },
	{ "Help", 114 }, { "VolumeUp", 72 }, { "VolumeDown", 73 }, { "RightShift", 60 },
	{ "LeftShift", 57 }, { "LeftControl", 59 }, { "LeftOption", 58 }, { "LeftCommand", 55 },
	{ "RightCommand", 55 }, { "RightOption", 61 }, { "RightControl", 62 }, { "\n", 36 },
	{ "Enter", 36 }, { NULL, -1 } };

// Returns -1 on failure.
int get_keycode_for_string(char *input_str)
{
	for (int i = 0; kc_to_str[i].str != NULL; i++) {
		if (strcmp(kc_to_str[i].str, input_str) == 0) { return kc_to_str[i].kc; }
	}

	// Make sure it's lower-case so that we can find capital keys too.
	CFMutableStringRef input_str_int = CFStringCreateMutable(NULL, 0);
	// 5 should be unicode.
	// https://developer.apple.com/documentation/corefoundation/cfstringbuiltinencodings
	CFStringAppendCString(input_str_int, input_str, 5);
	CFStringLowercase(input_str_int, 0);
	bool success = CFStringGetCString(input_str_int, input_str, strlen(input_str) + 1, 5);
	if (!success) {
		CFRelease(input_str_int);
		return -1;
	}
	CFRelease(input_str_int);

	// Get current keyboard layout data
	OSStatus err;
	TISInputSourceRef tis_src = TISCopyCurrentKeyboardInputSource();
	if (!tis_src) return -1;

	CFDataRef layout_data
	    = (CFDataRef)TISGetInputSourceProperty(tis_src, kTISPropertyUnicodeKeyLayoutData);
	CFRelease(tis_src);

	if (!layout_data) {
		tis_src = TISCopyCurrentASCIICapableKeyboardLayoutInputSource();
		layout_data
		    = (CFDataRef)TISGetInputSourceProperty(tis_src, kTISPropertyUnicodeKeyLayoutData);
		CFRelease(tis_src);

		return -1;
	}

	const UCKeyboardLayout *key_layout
	    = (const UCKeyboardLayout *)CFDataGetBytePtr(layout_data);
	UniChar chars[4];
	UInt32 keys_down = 0;

	for (int kc = 0; kc < 128; kc++) {
		UniCharCount len = 4, real_len;

		err = UCKeyTranslate(key_layout, kc, kUCKeyActionDisplay, 0, LMGetKbdType(),
		    kUCKeyTranslateNoDeadKeysBit, &keys_down, len, &real_len, chars);

		if (err != noErr) continue;

		char key_str[5];
		CFStringRef cf_str = CFStringCreateWithCharacters(NULL, chars, real_len);
		CFStringGetCString(cf_str, key_str, sizeof(key_str), kCFStringEncodingUTF8);
		CFRelease(cf_str);

		// We found our key!
		if (strcmp(key_str, input_str) == 0) { return kc; }
	}

	// Not found
	return -1;
}
