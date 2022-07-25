
from pybind11 import get_cmake_dir
from pybind11.setup_helpers import Pybind11Extension, build_ext
from setuptools import Extension
from .utils import get_incs, get_srcs

libi2c_module = Extension('pylibi2c',  include_dirs=[
                          'ext_modules/libi2c/src'], sources=get_srcs('ext_modules/libi2c/src'))

ext_so = "./ext_modules/libmaix/components/libmaix/lib/arch/v831"

_maix_module = Extension('_maix', include_dirs=['ext_modules/_maix/include', 'ext_modules/libmaix/components/libmaix/include'],
                         sources=get_srcs('ext_modules/_maix'),
                         libraries=[ "jpeg"],
)

# python3.8 -m pip install pybind11
_maix_vivo_module = Pybind11Extension("_maix_vivo",
    include_dirs=[
        get_incs(
            'ext_modules/libmaix/components/libmaix/include')
    ],
    sources=get_srcs('ext_modules/_maix_vivo'),
    libraries=[
        "dl", "rt", "log", "ion", "pthread", "cdc_base",
            "MemAdapter", "media_utils", "mpp_vi", "mpp_isp", "ISP",
            "venc_base", "mpp_component", "adecoder", "asound", "venc_base", "hwdisplay",
            "maix_utils", "maix_cam", "maix_image",
    ],
    library_dirs=[ ext_so, ],
    extra_link_args=[
        "-Wl,-rpath=/usr/lib/python3.8/site-packages/maix"],
    # define_macros=[('V831Camera', None)],
)

# python3.8 -m pip install pybind11
_maix_speech_module = Pybind11Extension("_maix_speech",
    include_dirs=[
        get_incs(
            './ext_modules/libmaix/components/maix_speech/Maix-Speech/components/asr_lib/include'),
        get_incs(
            './ext_modules/libmaix/components/maix_speech/Maix-Speech/components/utils/include')
    ],
    sources = get_srcs('ext_modules/_maix_speech', exclude=["utils", "projects"]),
    libraries=[
        "ms_asr_v83x", "asound"
    ],
    library_dirs=[
        ext_so,
        "./ext_modules/libmaix/components/maix_speech/Maix-Speech/components/asr_lib/lib/v83x",
    ],
    extra_objects=[
        "./ext_modules/libmaix/components/maix_speech/Maix-Speech/components/asr_lib/lib/v83x/libms_asr_v83x.a",
    ],
    extra_compile_args=['-D__ARM__', '-D__ARMV7__', '-DCONF_KERNEL_IOMMU', '-DCONF_KERNEL_VERSION_4_9', '-std=c++11', '-std=gnu++11'],
    extra_link_args=[
        # set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wno-unused-variable -fPIC -c -s -ffunction-sections -fdata-sections -march=armv7-a  -mtune=cortex-a7" PARENT_SCOPE)
        # set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wno-sign-compare -Wno-unused-variable -fPIC -fexceptions -s -ffunction-sections -fdata-sections -fpermissive -march=armv7-a  -mtune=cortex-a7" PARENT_SCOPE)
    ],
)


_maix_camera_module = Pybind11Extension(
    name='_maix_camera',
    include_dirs=['ext_modules/_maix_camera/include',
                  'ext_modules/libmaix/components/libmaix/include'],
    sources=get_srcs('ext_modules/_maix_camera'),
    libraries=[
        # "dl",
        # "rt",
        # "log",
        # "ion",
        "pthread",
        # "cdc_base",
        # "MemAdapter",
        # "media_utils",
        # "mpp_vi",
        # "mpp_isp",
        # "ISP",
        # "venc_base",
        # "mpp_component",
        # "adecoder",
        # "asound",
        # "venc_base",
        # "hwdisplay",
        # "maix_utils",
        # "maix_nn",
        "maix_image",
        "maix_cam",
    ],
    library_dirs=[ ext_so, ],
    # extra_link_args  = [ "-Wl,-z,origin", "-Wl,-rpath='$ORIGIN/maix'" ]
    extra_compile_args=['-DV83xCamera', '-std=c++11', '-std=gnu++11'],
    extra_link_args=["-Wl,-rpath=/usr/lib/python3.8/site-packages/maix"]
)

