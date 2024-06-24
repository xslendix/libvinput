from setuptools import setup, Extension
from setuptools.command.build_ext import build_ext
import subprocess
import sysconfig
import sys
import os

with open("README.md", "r") as fh:
    long_description = fh.read()

libraries = []
extra_link_args=[]
include_dirs = ['src']
library_dirs = ['.']

if sys.platform == 'win32':
    libraries.append('User32')
    include_dirs.append(sysconfig.get_paths()['include'])
    library_dirs.append(os.path.join(sysconfig.get_paths()['data'], 'libs'))
elif sys.platform == 'darwin':
    extra_link_args=['-framework', 'ApplicationServices', '-framework', 'Carbon']
else:
    libraries.extend(['X11', 'Xtst', 'xdo'])

sources = [
    'src/pybind.c',
    'src/libvinput.c',
]

if sys.platform == 'win32':
    sources.extend(['src/windows.c', 'src/windows_emu.c'])
elif sys.platform == 'darwin':
    sources.extend(['src/macos.c', 'src/macos_emu.c'])
else:
    sources.extend(['src/linux.c', 'src/linux_emu.c'])

module = Extension('libvinput',
                   sources=sources,
                   include_dirs=include_dirs,
                   libraries=libraries,
                   library_dirs=library_dirs,
                   extra_link_args=extra_link_args)

setup(
    name='libvinput',
    version='1.3',
    author='Slendi',
    description='Python interface for libvinput',
    long_description=long_description,
    long_description_content_type="text/markdown",
    ext_modules=[module],
    setup_requires=['wheel']  # Ensure wheel is available for building
)
