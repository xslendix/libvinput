#include "libvinput.h"

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

VInputError EventEmulator_press(EventEmulator *emulator, uint16_t keysym)
{
	(void)keysym;
	if (!emulator->initialized) return VINPUT_UNINITIALIZED;
	return VINPUT_OK;
}

VInputError EventEmulator_release(EventEmulator *emulator, uint16_t keysym)
{
	(void)keysym;
	if (!emulator->initialized) return VINPUT_UNINITIALIZED;
	return VINPUT_OK;
}

VInputError EventEmulator_typec(EventEmulator *emulator, char ch)
{
	if (!ch) return VINPUT_OK;
	if (!emulator->initialized) return VINPUT_UNINITIALIZED;
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
