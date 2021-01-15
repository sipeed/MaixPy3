#!/usr/bin/env python

"""
setup.py file for MaixPy3
"""

import os

def get_srcs(path, file_type=['c', 'cpp', 'cc', 'cxx', 's']):
    _sources = []
    setup_path = os.path.dirname(os.path.abspath(__file__)) + '/'
    for root, dirs, files in os.walk(setup_path + path):
        for file in files:
            _type = file.split(".")
            if len(_type) > 1 and _type[1].lower() in file_type:
                abs_path = root.replace(setup_path, '')
                _sources.append(abs_path + '/' + file)
    # print(_sources)
    return _sources

from setuptools import setup, Extension, find_packages

ext_so = "./ext_modules/libmaix/components/libmaix/lib/arch/v831"

# _maix_module = Extension('_maix', include_dirs=['ext_modules/_maix/include'], sources=get_srcs('ext_modules/_maix'), libraries=['jpeg'])
_maix_module = Extension('_maix', include_dirs=['ext_modules/_maix/include', 'ext_modules/libmaix/components/libmaix/include'], 
                        sources=get_srcs('ext_modules/_maix'), 
                        libraries=[
                            "jpeg", "dl", "rt", "log", "ion", "pthread", "cdc_base",
                            "MemAdapter", "media_utils", "mpp_vi", "mpp_isp", "ISP",
                            "mpp_mini", "maix_cam", "maix_disp", "maix_image", "maix_nn",
                        ],
                        library_dirs=[ "/lib",  "/usr/lib", ext_so, ],
                        # extra_link_args  = [ "-Wl,-z,origin", "-Wl,-rpath='$ORIGIN/maix'" ]
                        extra_link_args  = [ "-Wl,-rpath=/usr/lib/python3.8/site-packages/maix" ]
                    )

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
    ext_modules=[
        _maix_module,
        # libi2c_module,
    ],
    packages = find_packages(), # find __init__.py packages
    classifiers=[
        'Programming Language :: Python :: 3',
    ],
    include_package_data=True,
    package_data = {
        '': ['*.so'],
    },
    data_files=[
        ('/maix', get_srcs(ext_so, ['so'])),
    ],
)
