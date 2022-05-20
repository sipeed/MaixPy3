
version='0.4.8'

__all__ = ['nn', 'display', 'camera', 'image']

try:
  from . import nn
  __all__.extend(['nn'])
except ModuleNotFoundError as e:
  pass

import signal
signal.signal(signal.SIGINT,lambda signum,frame:exit(0))
