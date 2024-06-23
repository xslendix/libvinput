from setuptools import setup, Extension
from setuptools.command.build_ext import build_ext
import subprocess
import sys

with open("README.md", "r") as fh:
    long_description = fh.read()

libraries = ['vinput']
if sys.platform == 'win32':
    libraries.append('User32')

class CustomBuildExt(build_ext):
    def run(self):
        lib = ""
        if sys.platform == 'win32':
            lib = "vinput.lib"
        elif sys.platform == 'darwin':
            lib = "libvinput.dylib"
        else:
            lib = "libvinput.so"
        protoc_command = ["make", lib]
        if subprocess.call(protoc_command) != 0:
            sys.exit(-1)
        super().run()

module = Extension('libvinput',
                   sources=['src/pybind.c'],
                   include_dirs=['src'],
                   libraries=libraries,
                   library_dirs=['.'])

setup(
    name='libvinput',
    version='1.1',
    author='Slendi',
    description='Python interface for libvinput',
    long_description=long_description,
    long_description_content_type="text/markdown",
    ext_modules=[module],
    cmdclass={
        'build_ext': CustomBuildExt,  # Changed from build_py to build_ext
    },
    setup_requires=['wheel']  # Ensure wheel is available for building
)
