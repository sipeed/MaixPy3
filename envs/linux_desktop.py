
from pybind11 import get_cmake_dir
from pybind11.setup_helpers import Pybind11Extension, build_ext
from setuptools import Extension
from .utils import get_incs, get_srcs

libi2c_module = Extension('pylibi2c',
  include_dirs=[
    'ext_modules/libi2c/src'
  ],
  sources=get_srcs('ext_modules/libi2c/src')
)

_maix_module = Extension('_maix',
    include_dirs=[
    'ext_modules/_maix/include'],
    sources=get_srcs('ext_modules/_maix'),
    libraries=[
    "jpeg"
])

_maix_image_module = Pybind11Extension(
    name = "_maix_image",
    include_dirs=[ '/usr/include/opencv4/', '/usr/local/include/opencv4/',
        get_incs(
            'ext_modules/libmaix/components/libmaix/include'),
        get_incs(
            'ext_modules/libmaix/components/maix_cv_image/include'),
        get_incs(
            'ext_modules/libmaix/libmaix/lib/arch/desktop/libmaix_utils'),
        get_incs(
            'ext_modules/libmaix/libmaix/lib/arch/desktop/libmaix_image'),
        get_incs(
            'ext_modules/libmaix/components/maix_cv_image/include'),
        get_incs(
            'ext_modules/libmaix/components/third_party/include'),
        get_incs(
            'ext_modules/libmaix/components/third_party/imlib/include'),
        get_incs(
            'ext_modules/_maix_image/include')
    ],
    sources= get_srcs('ext_modules/libmaix/components/maix_cv_image/src') +
             get_srcs('ext_modules/libmaix/components/libmaix/lib/arch/desktop/libmaix_utils') +
             get_srcs('ext_modules/libmaix/components/libmaix/lib/arch/desktop/libmaix_image') +
             get_srcs('ext_modules/libmaix/components/third_party/imlib/src') +
             get_srcs('ext_modules/_maix_image'),
    libraries=[
        "opencv_videoio", "opencv_highgui", "opencv_core", "opencv_imgproc", "opencv_imgcodecs", "opencv_freetype", "opencv_flann", "opencv_features2d", "opencv_calib3d"
    ],
    library_dirs=[
        "/usr/lib/",
        "/usr/lib/x86_64-linux-gnu/",
        "/usr/local/lib64/",
    ],
    extra_link_args=[
    ],
    extra_compile_args=['-std=c++11', '-std=gnu++11', '-DIMLIB_CONFIG_H_FILE="costom_imlib_config.h"' ],
)

_maix_camera_module = Pybind11Extension(
    name = '_maix_camera',
    include_dirs=[ '/usr/include/opencv4/', '/usr/local/include/opencv4/',
        get_incs(
            'ext_modules/libmaix/components/libmaix/include'),
        get_incs(
            'ext_modules/libmaix/components/maix_cv_image/include'),
        get_incs(
            'ext_modules/libmaix/libmaix/lib/arch/desktop/libmaix_utils'),
        get_incs(
            'ext_modules/libmaix/libmaix/lib/arch/desktop/libmaix_image'),
        get_incs(
            'ext_modules/libmaix/libmaix/lib/arch/desktop/libmaix_cam'),
        get_incs(
            'ext_modules/libmaix/components/maix_cv_image/include'),
        get_incs(
            'ext_modules/_maix_camera/include')
    ],
    sources= get_srcs('ext_modules/libmaix/components/maix_cv_image/src') +
             get_srcs('ext_modules/libmaix/components/libmaix/lib/arch/desktop/libmaix_utils') +
             get_srcs('ext_modules/libmaix/components/libmaix/lib/arch/desktop/libmaix_image') +
             get_srcs('ext_modules/libmaix/components/libmaix/lib/arch/desktop/libmaix_cam') +
             get_srcs('ext_modules/_maix_camera'),
    libraries=[
        "opencv_videoio", "opencv_highgui", "opencv_core", "opencv_imgproc", "opencv_imgcodecs", "opencv_freetype"
    ],
    library_dirs=[
        "/usr/lib/",
        "/usr/lib/x86_64-linux-gnu/",
        "/usr/local/lib64/",
    ],
    extra_link_args=[
    ],
    extra_compile_args=['-std=c++11', '-std=gnu++11' ],
)

_maix_display_module = Pybind11Extension(
    name = "_maix_display",
    include_dirs=[ '/usr/include/opencv4/', '/usr/local/include/opencv4/',
        get_incs(
            'ext_modules/libmaix/components/libmaix/include'),
        get_incs(
            'ext_modules/libmaix/components/maix_cv_image/include'),
        get_incs(
            'ext_modules/libmaix/libmaix/lib/arch/desktop/libmaix_utils'),
        get_incs(
            'ext_modules/libmaix/libmaix/lib/arch/desktop/libmaix_image'),
        get_incs(
            'ext_modules/libmaix/libmaix/lib/arch/desktop/libmaix_disp'),
        get_incs(
            'ext_modules/libmaix/components/maix_cv_image/include'),
        get_incs(
            'ext_modules/_maix_display/include')
    ],
    sources= get_srcs('ext_modules/libmaix/components/maix_cv_image/src') +
             get_srcs('ext_modules/libmaix/components/libmaix/lib/arch/desktop/libmaix_utils') +
             get_srcs('ext_modules/libmaix/components/libmaix/lib/arch/desktop/libmaix_image') +
             get_srcs('ext_modules/libmaix/components/libmaix/lib/arch/desktop/libmaix_disp') +
             get_srcs('ext_modules/_maix_display'),
    libraries=[
        "opencv_videoio", "opencv_highgui", "opencv_core", "opencv_imgproc", "opencv_imgcodecs", "opencv_freetype"
    ],
    library_dirs=[
        "/usr/lib/",
        "/usr/lib/x86_64-linux-gnu/",
        "/usr/local/lib64/",
    ],
    extra_link_args=[
    ],
    extra_compile_args=['-std=c++11', '-std=gnu++11' ],
)

_maix_modules = [
    libi2c_module,
    _maix_module,
    _maix_image_module,
    _maix_camera_module,
    _maix_display_module,
]

_maix_data_files = [

]

_maix_py_modules = [
    "numpy",
    "rpyc",
    "gpiod",
    "evdev",
    "spidev",
    "pybind11",
    "pyserial"
]
