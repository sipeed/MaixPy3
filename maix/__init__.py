
__all__ = ['nn', 'display', 'camera']

# try:
#   import gpiod as gpio
#   __all__.append('gpio')
# except ModuleNotFoundError as e:
#   pass

# try:
#   import pylibi2c as i2c
#   __all__.append('i2c')
# except ModuleNotFoundError as e:
#   pass

# try:
#   import evdev as evdev
#   __all__.append('evdev')
# except ModuleNotFoundError as e:
#   pass

# try:
#   import spidev as spi
#   __all__.append('spi')
# except ModuleNotFoundError as e:
#   pass

# try:
#   import serial as serial
#   __all__.append('serial')
# except ModuleNotFoundError as e:
#   pass

# try:
#   # from .video import camera
#   from . import display, rpycs
#   __all__.extend(['display', 'video', 'rpycs', 'camera'])
# except ModuleNotFoundError as e:
#   pass

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

# try:
#   import shutil
#   from PIL import ImageShow
#   # use fbviewer on linux
#   # os.system('ln -s /usr/sbin/fbviewer /usr/sbin/display')
#   if shutil.which("fbviewer"):
#     class fbViewer(ImageShow.UnixViewer):
#       def get_command_ex(self, file, **options):
#         command = executable = "fbviewer"
#         return command, executable
#     ImageShow.register(fbViewer, 0)
# except ModuleNotFoundError as e:
#   pass
