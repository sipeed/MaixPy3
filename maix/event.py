
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
  asyncio = LazyImport("asyncio") # evdev will normal load asyncio > 5M

  import evdev as LowEvent
  class Event:
    def __init__(self) -> None:
      pass
except ModuleNotFoundError as e:
  pass

# class LazyImport:
#   def __init__(self, module_name):
#       self.module_name = module_name
#       self.module = None

#   def __getattr__(self, name):
#       if self.module is None:
#           self.module = __import__(self.module_name)
#       return getattr(self.module, name)

# asyncio = LazyImport("asyncio") # evdev will load asyncio

# from evdev import InputDevice
# from selectors import DefaultSelector, EVENT_READ

# selector = DefaultSelector()

# mouse = InputDevice('/dev/input/event0')
# # keybd = InputDevice('/dev/input/event1')

# # This works because InputDevice has a `fileno()` method.
# selector.register(mouse, EVENT_READ)
# # selector.register(keybd, EVENT_READ)

# while True:
#     for key, mask in selector.select():
#         device = key.fileobj
#         for event in device.read():
#             print(event)