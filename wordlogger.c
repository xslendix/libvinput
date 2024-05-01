#include <ctype.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>

#include <libvinput.h>

char g_word_buffer[128] = { 0 };
int g_word_len = 0;

EventListener g_listener;

void keyboard_callback(KeyboardEvent evt)
{
	if (!evt.pressed) return;

	if (isdigit(evt.keychar)) {
		g_word_len = 0;
		return;
	}

	if (evt.keychar == '\b') {
		g_word_len--;
		if (g_word_len < 0) g_word_len = 0;
		return;
	}

	if (!isprint(evt.keychar)) return;

	if (!isalpha(evt.keychar) || isspace(evt.keychar)
	    || VInput_modifier_pressed_except_shift(evt.modifiers)) { // New word
		if (g_word_len) printf("Word introduced: %.*s\n", g_word_len, g_word_buffer);
		g_word_len = 0;
	} else {
		if (g_word_len > (int)(sizeof(g_word_buffer) / sizeof(char))) g_word_len = 0;
		g_word_buffer[g_word_len++] = evt.keychar;
	}
}

void signal_handler(int sig)
{
	printf("Signal %d received, exiting...\n", sig);
	EventListener_free(&g_listener);
	exit(0);
}

int main(void)
{
	// signal(SIGINT, signal_handler);
	signal(SIGTERM, signal_handler);

	VInputError status = EventListener_create(&g_listener, true);
	if (status != VINPUT_OK) {
		fprintf(stderr, "ERROR: Failed to create keyboard listener! Error: %s",
		    VInput_error_get_message(status));
	}

	EventListener_start(&g_listener, &keyboard_callback);

	return 0;
}
