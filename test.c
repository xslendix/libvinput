#include "libvinput.h"

#include <stdio.h>
#include <unistd.h>

void cb(KeyboardEvent ev)
{
	printf("%c, pressed: %d\n", ev.keychar, ev.pressed);
	fflush(stdout);
}

int main(void)
{
	// int ret;
	// Listener listener;
	// if ((ret = Listener_create(&listener, true)) != VINPUT_OK) {
	// 	printf("1NOT OK! %d\n", ret);
	// 	return -1;
	// }
	// if ((ret = Listener_start(&listener, cb)) != VINPUT_OK) {
	// 	printf("2NOT OK! %d\n", ret);
	// 	return -1;
	// }
	// if ((ret = Listener_free(&listener)) != VINPUT_OK) {
	// 	printf("3NOT OK! %d\n", ret);
	// 	return -1;
	// }

	puts("1");
	sleep(1);
	puts("2");
	sleep(1);
	puts("3");
	sleep(1);
	puts("test");

	Emulator emu;
	int ret;
	if ((ret = Emulator_create(&emu)) != VINPUT_OK) {
		printf("1NOT OK! %d\n", ret);
		return -1;
	}
	if ((ret = Emulator_press(&emu, 0x0061)) != VINPUT_OK) {
		printf("2NOT OK! %d\n", ret);
		return -1;
	};
	if ((ret = Emulator_release(&emu, 0x0061)) != VINPUT_OK) {
		printf("3NOT OK! %d\n", ret);
		return -1;
	};
	if ((ret = Emulator_typec(&emu, 'b')) != VINPUT_OK) {
		printf("4NOT OK! %d\n", ret);
		return -1;
	};
	if ((ret = Emulator_types(&emu, "test", 4)) != VINPUT_OK) {
		printf("5NOT OK! %d\n", ret);
		return -1;
	};

	int *state, nstate;
	if ((ret = Emulator_keyboard_state_get(&emu, &state, &nstate)) != VINPUT_OK) {
		printf("6NOT OK! %d\n", ret);
		return -1;
	}
	if ((ret = Emulator_keyboard_state_clear(&emu)) != VINPUT_OK) {
		printf("7NOT OK! %d\n", ret);
		return -1;
	}
	if ((ret = Emulator_press(&emu, 0xffe5)) != VINPUT_OK) {
		printf("8NOT OK! %d\n", ret);
		return -1;
	};
	if ((ret = Emulator_types(&emu, "capslock", 4)) != VINPUT_OK) {
		printf("9NOT OK! %d\n", ret);
		return -1;
	};
	if ((ret = Emulator_release(&emu, 0xffe5)) != VINPUT_OK) {
		printf("10NOT OK! %d\n", ret);
		return -1;
	};
	if ((ret = Emulator_keyboard_state_set(&emu, state, nstate)) != VINPUT_OK) {
		printf("11NOT OK! %d\n", ret);
		return -1;
	}
	return 0;
}
