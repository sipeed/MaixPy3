
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
_maix_opencv_module = Pybind11Extension("_maix_opencv",
    include_dirs=[
        get_incs(
            'ext_modules/libmaix/components/libmaix/include'),
        get_incs(
            'ext_modules/libmaix/components/libmaix/lib/arch/v83x/include/opencv4/'),
        get_incs(
            'ext_modules/libmaix/components/maix_cv_image/include')
    ],
    sources = get_srcs('ext_modules/_maix_opencv') + get_srcs('ext_modules/libmaix/components/maix_cv_image/src'),
    libraries=[
        "maix_utils", "maix_cam", "maix_image",
        "opencv_videoio", "opencv_highgui", "opencv_core", "opencv_imgproc", "opencv_imgcodecs", "opencv_freetype"
        # "opencv_aruco", "opencv_dnn", "opencv_hfs", "opencv_optflow", "opencv_shape",
        # "opencv_videoio","opencv_bgsegm", "opencv_dpm", "opencv_highgui", "opencv_phase_unwrapping", "opencv_stereo",
        # "opencv_video", "opencv_bioinspired", "opencv_face", "opencv_imgcodecs", "opencv_photo",
        # "opencv_stitching", "opencv_videostab", "opencv_calib3d", "opencv_features2d", "opencv_img_hash",
        # "opencv_plot", "opencv_structured_light", "opencv_xfeatures2d", "opencv_ccalib", "opencv_flann",
        # "opencv_imgproc", "opencv_quality", "opencv_superres", "opencv_ximgproc", "opencv_core", "opencv_freetype",
        # "opencv_line_descriptor", "opencv_reg", "opencv_surface_matching", "opencv_xobjdetect", "opencv_datasets",
        # "opencv_fuzzy", "opencv_ml", "opencv_rgbd", "opencv_text", "opencv_xphoto", "opencv_dnn_objdetect",
        # "opencv_gapi", "opencv_objdetect", "opencv_saliency", "opencv_tracking"
    ],
    library_dirs=[
        ext_so,
        "./ext_modules/libmaix/components/libmaix/lib/arch/v83x/opencv4",
    ],
    extra_link_args=[
        "-Wl,-rpath=/usr/lib/python3.8/site-packages/maix",
        "-Wl,-rpath=/usr/lib/python3.8/site-packages/maix/_maix_opencv"
    ],
)


# python3.8 -m pip install pybind11
_maix_speech_module = Pybind11Extension("_maix_speech",
    include_dirs=[
        get_incs(
            'ext_modules/_maix_speech/Maix-Speech/components/asr_lib/include'),
        get_incs(
            'ext_modules/_maix_speech/Maix-Speech/components/utils/include')
    ],
    sources = get_srcs('ext_modules/_maix_speech', exclude=["utils", "projects"]),
    libraries=[
        "ms_asr_v83x", "asound"
    ],
    library_dirs=[
        ext_so,
        "./ext_modules/_maix_speech/Maix-Speech/components/asr_lib/lib/v83x",
    ],
    extra_objects=[
        "./ext_modules/_maix_speech/Maix-Speech/components/asr_lib/lib/v83x/libms_asr_v83x.a",
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


max_nn_srcs = get_srcs('ext_modules/_maix_nn/src')
max_nn_srcs.extend(get_srcs('ext_modules/libmaix/components/libmaix/src'))
max_nn_srcs.remove("ext_modules/libmaix/components/libmaix/src/libmaix.c")
_maix_nn_module = Extension('_maix_nn', include_dirs=['ext_modules/_maix_nn/include', 'ext_modules/libmaix/components/libmaix/include'],
                            sources=max_nn_srcs,
                            libraries=[
    "maix_utils", "maix_nn",
],
    library_dirs=[ ext_so, ],
    # extra_link_args  = [ "-Wl,-z,origin", "-Wl,-rpath='$ORIGIN/maix'" ]
    extra_link_args=["-Wl,-rpath=/usr/lib/python3.8/site-packages/maix"]
)

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
            'ext_modules/_maix_image/include')
    ],
    sources = get_srcs('ext_modules/_maix_image') + get_srcs('ext_modules/libmaix/components/maix_cv_image/src'),
    libraries=[
        "maix_utils", "maix_cam", "maix_disp", "maix_image",
        "opencv_videoio", "opencv_highgui", "opencv_core", "opencv_imgproc", "opencv_imgcodecs", "opencv_freetype"
        # "opencv_aruco", "opencv_dnn", "opencv_hfs", "opencv_optflow", "opencv_shape",
        # "opencv_videoio","opencv_bgsegm", "opencv_dpm", "opencv_highgui", "opencv_phase_unwrapping", "opencv_stereo",
        # "opencv_video", "opencv_bioinspired", "opencv_face", "opencv_imgcodecs", "opencv_photo",
        # "opencv_stitching", "opencv_videostab", "opencv_calib3d", "opencv_features2d", "opencv_img_hash",
        # "opencv_plot", "opencv_structured_light", "opencv_xfeatures2d", "opencv_ccalib", "opencv_flann",
        # "opencv_imgproc", "opencv_quality", "opencv_superres", "opencv_ximgproc", "opencv_core", "opencv_freetype",
        # "opencv_line_descriptor", "opencv_reg", "opencv_surface_matching", "opencv_xobjdetect", "opencv_datasets",
        # "opencv_fuzzy", "opencv_ml", "opencv_rgbd", "opencv_text", "opencv_xphoto", "opencv_dnn_objdetect",
        # "opencv_gapi", "opencv_objdetect", "opencv_saliency", "opencv_tracking"
    ],
    library_dirs=[
        ext_so,
        "./ext_modules/libmaix/components/libmaix/lib/arch/v83x/opencv4",
    ],
    extra_link_args=[
        "-Wl,-rpath=/usr/lib/python3.8/site-packages/maix",
        "-Wl,-rpath=/usr/lib/python3.8/site-packages/maix/_maix_opencv"
    ],
    extra_compile_args=['-std=c++11', '-std=gnu++11' ],
)

_maix_modules = [
    libi2c_module,
    _maix_module,
    _maix_vivo_module,
    _maix_image_module,
    # _maix_opencv_module,
    # _maix_camera_module,
    _maix_speech_module,
    _maix_display_module,
    _maix_nn_module
]

_maix_data_files = [
    ('/maix', get_srcs(ext_so, ['so'], ['libopencv_'])),
    # ('/maix/_maix_opencv/', get_srcs("ext_modules/_maix_opencv/ports/v83x/lib", ['so'])),  # depend system provide
]

_maix_py_modules = [
    "Pillow",
    "rpyc",
    "gpiod",
    "evdev",
    "spidev",
    "pyserial",
    "zbarlight",
]
