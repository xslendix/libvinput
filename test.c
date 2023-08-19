#include "libvinput.h"

#include <stdio.h>

void cb(KeyboardEvent ev)
{
	printf("%c, pressed: %d\n", ev.keychar, ev.pressed);
	fflush(stdout);
}

int main(void)
{
	int ret;
	Listener listener;
	if ((ret = Listener_create(&listener, true)) != VINPUT_OK) {
		printf("1NOT OK! %d\n", ret);
		return -1;
	}
	if ((ret = Listener_start(&listener, cb)) != VINPUT_OK) {
		printf("2NOT OK! %d\n", ret);
		return -1;
	}
	if ((ret = Listener_free(&listener)) != VINPUT_OK) {
		printf("3NOT OK! %d\n", ret);
		return -1;
	}
}
