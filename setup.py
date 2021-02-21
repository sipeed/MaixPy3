#!/usr/bin/env python

"""
setup.py file for MaixPy3
"""

import sys
from setuptools import setup, Extension, find_packages

ext_modules = []
data_files = []
py_modules = []

if 'maix_v831' in sys.argv:
  sys.argv.remove('maix_v831')
  from envs.maix_v831 import _maix_modules, _maix_data_files, _maix_py_modules
else:
  from envs.general import _maix_modules, _maix_data_files, _maix_py_modules
  
ext_modules.extend(_maix_modules)
data_files.extend(_maix_data_files)
py_modules.extend(_maix_py_modules)

setup(
  name='MaixPy3',
  version='0.3.0',
  license='MIT',
  author='Sipeed',
  author_email="support@sipeed.com",
  url='https://github.com/sipeed/MaixPy3',
  description="MaixPy Python3 library",
  long_description=open('README.md', 'r', encoding='UTF-8').read(),
  long_description_content_type='text/markdown',
  install_requires=_maix_py_modules,
  ext_modules=ext_modules,
  packages = find_packages(
    exclude=['envs'] # remove from build
    ), # find __init__.py packages
  classifiers=[
      'Programming Language :: Python :: 3',
  ],
  entry_points={"console_scripts": ["maixpy3_rpycs=maix.rpycs:start"]},
  include_package_data=True,
  package_data = {
      '': ['*.so'],
  },
  data_files=data_files,
)
