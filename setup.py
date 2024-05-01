import platform
from setuptools import setup, Extension

extra_link_args = []
if platform.system() == 'Darwin':  # Checks if the OS is macOS
    print('Detected MacOS')
    #extra_link_args.append('-Wl,-rpath,@loader_path/.')

module = Extension('libvinput',
                    sources=['src/pybind.c'],
                    include_dirs=['src'],
                    libraries=['vinput'],
                    library_dirs=['.'],
                    extra_link_args=extra_link_args)  # Apply OS-specific linker flags

setup(name='libvinput',
      version='1.0',
      description='Python interface for libvinput',
      ext_modules=[module])
