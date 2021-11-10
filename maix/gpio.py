
try:
  import gpiod as LowGPIO
  class GPIO:
    def __init__(self) -> None:
      pass
except ModuleNotFoundError as e:
  pass
