#include "libvinput.h"

extern VInputError _EventListener_init(EventListener *);
extern VInputError _EventEmulator_init(EventEmulator *);

VINPUT_PUBLIC bool VInput_modifier_pressed_except_shift(KeyboardModifiers modifiers)
{
	return modifiers.left_control || modifiers.right_control || modifiers.left_alt
	       || modifiers.right_alt || modifiers.left_meta || modifiers.right_meta
	       || modifiers.left_super || modifiers.right_super || modifiers.left_hyper
	       || modifiers.right_hyper;
}

VINPUT_PUBLIC char const *VInput_error_get_message(VInputError error)
{
	switch (error) {
	case VINPUT_OK: return "OK";
	case VINPUT_UNINITIALIZED: return "Uninitialized";
	case VINPUT_MALLOC: return "Malloc failed";
	case VINPUT_WINAPI_MODULE: return "Failed to get module handle";
	case VINPUT_WINAPI_HOOK: return "Failed to create keyboard hook";
	case VINPUT_WINAPI_SENDINPUT: return "SendInput() failed";
	case VINPUT_WINAPI_VKKEYSCANA: return "VkKeyScanA() failed";
	case VINPUT_WINAPI_GETKEYBOARDSTATE: return "Failed to get keyboard state";
	case VINPUT_X11_DISPLAY: return "Failed to open X11 display";
	case VINPUT_X11_XKB_GET_MAP: return "Failed to get X11 keymap";
	case VINPUT_X11_RANGE_ALLOC: return "XRecordAllocRange() failed";
	case VINPUT_X11_XRECORD_CONTEXT: return "Failed to create XRecord context";
	case VINPUT_X11_DISPLAY_DATALINK: return "Failed to open datalink X11 display";
	case VINPUT_X11_ENABLE_XRECORD: return "Failed to enable XRecord context";
	case VINPUT_XDO_NEW: return "Failed to create xdo instance";
	case VINPUT_MAC_TAP: return "Cannot craete event tap";
	}
	return "Unknown error code";
}

VINPUT_PUBLIC VInputError EventListener_create(
    EventListener *listener, bool listen_keyboard)
{
	return EventListener2_create(listener, listen_keyboard, false, false);
}

VINPUT_PUBLIC VInputError EventListener_start(
    EventListener *listener, KeyboardCallback callback)
{
	return EventListener2_start(listener, callback, NULL, NULL);
}

VINPUT_PUBLIC VInputError EventListener2_create(EventListener *listener,
    bool listen_keyboard, bool listen_mouse_button, bool listen_mouse_move)
{
	listener->listen_keyboard = listen_keyboard;
	listener->listen_mouse_button = listen_mouse_button;
	listener->listen_mouse_move = listen_mouse_move;
	return _EventListener_init(listener);
}

VINPUT_PUBLIC VInputError EventEmulator_create(EventEmulator *emulator)
{
	return _EventEmulator_init(emulator);
}
