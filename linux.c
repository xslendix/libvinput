#include "libvinput.h"

#include <stdlib.h>
#include <string.h>
#include <time.h>

#include <X11/XKBlib.h>
#include <X11/Xlib.h>
#include <X11/Xproto.h>
#include <X11/extensions/XKB.h>
#include <X11/extensions/record.h>

#define XK_MISCELLANY
#include <X11/keysymdef.h>

typedef struct _ListenerInternal
{
	Display *dpy, *dpy_datalink;
	XkbDescPtr keyboard_map;
	XRecordContext context;

	KeyboardCallback callback;

	KeyboardModifiers modifiers;
} ListenerInternal;

VInputError _Listener_init(Listener *listener)
{
	listener->data = malloc(sizeof(ListenerInternal));
	ListenerInternal *data = listener->data;

	memset(&data->modifiers, 0, sizeof(KeyboardModifiers));

	// Open X11 display connection
	data->dpy = XOpenDisplay(NULL);
	if (!data->dpy) return VINPUT_X11_DISPLAY;

	// Get keyboard map
	data->keyboard_map = XkbGetMap(data->dpy, XkbAllClientInfoMask, XkbUseCoreKbd);
	if (!data->keyboard_map) {
		XCloseDisplay(data->dpy);
		return VINPUT_X11_XKB_GET_MAP;
	}

	// Prepare XRecord to listen to global events
	XRecordClientSpec clients = XRecordAllClients;
	XRecordRange *range = XRecordAllocRange();
	if (range == 0) {
		XkbFreeClientMap(data->keyboard_map, XkbAllClientInfoMask, true);
		XCloseDisplay(data->dpy);
		return VINPUT_X11_RANGE_ALLOC;
	}

	memset(range, 0, sizeof(XRecordRange));
	// We only want KeyPresses and KeyReleases
	range->device_events.first = KeyPress;
	range->device_events.last = KeyRelease;

	// Create the context
	data->context = XRecordCreateContext(data->dpy, 0, &clients, 1, &range, 1);
	if (!data->context) {
		XFree(range);
		XkbFreeClientMap(data->keyboard_map, XkbAllClientInfoMask, true);
		XCloseDisplay(data->dpy);
		return VINPUT_X11_XRECORD_CONTEXT;
	}

	XFree(range);

	XSync(data->dpy, True);

	// Open a second display for datalink, this is what keeps events coming
	data->dpy_datalink = XOpenDisplay(NULL);
	if (!data->dpy_datalink) {
		XRecordFreeContext(data->dpy, data->context);
		XkbFreeClientMap(data->keyboard_map, XkbAllClientInfoMask, true);
		XCloseDisplay(data->dpy);
		return VINPUT_X11_DISPLAY_DATALINK;
	}

	listener->initialized = true;
	return VINPUT_OK;
}

// https://stackoverflow.com/a/10233743
KeySym keycode_to_keysym(ListenerInternal *data, KeyCode keycode, unsigned int event_mask)
{
	KeySym keysym = NoSymbol;

	if (data->keyboard_map) {
		unsigned char info = XkbKeyGroupInfo(data->keyboard_map, keycode);
		unsigned int num_groups = XkbKeyNumGroups(data->keyboard_map, keycode);

		unsigned int group = 0x00;
		switch (XkbOutOfRangeGroupAction(info)) {
		case XkbRedirectIntoRange:
			group = XkbOutOfRangeGroupInfo(info);
			if (group >= num_groups) { group = 0; }
			break;

		case XkbClampIntoRange: group = num_groups - 1; break;
		case XkbWrapIntoRange:
		default:
			if (num_groups != 0) { group %= num_groups; }
			break;
		}

		XkbKeyTypePtr key_type = XkbKeyKeyType(data->keyboard_map, keycode, group);
		unsigned int active_mods = event_mask & key_type->mods.mask;

		int i, level = 0;
		for (i = 0; i < key_type->map_count; i++) {
			if (key_type->map[i].active && key_type->map[i].mods.mask == active_mods) {
				level = key_type->map[i].level;
			}
		}

		keysym = XkbKeySymEntry(data->keyboard_map, keycode, level, group);
	}

	return keysym;
}

