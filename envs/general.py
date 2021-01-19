
from setuptools import Extension
from .utils import get_srcs

_maix_module = Extension('_maix', include_dirs=['ext_modules/_maix/include'], 
                        sources=get_srcs('ext_modules/_maix'), 
                        libraries=[
                            "jpeg"
                        ],
                    )

_maix_camera_module = Extension('_maix_camera', include_dirs=['ext_modules/_maix_camera/include'], 
                        sources=get_srcs('ext_modules/_maix_camera'),
                    )

_maix_display_module = Extension('_maix_display', include_dirs=['ext_modules/_maix_display/include'], 
                        sources=get_srcs('ext_modules/_maix_display'), 
                    )

_maix_modules = [
  _maix_module,
  _maix_camera_module,
  _maix_display_module
]

_maix_data_files = [
  
]
