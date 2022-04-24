
class v83x_ADC():
  def __init__(self, addr=b"0x05070080") -> None:
      self.addr = addr
      self.path = "/sys/class/sunxi_dump/dump"
      self.file = open(self.path, "wb+")
      self.last = self.value()
  def __del__(self):
      try:
        if self.file:
          self.file.close()
          del self.file
      except Exception as e:
        pass
  def value(self):
    self.file.write(b"0x05070080")
    self.file.seek(0)
    return int(self.file.read()[:-1], 16)

if __name__ == '__main__':
  import time
  from maix import display, image
  v831_adc0 = v83x_ADC()
  while True:
    time.sleep(0.1)
    tmp = image.Image().new((240, 240), (0x2c, 0x3e, 0x50), "RGB")
    val = v831_adc0.value()
    print(val)
    img = image.Image().open('/home/res/logo.png')
    tmp.draw_image(img, 50, 40, alpha=1).draw_string(20, 200, "adc0 : " + str(val), 1, (0xbd, 0xc3, 0xc7))
    display.show(tmp)

