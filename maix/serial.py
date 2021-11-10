
try:
  import serial as LowSerial
  class Serial:
    def __init__(self) -> None:
      pass
except ModuleNotFoundError as e:
  pass