KeyboardEvent xevent_to_key_event(ListenerInternal *data_, XRecordInterceptData *data)
{
	// Get time in seconds and nanoseconds
	struct timespec ts;
	if (clock_gettime(CLOCK_MONOTONIC, &ts) != 0) {
		return (KeyboardEvent) {
			.pressed = 0,
			.keychar = 0,
			.keycode = 0,
			.keysym = 0,
			.modifiers = data_->modifiers,
			.timestamp = 0,
		};
	}
	size_t timestamp = ts.tv_nsec / 1000000;

	xEvent *event = (xEvent *)data->data;
	KeySym keysym = keycode_to_keysym(data_, event->u.u.detail, 0);
	// If keysym is not ascii, set keychar to 0.
	char ch = keysym & 0xff;
	if ((keysym & 0xff00) > 0) ch = '\0';
	if (keysym == XK_Return)
		ch = '\n';
	else if (keysym == XK_Linefeed)
		ch = '\r';
	else if (keysym == XK_BackSpace)
		ch = '\b';

	KeyboardEvent final_event;

	switch (event->u.u.type) {
	case KeyPress:
		final_event = (KeyboardEvent) {
			.pressed = true,
			.keychar = ch,
			.keycode = event->u.u.detail,
			.keysym = keysym,
			.modifiers = data_->modifiers,
			.timestamp = timestamp,
		};
		break;
	case KeyRelease:
		final_event = (KeyboardEvent) {
			.pressed = false,
			.keychar = ch,
			.keycode = event->u.u.detail,
			.keysym = keysym,
			.modifiers = data_->modifiers,
			.timestamp = timestamp,
		};
		break;
	default:
		return (KeyboardEvent) {
			.pressed = false,
			.keychar = 0,
			.keycode = 0,
			.keysym = 0,
			.timestamp = 0,
		};
	}

	if (final_event.keycode == XK_Shift_L)
		data_->modifiers.left_shift = final_event.pressed;
	else if (final_event.keycode == XK_Shift_R)
		data_->modifiers.right_shift = final_event.pressed;
	else if (final_event.keycode == XK_Control_L)
		data_->modifiers.left_control = final_event.pressed;
	else if (final_event.keycode == XK_Control_R)
		data_->modifiers.right_control = final_event.pressed;
	else if (final_event.keycode == XK_Alt_L)
		data_->modifiers.left_alt = final_event.pressed;
	else if (final_event.keycode == XK_Alt_R)
		data_->modifiers.right_alt = final_event.pressed;
	else if (final_event.keycode == XK_Meta_L)
		data_->modifiers.left_meta = final_event.pressed;
	else if (final_event.keycode == XK_Meta_R)
		data_->modifiers.right_meta = final_event.pressed;
	else if (final_event.keycode == XK_Super_L)
		data_->modifiers.left_super = final_event.pressed;
	else if (final_event.keycode == XK_Super_R)
		data_->modifiers.right_super = final_event.pressed;
	else if (final_event.keycode == XK_Hyper_L)
		data_->modifiers.left_hyper = final_event.pressed;
	else if (final_event.keycode == XK_Hyper_R)
		data_->modifiers.right_hyper = final_event.pressed;

	final_event.modifiers = data_->modifiers;

	return final_event;
}

void xrecord_callback(XPointer incoming, XRecordInterceptData *data)
{
	ListenerInternal *data_ = (ListenerInternal *)incoming;
	if (data->category != XRecordFromServer) goto xrecord_callback_end;

	data_->callback(xevent_to_key_event(data_, data));

xrecord_callback_end:
	XRecordFreeData(data);
}

VInputError Listener_start(Listener *listener, KeyboardCallback callback)
{
	if (!listener->initialized) return VINPUT_UNINITIALIZED;
	ListenerInternal *data = listener->data;
	data->callback = callback;

	// Start listening for events
	if (!XRecordEnableContext(
	        data->dpy_datalink, data->context, xrecord_callback, (XPointer)data))
		return VINPUT_X11_ENABLE_XRECORD;

	return VINPUT_OK;
}

VInputError Listener_free(Listener *listener)
{
	if (!listener->initialized) return VINPUT_UNINITIALIZED;
	ListenerInternal *data = listener->data;

	XCloseDisplay(data->dpy_datalink);
	XRecordFreeContext(data->dpy, data->context);
	XkbFreeClientMap(data->keyboard_map, XkbAllClientInfoMask, true);
	XCloseDisplay(data->dpy);
	memset(data, 0, sizeof(ListenerInternal));

	return VINPUT_OK;
}
