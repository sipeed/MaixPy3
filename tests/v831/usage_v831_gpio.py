
from gpiod import chip, line, line_request
gpiochip1 = chip("gpiochip1")

# for V831
PF_BASE = 160 # "PF"
PG_BASE = 192 # "PG"
PH_BASE = 224 # "PH"

def gpio(line_offset=(PH_BASE + 14)):
  try:
    tmp = None
    tmp = gpiochip1.get_line(line_offset)
    config = line_request() # led.active_state == line.ACTIVE_LOW
    config.request_type = line_request.DIRECTION_OUTPUT # line.DIRECTION_INPUT
    tmp.request(config)
  except Exception as e:
      print(e)
  finally:
    return tmp

import time
led = gpio(PH_BASE + 14) # "PH14"
while led:
    led.set_value(0)
    time.sleep(0.1)
    led.set_value(1)
    time.sleep(0.1)
