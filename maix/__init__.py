
version='0.5.2'

__all__ = ['display', 'camera', 'image']

try:
  from . import nn
  __all__.extend(['nn'])
except ModuleNotFoundError as e:
  pass

import signal # v831 need release mpp, maybe have bug so remove.
signal.signal(signal.SIGINT,lambda signum,frame:exit(0))
