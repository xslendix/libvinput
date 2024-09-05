#include <stdio.h>
#include <unistd.h>

#include "src/libvinput.h"

int main(void)
{
	EventEmulator emu;
	VInputError err = EventEmulator_create(&emu);
	if (err) printf("Error: %s\n", VInput_error_get_message(err));

	puts("5");
	sleep(1);
	puts("4");
	sleep(1);
	puts("3");
	sleep(1);
	puts("2");
	sleep(1);
	puts("1");
	sleep(1);

	err = EventEmulator_typec(&emu, 'H');
	if (err) printf("Error: %s\n", VInput_error_get_message(err));
	err = EventEmulator_typec(&emu, 'i');
	if (err) printf("Error: %s\n", VInput_error_get_message(err));
	err = EventEmulator_types(&emu, "\nHello world!", 13);
	if (err) printf("Error: %s\n", VInput_error_get_message(err));
}
