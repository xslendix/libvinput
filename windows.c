#include "libvinput.h"

#include <stdlib.h>

#include <windows.h>
#include <winuser.h>

// FIXME: Make this thread-safe!

typedef struct _ListenerInternal
{
	HINSTANCE exe;
	HHOOK key_hook;
} ListenerInternal;

KeyboardCallback kcallback;

KeyboardEvent generate_keyevent(WPARAM wparam, LPARAM lparam)
{
	(void)lparam;
	KBDLLHOOKSTRUCT *keyboard = (KBDLLHOOKSTRUCT *)lparam;
	DWORD vkCode = keyboard->vkCode;

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
		};
	} else { // No translation
		return (KeyboardEvent) {
			.pressed = wparam == WM_KEYDOWN,
			.keychar = '\0',
			.keycode = scanCode,
			.keysym = vkCode,
		};
	}
}

LRESULT CALLBACK keyboard_callback(int code, WPARAM wparam, LPARAM lparam)
{
	switch (wparam) {
	case WM_KEYDOWN:
	case WM_KEYUP: kcallback(generate_keyevent(wparam, lparam)); break;
	default: return CallNextHookEx(NULL, code, wparam, lparam);
	}
	return 0;
}

VInputError _Listener_init(Listener *listener)
{
	listener->data = malloc(sizeof(ListenerInternal));
	ListenerInternal *data = listener->data;

	data->exe = GetModuleHandle(NULL);
	if (!data->exe) return VINPUT_WINAPI_MODULE;

	data->key_hook
	    = SetWindowsHookEx(WH_KEYBOARD_LL, (HOOKPROC)keyboard_callback, data->exe, 0);
	if (!data->key_hook) return VINPUT_WINAPI_HOOK;

	return VINPUT_OK;
}

VInputError Listener_start(Listener *listener, KeyboardCallback callback)
{
	if (!listener->initialized) return VINPUT_UNINITIALIZED;

	kcallback = callback;

	MSG msg;
	while (GetMessage(&msg, NULL, 0, 0) != 0) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return VINPUT_OK;
}

VInputError Listener_free(Listener *listener)
{
	if (!listener->initialized) return VINPUT_UNINITIALIZED;
	ListenerInternal *data = listener->data;

	UnhookWindowsHookEx(data->key_hook);
	memset(data, 0, sizeof(ListenerInternal));
	kcallback = NULL;

	return VINPUT_OK;
}
