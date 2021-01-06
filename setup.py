#!/usr/bin/env python

"""
setup.py file for _maix
"""

from distutils.core import setup, Extension

_maix_module = Extension('_maix', sources=['src/i2c.c', 'src/pyi2c.c', 'src/_maix.c'],)

maix_module = ["maix/__init__", "maix/display", "maix/Video"]

setup(
    name='python3-maix',
    version='0.1.1',
    license='MIT',
    author='junhuanchen',
    author_email="juwan@sipeed.com",
    url='https://github.com/sipeed/python3-maix',
    description="MaixPy Python3 library",
    long_description=open('README.md').read(),
    ext_modules=[_maix_module],
    py_modules=["_maix"] + maix_module,
    classifiers=[
        'Programming Language :: Python :: 3',
    ],
)

