
try:
  import spidev as LowSPI
  class SPI:
    def __init__(self) -> None:
      pass
except ModuleNotFoundError as e:
  pass
