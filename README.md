libvinput
=========

Cross-platform, simple C keyboard hook and emulator with mouse support in the
works.

Building and running demo
-------------------------

Note: On Linux, you need `x11`, `xcb-xtest` and `libxdo` development headers
installed.

Note (part 2): On Windows, you might want to use something like MSYS2 to
compile. Does not require any additional dependencies.

###### Linux
```
make wordlogger
./wordlogger
```

###### Windows
```
make wordlogger.exe
./wordlogger.exe
```

Documentation
-------------

All the documentation can be found inside the `libvinput.h` file.

License
-------

This software is licensed under the AGPLv3 license, more info in the LICENSE
file.

