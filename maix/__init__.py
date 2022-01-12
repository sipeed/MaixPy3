
__all__ = ['nn', 'display', 'camera']

try:
  from . import nn
  __all__.extend(['nn'])
except ModuleNotFoundError as e:
  pass

try:
  import _maix_image as image
  __all__.extend(['image'])
except ModuleNotFoundError as e:
  pass

try:
  import _maix as utils
  __all__.extend(['utils'])
except ModuleNotFoundError as e:
  pass

try:
  import spidev as spi
  __all__.extend(['spi'])
except ModuleNotFoundError as e:
  pass

try:
  import serial
  __all__.extend(['serial'])
except ModuleNotFoundError as e:
  pass
