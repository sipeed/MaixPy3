#!/usr/bin/env python

"""
setup.py file for MaixPy3
"""

import sys
from setuptools import setup, Extension, find_packages

ext_modules = []
data_files = []

if 'v831' in sys.argv:
  sys.argv.remove('v831')
  from envs.v831 import _maix_module, _maix_nn_module, _maix_data_files
  ext_modules.extend([_maix_module, _maix_nn_module])
  # print(_maix_module, _maix_nn_module)
  data_files.extend(_maix_data_files)
  # print(_maix_data_files)
else:
  pass
  # from envs.utils import get_srcs
  # libi2c_module = Extension('pylibi2c',  include_dirs=['ext_modules/libi2c/src'], sources=get_srcs('ext_modules/libi2c/src'))

setup(
    name='MaixPy3',
    version='0.1.2',
    license='MIT',
    author='Sipeed',
    author_email="support@sipeed.com",
    url='https://github.com/sipeed/MaixPy3',
    description="MaixPy Python3 library",
    long_description=open('README.md').read(),
    install_requires=["Pillow", "pexpect", "rpyc"],
    ext_modules=ext_modules,
    packages = find_packages(
      exclude=['envs'] # remove from build
      ), # find __init__.py packages
    classifiers=[
        'Programming Language :: Python :: 3',
    ],
    include_package_data=True,
    package_data = {
        '': ['*.so'],
    },
    data_files=data_files,
)
