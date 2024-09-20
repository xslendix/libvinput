#include "libvinput.h"

#include <dirent.h>
#include <fcntl.h>
#include <libevdev/libevdev.h>
#include <math.h>
#include <poll.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <xkbcommon/xkbcommon.h>

#include "ext/vec.c"

typedef struct _EventListenerInternal
{
	struct xkb_context *ctx;
	struct xkb_keymap *keymap;
	struct xkb_state *state;
	struct libevdev **v_valid_devices;
} EventListenerInternal;

VInputError is_keyboard(const char *device_path, bool *is)
{
	int fd = open(device_path, O_RDONLY | O_NONBLOCK);
	if (fd < 0) return VINPUT_NO_PERM;

	struct libevdev *dev = NULL;
	int err = libevdev_new_from_fd(fd, &dev);
	if (err < 0) {
		close(fd);
		return VINPUT_IOCTL_FAIL;
	}

	*is = libevdev_has_event_type(dev, EV_KEY);
	libevdev_free(dev);
	close(fd);
	return VINPUT_OK;
}

VInputError _evdev_EventListener_init(EventListener *listener)
{
	puts("EventListerner init");
	listener->data = malloc(sizeof(EventListenerInternal));
	EventListenerInternal *data = listener->data;
	if (!data) return VINPUT_MALLOC;

	data->ctx = xkb_context_new(XKB_CONTEXT_NO_FLAGS);
	if (data->ctx) {
		data->keymap
		    = xkb_keymap_new_from_names(data->ctx, NULL, XKB_KEYMAP_COMPILE_NO_FLAGS);
		if (data->keymap) {
			data->state = xkb_state_new(data->keymap);
			if (!data->state) {
				free(data);
				return VINPUT_XKB;
			}
		} else {
			free(data);
			return VINPUT_XKB;
		}
	} else {
		free(data);
		return VINPUT_XKB;
	}

	VInputError ret = VINPUT_OK;

	data->v_valid_devices = vector_create();

	struct dirent *entry;
	DIR *dp = opendir("/dev/input");
	if (!dp) {
		ret = VINPUT_DEV_INPUT_DIR;
		goto cleanup_no_dir;
	}

	while ((entry = readdir(dp)) != NULL) {
		if (strncmp(entry->d_name, "event", 5) != 0) continue;

		char device_path[300];
		snprintf(device_path, sizeof(device_path), "/dev/input/%s", entry->d_name);

		bool is_kbd;
		if ((ret = is_keyboard(device_path, &is_kbd)) != VINPUT_OK) goto cleanup;
		if (is_kbd) {
			int fd = open(device_path, O_RDONLY | O_NONBLOCK);
			if (fd < 0) {
				perror("Failed to open input device");
				continue;
			}

			struct libevdev *dev = NULL;
			if (libevdev_new_from_fd(fd, &dev) < 0) {
				close(fd);
				continue;
			}

			vector_add(&data->v_valid_devices, dev);
		}
	}

	goto final;

cleanup:
	closedir(dp);
cleanup_no_dir:
	vector_free(data->v_valid_devices);
	free(listener->data);
final:

	listener->initialized = true;
	return ret;
}

void update_keyboard_modifiers(
    KeyboardModifiers *modifiers, uint16_t keycode, bool pressed)
{
	switch (keycode) {
	case KEY_LEFTSHIFT: modifiers->left_shift = pressed; break;
	case KEY_RIGHTSHIFT: modifiers->right_shift = pressed; break;
	case KEY_LEFTCTRL: modifiers->left_control = pressed; break;
	case KEY_RIGHTCTRL: modifiers->right_control = pressed; break;
	case KEY_LEFTALT: modifiers->left_alt = pressed; break;
	case KEY_RIGHTALT: modifiers->right_alt = pressed; break;
	case KEY_LEFTMETA: modifiers->left_meta = pressed; break;
	case KEY_RIGHTMETA: modifiers->right_meta = pressed; break;
	}
}

