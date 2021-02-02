import spidev
spi = spidev.SpiDev()
spi.open(1, 0)

spi.bits_per_word = 8
spi.max_speed_hz = 1
spi.mode = 0b11

import time

while True:
  time.sleep(0.1)
  to_send = [0x01, 0x02, 0x01]
  print(spi.xfer2(to_send, 800000)) # 800Khz

