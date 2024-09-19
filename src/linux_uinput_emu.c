#include "libvinput.h"

#include <stdio.h>

VInputError uinput_EventEmulator_keyboard_state_get(
    EventEmulator *emulator, int **state, int *nstate)
{
	(void)emulator;
	(void)state;
	(void)nstate;
	fputs("Placeholder: uinput_EventEmulator_keyboard_state_get\n", stderr);
	return VINPUT_OK;
}

VInputError uinput_EventEmulator_keyboard_state_clear(EventEmulator *emulator)
{
	(void)emulator;
	fputs("Placeholder: uinput_EventEmulator_keyboard_state_clear\n", stderr);
	return VINPUT_OK;
}

VInputError uinput_EventEmulator_keyboard_state_set(
    EventEmulator *emulator, int *state, int nstate)
{
	(void)emulator;
	(void)state;
	(void)nstate;
	fputs("Placeholder: uinput_EventEmulator_keyboard_state_set\n", stderr);
	return VINPUT_OK;
}

VInputError uinput_EventEmulator_press(EventEmulator *emulator, uint16_t keysym)
{
	(void)emulator;
	(void)keysym;
	fputs("Placeholder: uinput_EventEmulator_press\n", stderr);
	return VINPUT_OK;
}

VInputError uinput_EventEmulator_release(EventEmulator *emulator, uint16_t keysym)
{
	(void)emulator;
	(void)keysym;
	fputs("Placeholder: uinput_EventEmulator_release\n", stderr);
	return VINPUT_OK;
}

VInputError uinput_EventEmulator_typec(EventEmulator *emulator, char ch)
{
	(void)emulator;
	(void)ch;
	fputs("Placeholder: uinput_EventEmulator_typec\n", stderr);
	return VINPUT_OK;
}

VInputError uinput_EventEmulator_types(EventEmulator *emulator, char *buf, size_t len)
{
	(void)emulator;
	(void)buf;
	(void)len;
	fputs("Placeholder: uinput_EventEmulator_types\n", stderr);
	return VINPUT_OK;
}

VInputError uinput_EventEmulator_free(EventEmulator *emulator)
{
	(void)emulator;
	fputs("Placeholder: uinput_EventEmulator_free\n", stderr);
	return VINPUT_OK;
}