void handle_key_event(
    struct libevdev *dev, KeyboardCallback callback, EventListenerInternal *data)
{
	struct input_event ev;
	while (libevdev_next_event(dev, LIBEVDEV_READ_FLAG_NORMAL, &ev) == 0) {
		if (ev.type == EV_KEY) {
			KeyboardEvent event;
			update_keyboard_modifiers(&event.modifiers, ev.code, ev.value);
			event.pressed = ev.value == 1;
			xkb_state_update_key(
			    data->state, ev.code + 8, event.pressed ? XKB_KEY_DOWN : XKB_KEY_UP);
			event.keycode = xkb_state_key_get_one_sym(data->state, ev.code + 8);
			if (event.keycode == 0) return;
			char buffer[8];
			int size = xkb_keysym_to_utf8(event.keycode, buffer, sizeof(buffer));
			if (size > 0)
				event.keychar = buffer[0];
			else
				event.keychar = 0;
			event.timestamp = ev.time.tv_sec * 1000 + ev.time.tv_usec / 1000;

			if (callback) callback(event);
		}
	}
}

void handle_mouse_event(struct libevdev *dev, MouseButtonCallback button_callback,
    MouseMoveCallback move_callback)
{
	struct input_event ev;
	static int x = 0, y = 0;

	while (libevdev_next_event(dev, LIBEVDEV_READ_FLAG_NORMAL, &ev) == 0) {
		if (ev.type == EV_KEY) {
			MouseButtonEvent event;
			switch (ev.code) {
			case BTN_LEFT: event.button = MouseButtonLeft; break;
			case BTN_RIGHT: event.button = MouseButtonRight; break;
			case BTN_MIDDLE: event.button = MouseButtonMiddle; break;
			default: return;
			}

			event.kind = ev.value == 1 ? MousePressEvent : MouseReleaseEvent;
			if (button_callback) button_callback(event);
		} else if (ev.type == EV_REL) {
			MouseMoveEvent move_event;
			switch (ev.code) {
			case REL_X: x += ev.value; break;
			case REL_Y: y += ev.value; break;
			}

			move_event.x = x;
			move_event.y = y;
			move_event.velocity_x = ev.code == REL_X ? ev.value : 0;
			move_event.velocity_y = ev.code == REL_Y ? ev.value : 0;
			move_event.velocity = sqrt(move_event.velocity_x * move_event.velocity_x
			                           + move_event.velocity_y * move_event.velocity_y);

			if (move_callback) move_callback(move_event);
		}
	}
}

VInputError evdev_EventListener2_start(EventListener *listener, KeyboardCallback callback,
    MouseButtonCallback button_callback, MouseMoveCallback move_callback)
{
	if (!listener->initialized) return VINPUT_UNINITIALIZED;
	EventListenerInternal *data = listener->data;

	struct pollfd *fds = malloc(vector_size(data->v_valid_devices) * sizeof(struct pollfd));

	for (size_t i = 0; i < vector_size(data->v_valid_devices); i++) {
		struct libevdev *dev = data->v_valid_devices[i];
		fds[i].fd = libevdev_get_fd(dev);
		fds[i].events = POLLIN;
	}

	printf("Devices: %ld\n", vector_size(data->v_valid_devices));

	while (1) {
		int ret = poll(fds, vector_size(data->v_valid_devices), -1);
		if (ret < 0) {
			perror("Poll failed");
			break;
		}

		for (unsigned int i = 0; i < vector_size(data->v_valid_devices); i++) {
			if (fds[i].revents & POLLIN) {
				struct libevdev *dev = data->v_valid_devices[i];
				if (libevdev_has_event_type(dev, EV_KEY) && libevdev_has_event_type(dev, EV_REL))
					handle_mouse_event(dev, button_callback, move_callback);
				else
					handle_key_event(dev, callback, data);
			}
		}
	}

	free(fds);
	return VINPUT_OK;
}

VInputError evdev_EventListener_free(EventListener *listener)
{
	EventListenerInternal *data = listener->data;
	for (size_t i = 0; i < vector_size(data->v_valid_devices); i++) {
		libevdev_free(data->v_valid_devices[i]);
	}
	vector_free(data->v_valid_devices);
	free(listener->data);
	return VINPUT_OK;
}
