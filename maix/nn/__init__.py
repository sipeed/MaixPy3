
try:
    from _maix_nn import *
    from . import decoder
    from . import app

    from _maix_nn import F, load
except ModuleNotFoundError as e:
  pass
