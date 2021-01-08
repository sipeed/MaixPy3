#!/usr/bin/env python

"""
setup.py file for _maix
"""

from setuptools import setup, Extension

# sudo apt-get install libjpeg-dev
_maix_module = Extension('_maix', include_dirs=['include'], sources=['_maix.c', 'pyCamera.c'], libraries=['jpeg'],)

setup(
    name='_maix',
    license='MIT',
    ext_modules=[_maix_module],
    classifiers=[
        'Programming Language :: Python',
        'Programming Language :: Python :: 3',
    ],
)
