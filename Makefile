CFLAGS = -Wall -Wextra -O0 -ggdb #-O3

libvinput.so: libvinput.c linux_emu.c linux.c
	$(CC) $(CFLAGS) -fPIC -o $@ -shared $^

test: libvinput.so test.c
	$(CC) $(CFLAGS) test.c -o $@ -L. -l:$< -lX11 -lXtst -I/usr/local/include -L/usr/local/lib -lxdo


libvinput.dll: libvinput.c windows_emu.c windows.c
	x86_64-w64-mingw32-gcc $(CFLAGS) -fPIC -o $@ -shared $^

test.exe: libvinput.dll test.c
	x86_64-w64-mingw32-gcc $(CFLAGS) test.c -o $@ -L. -l:$<

clean:
	rm -f test.exe
	rm -f test
	rm -f libvinput.dll
	rm -f libvinput.so
