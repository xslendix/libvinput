CFLAGS = -I. -Wall -Wextra -O0 -ggdb #-O3

# Linux/X11
libvinput.so: src/libvinput.c src/linux_emu.c src/linux.c
	$(CC) $(CFLAGS) -fPIC -o $@ -shared $^ -lX11 -lXtst -I/usr/local/include -L/usr/local/lib -lxdo

wordlogger: wordlogger.c
	$(CC) $(CFLAGS) wordlogger.c -o $@ -L. -lvinput -lX11 -lXtst -I/usr/local/include -L/usr/local/lib -lxdo

# MacOS
libvinput.dylib: src/libvinput.c src/macos_emu.c src/macos.c
	$(CC) $(CFLAGS) -fPIC -framework ApplicationServices -framework Carbon -o $@ -shared $^

wordlogger_mac: wordlogger.c
	$(CC) $(CFLAGS) wordlogger.c -o $@ -Isrc -L. -lvinput -Wl,-rpath,@loader_path/.

# Windows/WINAPI
libvinput.dll: src/libvinput.c src/windows_emu.c src/windows.c
	x86_64-w64-mingw32-gcc $(CFLAGS) -fPIC -o $@ -shared $^

vinput.lib: src/libvinput.c src/windows_emu.c src/windows.c
	x86_64-w64-mingw32-gcc $(CFLAGS) -c src/libvinput.c -o libvinput.o
	x86_64-w64-mingw32-gcc $(CFLAGS) -c src/windows_emu.c -o windows_emu.o
	x86_64-w64-mingw32-gcc $(CFLAGS) -c src/windows.c -o windows.o
	ar rcs vinput.lib libvinput.o windows_emu.o windows.o

wordlogger.exe: libvinput.dll wordlogger.c
	x86_64-w64-mingw32-gcc $(CFLAGS) wordlogger.c -o $@ -L. -Isrc -l:$<

clean:
	rm -f wordlogger_mac
	rm -f wordlogger
	rm -f wordlogger.exe
	rm -f libvinput.dll
	rm -f libvinput.so
	rm -f libvinput.dylib

.PHONY: install
install: libvinput.so
	install -m 777 libvinput.so /usr/local/lib
	install -m 644 src/libvinput.h /usr/local/include
	install -m 644 libvinput.pc /usr/local/lib/pkgconfig
