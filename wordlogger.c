#include <ctype.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>

#include "src/libvinput.h"

#ifdef _WIN32
#	include <windows.h>
#endif

char g_word_buffer[128] = { 0 };
int g_word_len = 0;

EventListener g_listener;

void commit_word(void)
{
	if (g_word_len) printf("Word introduced: %.*s\n", g_word_len, g_word_buffer);
	g_word_len = 0;
}

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

	if (evt.keychar == '\n' || evt.keychar == '\r') commit_word();

	if (!isprint(evt.keychar)) return;

	if (!isalpha(evt.keychar) || isspace(evt.keychar)
	    || VInput_modifier_pressed_except_shift(evt.modifiers)) { // New word
		commit_word();
	} else {
		if (g_word_len > (int)(sizeof(g_word_buffer) / sizeof(char))) g_word_len = 0;
		g_word_buffer[g_word_len++] = evt.keychar;
	}
}

void mouse_button_callback(MouseButtonEvent evt)
{
	(void)evt;
	commit_word();
}

void mouse_move_callback(MouseMoveEvent evt)
{
	(void)evt;
	commit_word();
}

#ifndef _WIN32
void signal_handler(int sig)
{
	printf("Signal %d received, exiting...\n", sig);
	EventListener_free(&g_listener);
	exit(0);
}
#endif

#ifdef _WIN32
BOOL WINAPI CtrlHandler(DWORD fdwCtrlType)
{
	switch (fdwCtrlType) {
	case CTRL_C_EVENT:
	case CTRL_CLOSE_EVENT:
	case CTRL_BREAK_EVENT:
	case CTRL_LOGOFF_EVENT:
	case CTRL_SHUTDOWN_EVENT:
		printf("Ctrl received, exiting...\n");
		EventListener_free(&g_listener);
	default: return FALSE;
	}
}
#endif

int main(void)
{
#ifdef _WIN32
	if (!SetConsoleCtrlHandler(CtrlHandler, TRUE)) {
		puts("Failed to set handler. Please force quit.");
		while (1)
			;
	}
#else
	// signal(SIGINT, signal_handler);
	signal(SIGTERM, signal_handler);
#endif

	uint32_t version = VInput_version();
	printf("VInput version: %d.%d.%d\n", VINPUT_VERSION_MAJOR(version),
	    VINPUT_VERSION_MINOR(version), VINPUT_VERSION_PATCH(version));
	fflush(stdout);

	VInputError status = EventListener2_create(&g_listener, true, true, true);
	if (status != VINPUT_OK) {
		fprintf(stderr, "ERROR: Failed to create keyboard listener! Error: %s\n",
		    VInput_error_get_message(status));
	}

	if ((status = EventListener2_start(
	         &g_listener, &keyboard_callback, &mouse_button_callback, &mouse_move_callback))
	    != VINPUT_OK) {
		fprintf(stderr, "ERROR: Failed to start keyboard listener! Error: %s\n",
		    VInput_error_get_message(status));
	}

	return 0;
}
