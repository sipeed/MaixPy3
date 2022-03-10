# https://pypi.org/project/gpiod/

try:
  from gpiod import chip, line, line_request
  config = None # rpi is default value A 0
  def gpio(gpio_line=0, gpio_bank="A", gpio_chip=0, line_mode = line_request.DIRECTION_OUTPUT):
    global config
    if config != None and gpio_line in config:
        gpio_bank, gpio_chip = config[gpio_line]
    l, c = [32 * (ord(gpio_bank.lower()[0]) - ord('a')) + gpio_line, chip("gpiochip%d" % gpio_chip)]
    tmp = c.get_line(l)
    cfg = line_request() # led.active_state == line.ACTIVE_LOW
    cfg.request_type = line_mode # line.DIRECTION_INPUT
    tmp.request(cfg)
    tmp.source = "GPIO chip %s bank %s line %d" % (gpio_chip, gpio_bank, gpio_line)
    return tmp
  def load(cfg=None):
    global config
    config = cfg
except ModuleNotFoundError as e:
  pass

m2dock = {
  6 : ("H", 1),
  7 : ("H", 1),
  8 : ("H", 1),
  13 : ("H", 1),
  14 : ("H", 1),
}

rv11xx = {
  0 : ("A", 4),
  1 : ("A", 4),
}

if __name__ == '__main__':
    import time
    # from maix import gpio
    # gpio.load(m2dock)
    load(m2dock)
    led = gpio(6)
    print(led.source)
    for i in range(3):
      led.set_value(0)
      print(led.get_value())
      time.sleep(0.5)
      led.set_value(1)
      print(led.get_value())
      time.sleep(0.5)
    led.release()
    load(None)
    led = gpio(6, "H", 1)
    print(led.source)
    for i in range(3):
      led.set_value(0)
      print(led.get_value())
      time.sleep(0.5)
      led.set_value(1)
      print(led.get_value())
      time.sleep(0.5)
    led.release()

