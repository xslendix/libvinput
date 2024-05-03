from setuptools import setup, Extension

with open("README.md", "r") as fh:
    long_description = fh.read()

module = Extension('libvinput',
                    sources=['src/pybind.c'],
                    include_dirs=['src'],
                    libraries=['vinput'],
                    library_dirs=['.'])

setup(name='libvinput',
      version='1.0',
      author='Slendi',
      description='Python interface for libvinput',
      long_description=long_description,
      long_description_content_type="text/markdown",
      ext_modules=[module])
