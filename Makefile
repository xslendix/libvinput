CFLAGS = -I. -Wall -Wextra -O0 -ggdb #-O3

# Linux/X11
libvinput.so: src/libvinput.c src/linux_emu.c src/linux.c
	$(CC) $(CFLAGS) -fPIC -o $@ -shared $^

wordlogger: libvinput.so wordlogger.c
	$(CC) $(CFLAGS) wordlogger.c -o $@ -L. -l:$< -lX11 -lXtst -I/usr/local/include -L/usr/local/lib -lxdo

# Windows/WINAPI
libvinput.dll: src/libvinput.c src/windows_emu.c src/windows.c
	x86_64-w64-mingw32-gcc $(CFLAGS) -fPIC -o $@ -shared $^

wordlogger.exe: libvinput.dll wordlogger.c
	x86_64-w64-mingw32-gcc $(CFLAGS) wordlogger.c -o $@ -L. -l:$<

clean:
	rm -f test.exe
	rm -f test
	rm -f libvinput.dll
	rm -f libvinput.so

.PHONY: install
install: libvinput.so
	install -m 777 libvinput.so /usr/local/lib
	install -m 644 src/libvinput.h /usr/local/include
	install -m 644 libvinput.pc /usr/local/lib/pkgconfig
