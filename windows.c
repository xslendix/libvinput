#include "libvinput.h"

#include <stdlib.h>

#include <windows.h>
#include <winuser.h>

// FIXME: Make this thread-safe!

typedef struct _EventListenerInternal
{
	HINSTANCE exe;
	HHOOK key_hook;
} EventListenerInternal;

DWORD tls_index; // Thread-Local Storage

KeyboardEvent generate_keyevent(WPARAM wparam, LPARAM lparam)
{
	(void)lparam;
	DWORD timestamp = GetTickCount(); // Milliseconds, from boot time

	// Cast lParam to keyboard hook data
	KBDLLHOOKSTRUCT *keyboard = (KBDLLHOOKSTRUCT *)lparam;
	DWORD vkCode = keyboard->vkCode; // Virtual key code

	char keychar = '\0';
	WORD scanCode = keyboard->scanCode;
	BYTE keyState[256];         // Create a buffer for keyboard state
	GetKeyboardState(keyState); // Get the current keyboard state

	int ret = ToAscii(vkCode, scanCode, keyState, (LPWORD)&keychar, 0);
	if (ret == 1) { // Successful translation
		return (KeyboardEvent) {
			.pressed = wparam == WM_KEYDOWN,
			.keychar = keychar,
			.keycode = scanCode,
			.keysym = vkCode,
			.timestamp = timestamp,
		};
	} else { // No translation
		return (KeyboardEvent) {
			.pressed = wparam == WM_KEYDOWN,
			.keychar = '\0',
			.keycode = scanCode,
			.keysym = vkCode,
			.timestamp = timestamp,
		};
	}
}

LRESULT CALLBACK keyboard_callback(int code, WPARAM wparam, LPARAM lparam)
{
	KeyboardCallback cb = TlsGetValue(tls_index);
	// Handle key events or call next hook in chain.
	switch (wparam) {
	case WM_KEYDOWN:
	case WM_KEYUP: cb(generate_keyevent(wparam, lparam)); break;
	default: return CallNextHookEx(NULL, code, wparam, lparam);
	}
	return 0;
}

VInputError _Listener_init(EventListener *listener)
{
	listener->data = malloc(sizeof(EventListenerInternal));
	EventListenerInternal *data = listener->data;

	// Get module handle
	data->exe = GetModuleHandle(NULL);
	if (!data->exe) return VINPUT_WINAPI_MODULE;

	// Create keyboard hook
	data->key_hook
	    = SetWindowsHookEx(WH_KEYBOARD_LL, (HOOKPROC)keyboard_callback, data->exe, 0);
	if (!data->key_hook) return VINPUT_WINAPI_HOOK;

	tls_index = TlsAlloc();
	if (tls_index == TLS_OUT_OF_INDEXES) {
		UnhookWindowsHookEx(data->key_hook);
		return VINPUT_MALLOC;
	}

	listener->initialized = true;

	return VINPUT_OK;
}

VInputError EventListener_start(EventListener *listener, KeyboardCallback callback)
{
	if (!listener->initialized) return VINPUT_UNINITIALIZED;

	TlsSetValue(tls_index, callback);

	// Propagate messages.
	MSG msg;
	while (GetMessage(&msg, NULL, 0, 0) != 0) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return VINPUT_OK;
}

VInputError EventListener_free(EventListener *listener)
{
	if (!listener->initialized) return VINPUT_UNINITIALIZED;
	EventListenerInternal *data = listener->data;

	UnhookWindowsHookEx(data->key_hook);
	memset(data, 0, sizeof(EventListenerInternal));

	TlsFree(tls_index);

	return VINPUT_OK;
}
