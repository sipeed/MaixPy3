
try:
  import pylibi2c as LowI2C
  class I2C:
    def __init__(self) -> None:
      pass
except ModuleNotFoundError as e:
  pass
