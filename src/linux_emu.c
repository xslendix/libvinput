#include "libvinput.h"

extern bool is_x11(void);

extern VInputError _x11_EventEmulator_init(EventEmulator *emulator);
VInputError x11_EventEmulator_keyboard_state_get(
    EventEmulator *emulator, int **state, int *nstate);
VInputError x11_EventEmulator_keyboard_state_clear(EventEmulator *emulator);
VInputError x11_EventEmulator_keyboard_state_set(
    EventEmulator *emulator, int *state, int nstate);
VInputError x11_EventEmulator_press(EventEmulator *emulator, uint16_t keysym);
VInputError x11_EventEmulator_release(EventEmulator *emulator, uint16_t keysym);
VInputError x11_EventEmulator_typec(EventEmulator *emulator, char ch);
VInputError x11_EventEmulator_types(EventEmulator *emulator, char *buf, size_t len);
VInputError x11_EventEmulator_free(EventEmulator *emulator);

extern VInputError _uinput_EventEmulator_init(EventEmulator *emulator);
VInputError uinput_EventEmulator_keyboard_state_get(
    EventEmulator *emulator, int **state, int *nstate);
VInputError uinput_EventEmulator_keyboard_state_clear(EventEmulator *emulator);
VInputError uinput_EventEmulator_keyboard_state_set(
    EventEmulator *emulator, int *state, int nstate);
VInputError uinput_EventEmulator_press(EventEmulator *emulator, uint16_t keysym);
VInputError uinput_EventEmulator_release(EventEmulator *emulator, uint16_t keysym);
VInputError uinput_EventEmulator_typec(EventEmulator *emulator, char ch);
VInputError uinput_EventEmulator_types(EventEmulator *emulator, char *buf, size_t len);
VInputError uinput_EventEmulator_free(EventEmulator *emulator);

VInputError _EventEmulator_init(EventEmulator *emulator)
{
	if (is_x11())
		return _x11_EventEmulator_init(emulator);
	else
		return _uinput_EventEmulator_init(emulator);
}

VInputError EventEmulator_keyboard_state_get(
    EventEmulator *emulator, int **state, int *nstate)
{
	if (is_x11())
		return x11_EventEmulator_keyboard_state_get(emulator, state, nstate);
	else
		return uinput_EventEmulator_keyboard_state_get(emulator, state, nstate);
}

VInputError EventEmulator_keyboard_state_clear(EventEmulator *emulator)
{
	if (is_x11())
		return x11_EventEmulator_keyboard_state_clear(emulator);
	else
		return uinput_EventEmulator_keyboard_state_clear(emulator);
}

VInputError EventEmulator_keyboard_state_set(
    EventEmulator *emulator, int *state, int nstate)
{
	if (is_x11())
		return x11_EventEmulator_keyboard_state_set(emulator, state, nstate);
	else
		return uinput_EventEmulator_keyboard_state_set(emulator, state, nstate);
}

VInputError EventEmulator_press(EventEmulator *emulator, uint16_t keysym)
{
	if (is_x11())
		return x11_EventEmulator_press(emulator, keysym);
	else
		return uinput_EventEmulator_press(emulator, keysym);
}

VInputError EventEmulator_release(EventEmulator *emulator, uint16_t keysym)
{
	if (is_x11())
		return x11_EventEmulator_release(emulator, keysym);
	else
		return uinput_EventEmulator_release(emulator, keysym);
}

VInputError EventEmulator_typec(EventEmulator *emulator, char ch)
{

	if (is_x11())
		return x11_EventEmulator_typec(emulator, ch);
	else
		return uinput_EventEmulator_typec(emulator, ch);
}

VInputError EventEmulator_types(EventEmulator *emulator, char *buf, size_t len)
{
	if (is_x11())
		return x11_EventEmulator_types(emulator, buf, len);
	else
		return uinput_EventEmulator_types(emulator, buf, len);
}

VInputError EventEmulator_free(EventEmulator *emulator)
{
	if (is_x11())
		return x11_EventEmulator_free(emulator);
	else
		return uinput_EventEmulator_free(emulator);
}
