
try:
    from _maix_nn import *
    from . import decoder
    from . import app

    from _maix_nn import F, load

    # from .import nn_new
except ModuleNotFoundError as e:
  pass
