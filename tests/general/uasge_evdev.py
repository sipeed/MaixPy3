from maix import event, display, image
from select import select

dev = event.InputDevice("/dev/input/event1")
while True:
  r, w, x = select([dev], [], [], 0) # if r == 0 or set 0 will read() raise BlockingIOError
  if r:
    img = display.as_image()
    img.draw_rectangle(0, 0, 240, 240, color=(255, 0, 0), thickness=-1)
    x, y = 0, 0
    for data in dev.read():
      if (data.type == 0x03):
        print(' type: ', data.type, ' code: ', hex(data.code), ' value: ', hex(data.value))
        if data.code == 0x35: # ABS_MT_POSITION_X
          x = data.value
        if data.code == 0x36: # ABS_MT_POSITION_Y
          y = data.value
        if data.code == 0x30: # ABS_MT_TOUCH_MAJOR
          tmp = "(%d, %d, %d)" % (x, y, hex(data.value))
          w, h = image.get_string_size(tmp)
          img.draw_string(120 - w // 2, 120, tmp)
          display.show(img)
