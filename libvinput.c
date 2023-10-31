#include "libvinput.h"

extern VInputError _Listener_init(Listener *);
extern VInputError _Emulator_init(Emulator *);

char const *VInput_error_get_message(VInputError error)
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
	}
	return "Unknown error code";
}

VInputError Listener_create(Listener *listener, bool listen_keyboard)
{
	listener->listen_keyboard = listen_keyboard;
	return _Listener_init(listener);
}

VInputError Emulator_create(Emulator *emulator)
{
	return _Emulator_init(emulator);
}
