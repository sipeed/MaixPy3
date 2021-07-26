
from pybind11 import get_cmake_dir
from pybind11.setup_helpers import Pybind11Extension, build_ext
from setuptools import Extension
from .utils import get_incs, get_srcs

libi2c_module = Extension('pylibi2c',  include_dirs=[
                          'ext_modules/libi2c/src'], sources=get_srcs('ext_modules/libi2c/src'))

ext_so = "./ext_modules/libmaix/components/libmaix/lib/arch/v831"

_maix_module = Extension('_maix', include_dirs=['ext_modules/_maix/include', 'ext_modules/libmaix/components/libmaix/include'],
                         sources=get_srcs('ext_modules/_maix'),
                         libraries=[
    "jpeg"
],
)

# pip3.8 install pybind11
_maix_vivo_module = Pybind11Extension("_maix_vivo",
                                            include_dirs=[
                                                get_incs(
                                                    'ext_modules/libmaix/components/libmaix/include')
                                            ],
                                            sources=get_srcs(
                                                'ext_modules/_maix_vivo'),
                                            libraries=[
                                                "dl", "rt", "log", "ion", "pthread", "cdc_base",
                                                "MemAdapter", "media_utils", "mpp_vi", "mpp_isp", "ISP",
                                                "venc_base", "mpp_component", "adecoder", "asound", "venc_base", "hwdisplay",
                                                "maix_utils", "maix_cam", "maix_image",
                                            ],
                                            library_dirs=["/lib",  "/usr/lib", ext_so, ],
                                            extra_link_args=["-Wl,-rpath=/usr/lib/python3.8/site-packages/maix"],
                                            define_macros=[
                                                ('V831Camera', None)],
                                            )

_maix_camera_module = Extension('_maix_camera', include_dirs=['ext_modules/_maix_camera/include', 'ext_modules/libmaix/components/libmaix/include'],
                                sources=get_srcs('ext_modules/_maix_camera'),
                                libraries=[
    "dl", "rt", "log", "ion", "pthread", "cdc_base",
    "MemAdapter", "media_utils", "mpp_vi", "mpp_isp", "ISP",
    "venc_base", "mpp_component", "adecoder", "asound", "venc_base", "hwdisplay",
    "maix_utils", "maix_cam", "maix_image",
],
    library_dirs=["/lib",  "/usr/lib", ext_so, ],
    # extra_link_args  = [ "-Wl,-z,origin", "-Wl,-rpath='$ORIGIN/maix'" ]
    extra_compile_args=['-DV831Camera'],
    extra_link_args=["-Wl,-rpath=/usr/lib/python3.8/site-packages/maix"]
)

_maix_display_module = Extension('_maix_display', include_dirs=['ext_modules/_maix_display/include', 'ext_modules/libmaix/components/libmaix/include'],
                                 sources=get_srcs('ext_modules/_maix_display'),
                                 libraries=[
    "dl", "rt", "log", "ion", "pthread", "cdc_base",
    "maix_utils", "maix_disp", "maix_image",
],
    library_dirs=["/lib",  "/usr/lib", ext_so, ],
    extra_compile_args=['-DV831Display'],
    extra_link_args=["-Wl,-rpath=/usr/lib/python3.8/site-packages/maix"]
)

_maix_nn_module = Extension('_maix_nn', include_dirs=['ext_modules/_maix_nn/include', 'ext_modules/libmaix/components/libmaix/include'],
                            sources=get_srcs('ext_modules/_maix_nn/src'),
                            libraries=[
    "maix_utils", "maix_nn",
],
    library_dirs=["/lib",  "/usr/lib", ext_so, ],
    # extra_link_args  = [ "-Wl,-z,origin", "-Wl,-rpath='$ORIGIN/maix'" ]
    extra_link_args=["-Wl,-rpath=/usr/lib/python3.8/site-packages/maix"]
)

_maix_modules = [
    libi2c_module,
    _maix_module,
    _maix_vivo_module,
    _maix_camera_module,
    _maix_display_module,
    _maix_nn_module
]

_maix_data_files = [
    ('/maix', get_srcs(ext_so, ['so'])),
]

_maix_py_modules = [
    "Pillow",
    "rpyc",
    "gpiod",
    "evdev",
    "spidev",
    "pyserial"
]
