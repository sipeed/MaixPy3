# MaixPy3

a cross-platform MaixPy for Python3, let's play with edge AI easier! 

## example

```python
from maix import display, camera
while True:
  im = camera.capture()
  if im:
      display.show(im)
```

```python
from maix import display, camera
from PIL import ImageDraw
while True:
  tmp = camera.capture()
  if tmp:
    draw = ImageDraw.Draw(tmp)
    draw.text((10, 10), str(time.asctime()), (255, 255, 255))
    display.show(tmp)
```

```python
import numpy as np
array = np.array([[1,2,3],[4,5,6]])
print(array)
print('number of dim:', array.ndim)
print('shape:',array.shape)
print('size:',array.size)
```

```python
from maix import i2c
i2c = i2c.I2CDevice('/dev/i2c-2', 0x26)

i2c.delay = 10
# i2c.page_bytes = 16
data = i2c.ioctl_read(0x1, 1)
print(data)
print(i2c.read(0x1, 1))
```

## Remote Procedure Call Python Code

### install maixpy3 to your machine

- use `ifconfig` at linux. (get your ip address)

- `pip3 install maixpy3`

- `python3 -c "from maix import rpycs; rpycs.start()" &`

### install jupyter to your computer

- [rpyc_ikernel](https://github.com/sipeed/rpyc_ikernel)

## help

- [MaixPy3 CPython 开发文档](https://github.com/sipeed/MaixPy3/tree/main/docs/develop_zh.md)

- [MaixPy3 CPython develop_en.md](https://github.com/sipeed/MaixPy3/tree/main/docs/develop_en.md)

## pass

- glibc >= 27 on Ubuntu20.04 & manjaro20.03

## usage

- python3 setup.py build (for your linux x86)

- python3.8 setup.py build v831 (for your linux arm)

- more linux
