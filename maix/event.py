
# https://python-evdev.readthedocs.io/

try:
  class LazyImport:
    def __init__(self, module_name):
      self.module_name = module_name
      self.module = None
    def __getattr__(self, name):
      if self.module is None:
        self.module = __import__(self.module_name)
      return getattr(self.module, name)
  # import asyncio
  asyncio = LazyImport("asyncio")  # evdev will normal load asyncio > 5M
  from evdev import InputDevice
except ModuleNotFoundError as e:
  pass

if __name__ == '__main__':
  # from maix import event
  # event.InputDevice

  def check_key(default="/dev/input/event0"):
    import os
    tmp = "/dev/input/by-path/"
    if os.path.exists(tmp):
      for i in os.listdir(tmp):
        if i.find("kbd") != -1:
          return tmp + i
    return default

  def detectInputKey(count):
    from select import select
    dev = InputDevice(check_key())
    while True:
      select([dev], [], [])
      for event in dev.read():
        if event.value == 1 and event.code != 0:
          count += 1
          print(count)
  from selectors import DefaultSelector, EVENT_READ
  selector = DefaultSelector()
  mouse = InputDevice('/dev/input/event7')
  keybd = InputDevice('/dev/input/event13')
  # This works because InputDevice has a `fileno()` method.
  selector.register(mouse, EVENT_READ)
  selector.register(keybd, EVENT_READ)
  try:
    while True:
      for key, mask in selector.select():
        device = key.fileobj
        for event in device.read():
          print(event)
  except Exception as e:
    mouse.close()
    keybd.close()

