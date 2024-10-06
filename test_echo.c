#include "src/libvinput.h"

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>

EventListener g_listener;
EventEmulator g_emulator;

bool g_is_typing;

void keyboard_callback(KeyboardEvent evt)
{
	if (g_is_typing) return;
	if (!isprint(evt.keychar)) return;
	if (!evt.pressed) return;

	printf("got key press/release: %c\n", evt.keychar);

	g_is_typing = true;
	EventEmulator_typec(&g_emulator, evt.keychar);
	g_is_typing = false;
}

int main(void)
{
	uint32_t version = VInput_version();
	printf("VInput version: %d.%d.%d\n", VINPUT_VERSION_MAJOR(version),
	    VINPUT_VERSION_MINOR(version), VINPUT_VERSION_PATCH(version));

	VInputError err = EventListener2_create(&g_listener, true, false, false);
	if (err) {
		fprintf(stderr, "ERROR: Failed to create keyboard listener! Error: %s\n",
		    VInput_error_get_message(err));
	}

	err = EventEmulator_create(&g_emulator);
	if (err) {
		fprintf(stderr, "ERROR: Failed to start event emulator! Error: %s\n",
		    VInput_error_get_message(err));
		return 1;
	}

	err = EventListener2_start(&g_listener, &keyboard_callback, NULL, NULL);
	if (err) {
		fprintf(stderr, "ERROR: Failed to start keyboard listener! Error: %s\n",
		    VInput_error_get_message(err));
	}
}