_maix_display_module = Pybind11Extension(
    name="_maix_display",
    include_dirs=['ext_modules/_maix_display/include',
                  'ext_modules/libmaix/components/libmaix/include'],
    sources=get_srcs('ext_modules/_maix_display'),
    libraries=[
        # "dl",
        # "rt",
        # "log",
        # "ion",
        "pthread",
        # "cdc_base",
        # "maix_utils",
        # "maix_nn",
        "maix_image",
        "maix_disp",
    ],
    library_dirs=["/lib",  "/usr/lib", ext_so, ],
    # extra_compile_args=['-DV83xDisplay'],
    extra_link_args=["-Wl,-rpath=/usr/lib/python3.8/site-packages/maix"]
)


# max_nn_srcs = get_srcs('ext_modules/_maix_nn/src')
# max_nn_srcs.extend(get_srcs('ext_modules/libmaix/components/libmaix/src'))
# max_nn_srcs.remove("ext_modules/libmaix/components/libmaix/src/libmaix.c")
# _maix_nn_module = Extension('_maix_nn', include_dirs=['ext_modules/_maix_nn/include', 'ext_modules/libmaix/components/libmaix/include'],
#                             sources=max_nn_srcs,
#                             libraries=[
#     "maix_utils", "maix_nn",
# ],
#     library_dirs=[ ext_so, ],
#     # extra_link_args  = [ "-Wl,-z,origin", "-Wl,-rpath='$ORIGIN/maix'" ]
#     extra_link_args=["-Wl,-rpath=/usr/lib/python3.8/site-packages/maix"]
# )

# pybind11
# _maix_nn_new_module = Pybind11Extension(
#     name = '_maix_nn_new',
#     include_dirs = [ 'ext_modules/libmaix/components/libmaix/include'],
#     sources = get_srcs('ext_modules/_maix_nn_new') + get_srcs('ext_modules/libmaix/components/libmaix/src'),
#     library_dirs = [ext_so ,],
#     libraries = [
#         "maix_utils", "maix_nn",
#     ],
#     extra_compile_args=['-std=c++11', '-std=gnu++11'],
#     # extra_compile_args=['-std=c++11', '-std=gnu++11', '-DCONFIG_ARCH_V831'],
#     extra_link_args=["-Wl,-rpath=/usr/lib/python3.8/site-packages/maix"]
# )
max_nn_srcs = get_srcs('ext_modules/_maix_nn_mud/src')
max_nn_srcs.extend(get_srcs('ext_modules/libmaix/components/libmaix/src'))
max_nn_srcs.remove("ext_modules/libmaix/components/libmaix/src/libmaix.c")
_maix_nn_module = Extension('_maix_nn', include_dirs=['ext_modules/_maix_nn_mud/include', 'ext_modules/libmaix/components/libmaix/include'],
                            sources=max_nn_srcs,
                            libraries=[
    "maix_utils", "maix_nn", "maix_cam",
],
    library_dirs=[ ext_so, ],
    # extra_link_args  = [ "-Wl,-z,origin", "-Wl,-rpath='$ORIGIN/maix'" ]
    extra_link_args=["-Wl,-rpath=/usr/lib/python3.8/site-packages/maix"]
)


_maix_nn_decode_retinaface_module =Pybind11Extension(
    name = '_maix_nn_decoder_retinaface',
    include_dirs=['ext_modules/libmaix/components/libmaix/include'],
    sources=['ext_modules/_maix_nn_decoder/py_maix_nn_decoder_retinaface.cpp','ext_modules/libmaix/components/libmaix/src/decoder/decoder_retinaface.c'] ,
    libraries=[
        "pthread",
        "maix_nn",
    ],
    library_dirs=[ ext_so, ],
    # extra_compile_args=['-std=c++11', '-std=gnu++11', '-DCONFIG_ARCH_V831'],
    extra_link_args=["-Wl,-rpath=/usr/lib/python3.8/site-packages/maix"]
)


_maix_nn_decode_license_plate_location_module =Pybind11Extension(
    name = '_maix_nn_decoder_license_plate_location',
    include_dirs=['ext_modules/libmaix/components/libmaix/include'],
    sources=['ext_modules/_maix_nn_decoder/py_maix_nn_decoder_license_plate_location.cpp','ext_modules/libmaix/components/libmaix/src/decoder/decoder_license_plate_location.c'] ,
    libraries=[
        "pthread",
        "maix_nn",
    ],
    library_dirs=[ ext_so, ],
    # extra_compile_args=['-std=c++11', '-std=gnu++11', '-DCONFIG_ARCH_V831'],
    extra_link_args=["-Wl,-rpath=/usr/lib/python3.8/site-packages/maix"]
)


