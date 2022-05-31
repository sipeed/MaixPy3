from maix import event, display, image
from select import select
import os
os.system("insmod /lib/modules/4.9.118/cst816.ko")
dev = event.InputDevice("/dev/input/event1")
img = display.as_image()
while True:
  display.show(img)
  r, w, x = select([dev], [], [], 0) # if r == 0 or set 0 will read() raise BlockingIOError
  if r:
    x, y, e = 0, 0, 0
    for data in dev.read():
      if (data.type == 0x03):
        # print(' type: ', data.type, ' code: ', hex(data.code), ' value: ', hex(data.value))
        if data.code == 0x35: # ABS_MT_POSITION_X
          x = data.value
        if data.code == 0x36: # ABS_MT_POSITION_Y
          y = data.value
        if data.code == 0x30: # ABS_MT_TOUCH_MAJOR
          e = data.value
        if data.code == 0x32: # ABS_MT_WIDTH_MAJOR
          if data.value == 0:
            img.draw_rectangle(0, 0, 240, 240, color=(0, 0, 0), thickness=-1)
          else:
            img.draw_circle(x, y, 2, color=(255, 0, 0), thickness=-1)
          tmp = "(%d, %d, %d, %s)" % (data.value, x, y, hex(e))
          w, h = image.get_string_size(tmp)
          bak = image.new(size=(w + 20, h + 2))
          bak.draw_string(0, 0, tmp)
          img.draw_image(bak, 120 - w // 2, 120)

