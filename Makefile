CFLAGS = -Wall -Wextra -Werror -O3

libvinput.so: libvinput.c linux.c
	$(CC) $(CFLAGS) -fPIC -o $@ -shared $^

test: libvinput.so test.c
	$(CC) $(CFLAGS) test.c -o $@ -L. -l:$< -lX11 -lXtst


libvinput.dll: libvinput.c windows.c
	x86_64-w64-mingw32-gcc $(CFLAGS) -fPIC -o $@ -shared $^

test.exe: libvinput.dll test.c
	x86_64-w64-mingw32-gcc $(CFLAGS) test.c -o $@ -L. -l:$<
