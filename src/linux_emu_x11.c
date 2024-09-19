#include "libvinput.h"

#include <stdlib.h>
#include <string.h>

#define XK_MISCELLANY
#define XK_XKB_KEYS
#include <X11/XKBlib.h>
#include <X11/Xlib.h>
#include <X11/extensions/XKB.h>
#include <X11/extensions/XKBstr.h>
#include <X11/extensions/XTest.h>
#include <X11/keysymdef.h>

#include <xdo.h>

typedef struct _EventEmulatorInternal
{
	Display *dpy;
	xdo_t *xdo;
} EventEmulatorInternal;

VInputError _EventEmulator_init(EventEmulator *emulator)
{
	emulator->data = malloc(sizeof(EventEmulatorInternal));
	EventEmulatorInternal *data = emulator->data;

	data->dpy = XOpenDisplay(NULL);
	if (!data->dpy) return VINPUT_X11_DISPLAY;

	data->xdo = xdo_new(NULL);
	if (!data->xdo) {
		XCloseDisplay(data->dpy);
		return VINPUT_XDO_NEW;
	}

	emulator->initialized = true;

	return VINPUT_OK;
}

VInputError x11_EventEmulator_keyboard_state_get(
    EventEmulator *emulator, int **state, int *nstate)
{
	if (!emulator->initialized) return VINPUT_UNINITIALIZED;
	EventEmulatorInternal *data = emulator->data;

	xdo_get_active_modifiers(data->xdo, (charcodemap_t **)state, nstate);

	return VINPUT_OK;
}

VInputError x11_EventEmulator_keyboard_state_clear(EventEmulator *emulator)
{
	if (!emulator->initialized) return VINPUT_UNINITIALIZED;
	EventEmulatorInternal *data = emulator->data;

	charcodemap_t *active_mods = NULL;
	int active_mods_len;
	EventEmulator_keyboard_state_get(emulator, (void *)&active_mods, &active_mods_len);
	xdo_clear_active_modifiers(data->xdo, 0, active_mods, active_mods_len);

	return VINPUT_OK;
}

VInputError x11_EventEmulator_keyboard_state_set(
    EventEmulator *emulator, int *state, int nstate)
{
	if (!emulator->initialized) return VINPUT_UNINITIALIZED;
	EventEmulatorInternal *data = emulator->data;

	xdo_set_active_modifiers(data->xdo, 0, (charcodemap_t *)state, nstate);

	return VINPUT_OK;
}

VInputError x11_EventEmulator_press(EventEmulator *emulator, uint16_t keysym)
{
	if (!emulator->initialized) return VINPUT_UNINITIALIZED;
	EventEmulatorInternal *data = emulator->data;

	unsigned int keycode = XKeysymToKeycode(data->dpy, keysym);
	if (keycode) XTestFakeKeyEvent(data->dpy, keycode, True, 0);
	XSync(data->dpy, True);

	return VINPUT_OK;
}

VInputError x11_EventEmulator_release(EventEmulator *emulator, uint16_t keysym)
{
	if (!emulator->initialized) return VINPUT_UNINITIALIZED;
	EventEmulatorInternal *data = emulator->data;

	unsigned int keycode = XKeysymToKeycode(data->dpy, keysym);
	if (keycode) XTestFakeKeyEvent(data->dpy, keycode, False, 0);
	XSync(data->dpy, True);

	return VINPUT_OK;
}

VInputError x11_EventEmulator_typec(EventEmulator *emulator, char ch)
{
	if (!ch) return VINPUT_OK;
	if (!emulator->initialized) return VINPUT_UNINITIALIZED;
	EventEmulatorInternal *data = emulator->data;

	// Keysyms are ASCII compatible
	char keysym = XKeysymToKeycode(data->dpy, ch);
	XTestFakeKeyEvent(data->dpy, keysym, True, 0);
	XTestFakeKeyEvent(data->dpy, keysym, False, 0);
	XSync(data->dpy, True);
	return VINPUT_OK;
}

VInputError x11_EventEmulator_types(EventEmulator *emulator, char *buf, size_t len)
{
	if (!buf || !len) return VINPUT_OK;
	if (!emulator->initialized) return VINPUT_UNINITIALIZED;
	EventEmulatorInternal *data = emulator->data;

	for (size_t i = 0; i < len && buf[i]; i++) {
		// Keysyms are ASCII compatible
		char keysym = XKeysymToKeycode(data->dpy, buf[i]);
		XTestFakeKeyEvent(data->dpy, keysym, True, 0);
		XTestFakeKeyEvent(data->dpy, keysym, False, 0);
	}
	XSync(data->dpy, True);

	return VINPUT_OK;
}

VInputError x11_EventEmulator_free(EventEmulator *emulator)
{
	if (!emulator->initialized) return VINPUT_UNINITIALIZED;
	EventEmulatorInternal *data = emulator->data;

	xdo_free(data->xdo);
	XCloseDisplay(data->dpy);
	memset(emulator, 0, sizeof(EventEmulator));

	return VINPUT_OK;
}
