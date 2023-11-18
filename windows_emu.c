#include "libvinput.h"

#include <windows.h>
#include <winuser.h>

BYTE const modifiers[] = {
	VK_LSHIFT,
	VK_RSHIFT,
	VK_LCONTROL,
	VK_RCONTROL,
	VK_LMENU,
	VK_RMENU,
	0,
};
#define MODIFIERS_SIZE 6

VInputError _Emulator_init(EventEmulator *emulator)
{
	emulator->initialized = true;

	return VINPUT_OK;
}

VInputError EventEmulator_keyboard_state_get(
    EventEmulator *emulator, int **state, int *nstate)
{
	(void)nstate;

	if (!emulator->initialized) return VINPUT_UNINITIALIZED;

	BYTE states[256];
	if (!GetKeyboardState((PBYTE)states)) return VINPUT_WINAPI_GETKEYBOARDSTATE;

	*state = malloc(sizeof(BYTE) * MODIFIERS_SIZE);
	if (!*state) return VINPUT_MALLOC;

	for (int i = 0; i < MODIFIERS_SIZE; i++)
		(*state)[i] = states[modifiers[i]];

	return VINPUT_OK;
}

VInputError EventEmulator_keyboard_state_clear(EventEmulator *emulator)
{
	if (!emulator->initialized) return VINPUT_UNINITIALIZED;

	PBYTE cmod = (PBYTE)modifiers; // Current modifier
	while (*cmod) {
		INPUT input = { 0 };
		input.type = INPUT_KEYBOARD;
		input.ki.wVk = *(cmod++);
		input.ki.dwFlags = KEYEVENTF_KEYUP;
		if (SendInput(1, &input, sizeof(INPUT)) != 1) return VINPUT_WINAPI_SENDINPUT;
	}

	return VINPUT_OK;
}

VInputError EventEmulator_keyboard_state_set(
    EventEmulator *emulator, int *state, int nstate)
{
	(void)nstate;

	if (!emulator->initialized) return VINPUT_UNINITIALIZED;

	PBYTE cmod = (PBYTE)modifiers; // Current modifier
	while (*cmod) {
		if (!state[*cmod]) {
			cmod++;
			continue;
		}
		INPUT input = { 0 };
		input.type = INPUT_KEYBOARD;
		input.ki.wVk = *(cmod++);
		input.ki.dwFlags = 0;
		if (SendInput(1, &input, sizeof(INPUT)) != 1) return VINPUT_WINAPI_SENDINPUT;
	}

	return VINPUT_OK;
}

VInputError EventEmulator_press(EventEmulator *emulator, uint16_t keysym)
{
	if (!emulator->initialized) return VINPUT_UNINITIALIZED;

	INPUT input = { 0 };
	input.type = INPUT_KEYBOARD;
	input.ki.wVk = keysym;
	input.ki.dwFlags = 0;

	if (SendInput(1, &input, sizeof(INPUT)) != 1) return VINPUT_WINAPI_SENDINPUT;

	return VINPUT_OK;
}

VInputError EventEmulator_release(EventEmulator *emulator, uint16_t keysym)
{
	if (!emulator->initialized) return VINPUT_UNINITIALIZED;

	INPUT input = { 0 };
	input.type = INPUT_KEYBOARD;
	input.ki.wVk = keysym;
	input.ki.dwFlags = KEYEVENTF_KEYUP;

	if (SendInput(1, &input, sizeof(INPUT)) != 1) return VINPUT_WINAPI_SENDINPUT;

	return VINPUT_OK;
}

VInputError EventEmulator_typec(EventEmulator *emulator, char ch)
{
	if (!ch) return VINPUT_OK;
	if (!emulator->initialized) return VINPUT_UNINITIALIZED;

	BYTE vk = LOBYTE(VkKeyScanA(ch));
	BYTE shift_state = HIBYTE(VkKeyScanA(ch));

	INPUT input[2] = { 0 }; // One for key press another for key release

	// Key press event
	input[0].type = INPUT_KEYBOARD;
	input[0].ki.wVk = vk;
	input[1].ki.dwFlags = 0;

	// Key release event
	input[1].type = INPUT_KEYBOARD;
	input[1].ki.wVk = vk;
	input[1].ki.dwFlags = KEYEVENTF_KEYUP;

	if (shift_state & 1) {
		input[0].ki.dwFlags = KEYEVENTF_EXTENDEDKEY;
		input[1].ki.dwFlags = KEYEVENTF_EXTENDEDKEY | KEYEVENTF_KEYUP;
	}

	SendInput(2, input, sizeof(INPUT));

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
