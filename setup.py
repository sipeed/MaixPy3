#!/usr/bin/env python

"""
setup.py file for MaixPy3
"""

import os

def get_srcs(path):
    _sources = []
    setup_path = os.path.dirname(os.path.abspath(__file__)) + '/'
    for root, dirs, files in os.walk(setup_path + path):
        for file in files:
            _type = file.split(".")
            if len(_type) > 1 and _type[1].lower() in ['c', 'cpp', 'cc', 'cxx', 's']:
                abs_path = root.replace(setup_path, '')
                _sources.append(abs_path + '/' + file)
    # print(_sources)
    return _sources

from setuptools import setup, Extension, find_packages

_maix_module = Extension('_maix', include_dirs=['ext_modules/_maix/include'], sources=get_srcs('ext_modules/_maix'), libraries=['jpeg'])

libi2c_module = Extension('pylibi2c',  include_dirs=['ext_modules/libi2c/src'], sources=get_srcs('ext_modules/libi2c/src'))

setup(
    name='MaixPy3',
    version='0.1.2',
    license='MIT',
    author='Sipeed',
    author_email="support@sipeed.com",
    url='https://github.com/sipeed/MaixPy3',
    description="MaixPy Python3 library",
    long_description=open('README.md').read(),
    install_requires=["Pillow"],
    ext_modules=[
        _maix_module,
        libi2c_module,
    ],
    packages = find_packages(), # find __init__.py packages
    classifiers=[
        'Programming Language :: Python :: 3',
    ],
)
