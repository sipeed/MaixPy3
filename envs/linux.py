
from setuptools import Extension
from .utils import get_srcs

libi2c_module = Extension('pylibi2c',  include_dirs=[
                          'ext_modules/libi2c/src'], sources=get_srcs('ext_modules/libi2c/src'))

_maix_module = Extension('_maix', include_dirs=['ext_modules/_maix/include'],
                         sources=get_srcs('ext_modules/_maix'),
                         libraries=[
    "jpeg"
],
)

_maix_modules = [
    libi2c_module,
    _maix_module
]

_maix_data_files = [

]

_maix_py_modules = [
    "numpy",
    "opencv-python>=4.1.2.30",
    "opencv-contrib-python>=4.1.2.30",
    "Pillow",
    "rpyc",
    "gpiod",
    "evdev",
    "spidev",
    "pyserial"
]