_maix_nn_decode_CTC_module =Pybind11Extension(
    name = '_maix_nn_decoder_CTC',
    include_dirs=['ext_modules/libmaix/components/libmaix/include'],
    sources=['ext_modules/_maix_nn_decoder/py_maix_nn_decoder_ctc.cpp','ext_modules/libmaix/components/libmaix/src/decoder/decoder_ctc.c'] ,
    libraries=[
        "pthread",
        "maix_nn",
    ],
    library_dirs=[ ext_so, ],
    # extra_compile_args=['-std=c++11', '-std=gnu++11', '-DCONFIG_ARCH_V831'],
    extra_link_args=["-Wl,-rpath=/usr/lib/python3.8/site-packages/maix"]
)

# _maix_nn_decode_yolo2_module =Pybind11Extension(
#     name = '_maix_nn_decoder_yolo',
#     include_dirs=['ext_modules/libmaix/components/libmaix/include'],
#     # sources=get_srcs("ext_modules/_maix_nn_decoder") + get_srcs('ext_modules/libmaix/components/libmaix/src/decoder') + get_srcs('ext_modules/libmaix/components/libmaix/src'),
#     sources=['ext_modules/_maix_nn_decoder/py_maix_nn_decoder_yolo.cpp' ,'ext_modules/libmaix/components/libmaix/src/decoder/decoder_yolo2.c'],

#     libraries=[
#         "pthread",
#         # "maix_utils",
#         "maix_nn",
#         # "maix_nn_decoder",
#     ],
#     library_dirs=[ ext_so, ],
#     # extra_compile_args=['-std=c++11', '-std=gnu++11', '-DCONFIG_ARCH_V831'],
#     extra_link_args=["-Wl,-rpath=/usr/lib/python3.8/site-packages/maix"]
# )





# python3.8 -m pip install pybind11
_maix_image_module = Pybind11Extension("_maix_image",
    include_dirs=[
        get_incs(
            'ext_modules/libmaix/components/libmaix/include'),
        get_incs(
            'ext_modules/libmaix/components/libmaix/lib/arch/v83x/include/opencv4/'),
        get_incs(
            'ext_modules/libmaix/components/maix_cv_image/include'),
        get_incs(
            'ext_modules/libmaix/components/third_party/imlib/include'),
        get_incs(
            'ext_modules/_maix_image/include')
    ],
    sources =
            get_srcs('ext_modules/_maix_image') +
            get_srcs('ext_modules/libmaix/components/maix_cv_image/src') +
            get_srcs('ext_modules/libmaix/components/third_party/imlib/src'),
    libraries=[
        "maix_utils", "maix_cam", "maix_image",
        "opencv_videoio", "opencv_highgui", "opencv_core", "opencv_imgproc", "opencv_imgcodecs", "opencv_freetype", "opencv_flann", "opencv_features2d", "opencv_calib3d"
    ],
    library_dirs=[
        ext_so,
        "./ext_modules/libmaix/components/libmaix/lib/arch/v83x/opencv4",
    ],
    extra_link_args=[
        "-Wl,-rpath=/usr/lib/python3.8/site-packages/maix",
        "-Wl,-rpath=/usr/lib/python3.8/site-packages"
    ],
    extra_compile_args=['-std=c++11', '-std=gnu++11', '-DCONFIG_ARCH_V831', '-DIMLIB_CONFIG_H_FILE="costom_imlib_config.h"' ],
)

_maix_modules = [
    libi2c_module,
    _maix_module,
    _maix_vivo_module,
    _maix_image_module,
    # _maix_camera_module,
    _maix_speech_module,
    _maix_display_module,
    _maix_nn_module,
    _maix_nn_decode_retinaface_module,
    _maix_nn_decode_license_plate_location_module,
    _maix_nn_decode_CTC_module,
]

_maix_data_files = [
    ('/maix', get_srcs(ext_so, ['so'], ['libopencv_'])),
    # ('/', get_srcs("ext_modules/libmaix/components/libmaix/lib/arch/v83x/opencv4", ['so'])),  # depend system provide
]

_maix_py_modules = [
    "numpy",
    "rpyc",
    "gpiod",
    "evdev",
    "spidev",
    "pyserial",
]
