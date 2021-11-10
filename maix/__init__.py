
__all__ = ['nn', 'display', 'camera']

try:
  from . import nn
  __all__.extend(['nn'])
except ModuleNotFoundError as e:
  pass

try:
  from _maix_opencv import Vision, Image
  vision, image = Vision(), Image()
  __all__.extend(['vision', 'image'])
except ModuleNotFoundError as e:
  pass
