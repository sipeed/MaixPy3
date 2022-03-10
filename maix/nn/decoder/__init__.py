


try:
    from _maix_nn_decoder_retinaface import *
except ModuleNotFoundError as e:
  pass

try:
    from _maix_nn import _decoder
    Yolo2 = _decoder.Yolo2
except ModuleNotFoundError as e:
  pass
