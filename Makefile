CFLAGS = -I. -Wall -Wextra -O0 -ggdb #-O3

# Linux/X11
ifeq ($(shell uname), Linux)
all: wordlogger

libvinput.so: src/libvinput.c src/linux_emu.c src/linux.c
	$(CC) $(CFLAGS) -fPIC -o $@ -shared $^ -lm -lX11 -lXtst -I/usr/local/include -Isrc -L/usr/local/lib -lxdo

wordlogger: wordlogger.c libvinput.so
	$(CC) $(CFLAGS) wordlogger.c -o $@ -L. -lvinput -lX11 -lXtst -I/usr/local/include -Isrc -L/usr/local/lib -lxdo

install: libvinput.so
	install -m 777 libvinput.so /usr/local/lib
	install -m 644 src/libvinput.h /usr/local/include
	install -m 644 libvinput.pc /usr/local/lib/pkgconfig

clean:
	rm -f wordlogger
	rm -f libvinput.so
endif

# MacOS
ifeq ($(shell uname), Darwin)
all: wordlogger_mac

libvinput.dylib: src/libvinput.c src/macos_emu.c src/macos.c
	$(CC) $(CFLAGS) -fPIC -framework ApplicationServices -framework Carbon -o $@ -shared $^

wordlogger_mac: wordlogger.c libvinput.dylib
	$(CC) $(CFLAGS) wordlogger.c -o $@ -Isrc -L. -lvinput -Wl,-rpath,@loader_path/.

clean:
	rm -f wordlogger_mac
	rm -f libvinput.dylib
endif

# Windows/WINAPI
ifeq ($(OS), Windows_NT)
all: wordlogger libvinput.dll vinput.lib
	dumpbin /EXPORTS vinput.lib

libvinput.dll: libvinput.obj windows_emu.obj windows.obj
	link /DLL /OUT:libvinput.dll libvinput.obj windows_emu.obj windows.obj User32.lib Kernel32.lib

libvinput.obj: src/libvinput.c
	cl /LD /I src /DBUILDING_VINPUT /Fo$@ /c src\libvinput.c

windows_emu.obj: src/windows_emu.c
	cl /LD /I src /DBUILDING_VINPUT /Fo$@ /c src\windows_emu.c

windows.obj: src/windows.c
	cl /LD /I src /DBUILDING_VINPUT /Fo$@ /c src\windows.c

vinput.lib: libvinput.obj windows_emu.obj windows.obj
	lib /OUT:vinput.lib libvinput.obj windows_emu.obj windows.obj

wordlogger: wordlogger.c vinput.lib
	cl /I src /Fo$@ /Fe$@ wordlogger.c vinput.lib User32.lib Kernel32.lib

clean:
	del /F /Q libvinput.dll vinput.lib libvinput.obj windows_emu.obj windows.obj
endif

ifeq ($(OS), MINGW)
all: wordlogger.exe libvinput.dll

libvinput.dll: src/libvinput.c src/windows_emu.c src/windows.c
	x86_64-w64-mingw32-gcc $(CFLAGS) -shared -o $@ $^ -DWINVER=0x0600 -luser32 -lkernel32

wordlogger.exe: wordlogger.c libvinput.dll
	x86_64-w64-mingw32-gcc $(CFLAGS) -o $@ wordlogger.c -L. -lvinput -DWINVER=0x0600 -luser32 -lkernel32

clean:
	rm -f wordlogger.exe
	rm -f libvinput.dll
endif

.PHONY: all clean

.PHONY: install clean
