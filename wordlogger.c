#include <ctype.h>
#include <stdio.h>

#include <libvinput.h>

char word_buffer[128] = { 0 };
int word_len = 0;

void keyboard_callback(KeyboardEvent evt)
{
	if (!evt.pressed) return;

	if (isdigit(evt.keychar)) {
		word_len = 0;
		return;
	}

	if (evt.keychar == '\b') {
		word_len--;
		if (word_len < 0) word_len = 0;
		return;
	}

	if (!isprint(evt.keychar) && !isspace(evt.keychar)) return;

	if (!isalpha(evt.keychar)
	    || VInput_modifier_pressed_except_shift(evt.modifiers)) { // New word
		if (word_len) printf("Word introduced: %.*s\n", word_len, word_buffer);
		word_len = 0;
	} else {
		if (word_len > sizeof(word_buffer) / sizeof(char)) word_len = 0;
		word_buffer[word_len++] = evt.keychar;
	}
}

int main(void)
{
	EventListener listener;
	VInputError status = EventListener_create(&listener, true);
	if (status != VINPUT_OK) {
		fprintf(stderr, "ERROR: Failed to create keyboard listener! Error: %s",
		    VInput_error_get_message(status));
	}

	EventListener_start(&listener, &keyboard_callback);
	EventListener_free(&listener);

	return 0;
}
