
from gpiod import chip, line, line_request
gpiochip1 = chip("gpiochip1")

def pin(gpio="PD", pos=11):
  return 32 * (ord(gpio.lower()[1]) - ord('a')) + pos

def gpio(line_offset=(224 + 14), line_mode = line_request.DIRECTION_OUTPUT): # "default PH14 OUTPUT"
  try:
    tmp = None
    tmp = gpiochip1.get_line(line_offset)
    config = line_request() # led.active_state == line.ACTIVE_LOW
    config.request_type = line_mode # line.DIRECTION_INPUT
    tmp.request(config)
  except Exception as e:
      print(e)
  finally:
    return tmp

import time
led = gpio(pin("PH", 14))
while led:
    led.set_value(0)
    time.sleep(0.5)
    print("0", led.get_value())
    led.set_value(1)
    time.sleep(0.5)
    print("1", led.get_value())
