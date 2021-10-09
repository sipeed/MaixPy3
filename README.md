<p align="center">
    <img height="120" src="https://maixpy.sipeed.com/static/image/logo.png" title="" alt="MaixPy3 Logo">
</p>

# MaixPy3 ![GitHub forks](https://img.shields.io/github/forks/sipeed/MaixPy3.svg?style=social) ![GitHub stars](https://img.shields.io/github/stars/sipeed/MaixPy3.svg?style=social) ![GitHub watchers](https://img.shields.io/github/watchers/sipeed/MaixPy3.svg?style=social)

English | [简体中文](./README.CN.md) 

[![MIT](https://img.shields.io/badge/license-MIT-blue.svg)](./LICENSE) [![PyPI version](https://badge.fury.io/py/MaixPy3.svg)](https://badge.fury.io/py/MaixPy3) ![Python](https://img.shields.io/badge/Python-3.5↗-ff69b4.svg) ![issue](https://img.shields.io/github/issues/sipeed/maixpy3.svg)

MaixPy3 is a Python3 toolkit based on [cpython](https://github.com/python/cpython), which simplifies the development of applications on Linux AI edge devices through Python programming.

[TOC](#maixpy3---)
-  [Usage](#Usage)
-  [Jupyter](#Jupyter)
-  [Progress](#Progress)
-  [Build](#Build)
-  [Develop](#Develop)
-  [Thanks](#Thanks)
-  [License](#License)

## Usage

Display the camera image on the screen.

```python
from maix import display, camera
display.show(camera.capture())
```

After inputting the image to the model, the result of the forward algorithm is returned.

```python
from PIL import Image
from maix import nn

m = nn.load({
    "param": "/root/models/resnet_awnn.param",
    "bin": "/root/models/resnet_awnn.bin"
  }, opt={
    "model_type":  "awnn",
    "inputs": {
        "input0": (224, 224, 3)
    },
    "outputs": {
        "output0": (1, 1, 1000)
    },
    "first_layer_conv_no_pad": False,
    "mean": [127.5, 127.5, 127.5],
    "norm": [0.00784313725490196, 0.00784313725490196, 0.00784313725490196],
})

img = Image.open("input.jpg")
out = m.forward(img, quantize=True)
print(out.shape)
out = nn.F.softmax(out)
print(out.max(), out.argmax())

```

Some examples of accessing hardware peripherals.

- GPIO

```python
import time
from maix import gpio
PH_BASE = 224 # "PH"
gpiochip1 = gpio.chip("gpiochip1")
led = gpiochip1.get_line((PH_BASE + 14)) # "PH14"
config = gpio.line_request()
config.request_type = gpio.line_request.DIRECTION_OUTPUT
led.request(config)

while led:
    led.set_value(0)
    time.sleep(0.1)
    led.set_value(1)
    time.sleep(0.1)
```

- PWM

```python
import time
from maix import pwm

with pwm.PWM(6) as pwm6:
  pwm6.period = 1000000
  pwm6.duty_cycle = 10000
  pwm6.enable = True
  while True:
    for i in range(50, 1, -10):
      pwm6.duty_cycle = i * 10000
      time.sleep(0.1)
    for i in range(1, 50, +10):
      pwm6.duty_cycle = i * 10000
      time.sleep(0.1)
```

- I2C

```python
from maix import i2c
i2c = i2c.I2CDevice('/dev/i2c-2', 0x26)
i2c.write(0x1, b'\xAA')
print(i2c.read(0x1, 1))
```

- SPI

```python
from maix import spi
spi = spi.SpiDev()
spi.open(1, 0) # '/dev/spidev1.0'

spi.bits_per_word = 8
spi.max_speed_hz = 1
spi.mode = 0b11

import time

while True:
  time.sleep(0.1)
  to_send = [0x01, 0x02, 0x01]
  print(spi.xfer2(to_send, 800000)) # 800Khz

```

- UART

```python
from maix import serial

with serial.Serial("/dev/ttyS1",115200) as ser:
  ser.write(b"Hello Wrold !!!\n")
  ser.setDTR(True)
  ser.setRTS(True)
  tmp = ser.readline()
  print(tmp)
  ser.write(tmp)
  ser.setDTR(False)
  ser.setRTS(False)
```

- EVENT

```python
from maix import evdev
from select import select
dev = evdev.InputDevice('/dev/input/event9')

while True:
  select([dev], [], [])
  for event in dev.read():
    print(event.code, event.value)
```

### [See the documentation for more information](https://maixpy.sipeed.com/)

## Jupyter

Install [rpyc_ikernel](https://github.com/sipeed/rpyc_ikernel) kernel in jupyter notebook & lab to get an IDE that can remotely call Python code, videos, and image streaming.

Click here to view the effect [usage_display_hook.ipynb](./examples/general/usage_display_hook.ipynb). Note that jupyter runs on your computer.

## Progress

| **Platform**         | **Status** |   **Example**    | **config** | **Memory usage** |
| --------------   | ---------- | ---------------- | --------- | --------- |
| linux x86_64     | working    | [general](./examples/general/) | [general](./envs/general.py) | unlimited |
| maix_v83x        | working    | [maix_v83x](./examples/maix_v83x/) | [maix_v83x](./envs/maix_v83x.py)     | 11M+
| maix_r329      | working    | [maix_r329](./examples/maix_r329/) |   [maix_r329](./envs/maix_r329.py)    |  unknown
| RPi_XXXX         | maybe      |                  |   NULL    |
| Windows          | build fail |                  |           |

> The development progress is in no particular order.

## Build

Under `linux x86_64`, use `python3 setup.py build` to complete the general package construction.

For other platforms, take the version of `maix_v83x` as an example, match the Python3 + cross-compilation chain of the corresponding platform([toolchain_v83x_linux_x86](https://github.com/sipeed/MaixPy3/releases/tag/20210613)), and run `python3.8 setup.py build maix_v83x` to complete the construction of the target platform package.

Welcome to provide configurations of different platforms to [MaixPy3/envs/](./envs/) to adapt to the MaixPy3 environment.

> If you need `.whl` pre-compiled package, please change `build` to `bdist_wheel`.

## Develop

> Tested glibc >= 27 on Ubuntu20.04 & manjaro20.03.

Each catalog function of the project.

- [docs](./docs/) [store some general development documents]
- [envs](./envs/) [store compilation configurations for different platforms]
- [examples](./examples/) [store examples or applications on different platforms]
- [ext_modules](./ext_modules/) [store project modules that need to be compiled]
- [maix](./maix/) [Provide maix entry Python module]
- [tests](./tests/) [Provide tox common test items]
- [setup.py](./setup.py) [MaixPy3 project compilation entry]

If you want to submit the configuration of other platforms, please refer to the following:

- [general.py](./envs/general.py)
- [maix_v83x.py](./envs/maix_v83x.py)

```python

_maix_modules = [
    libi2c_module,
]

_maix_data_files = [
    ('/maix', get_srcs(ext_so, ['so'， 'ko'])),
]

_maix_py_modules = [
    "numpy",
]

```

If you want to submit some Python and C modules that need to be compiled, it is recommended to use the sub-repository to import, refer to the following:

- [_maix](./ext_modules/_maix)
- [_maix_nn](./ext_modules/_maix_nn)
- [libi2c](./ext_modules/libi2c)

If you want to submit some useful Python tools or sample code, refer to the following:

- [general](./examples/general/)
- [maix_v83x](./examples/general/)
- [rtsp_pyqt](./examples/rtsp_pyqt/)

## Thanks

All this comes from the power of open source. Thanks to them, they are listed in no particular order.

- [cpython](https://github.com/python/cpython)
- [rpyc](https://github.com/tomerfiliba-org/rpyc)
- [py-spidev](https://github.com/doceme/py-spidev)
- [libi2c](https://github.com/amaork/libi2c)
- [python-evdev](https://github.com/gvalkov/python-evdev)
- [python3-gpiod](https://github.com/hhk7734/python3-gpiod)
- [pwmpy](https://github.com/scottellis/pwmpy)
- [pyserial](https://github.com/pyserial/pyserial)
- [Pillow](https://github.com/python-pillow/Pillow)
- [opencv](https://github.com/opencv/opencv)
- [ncnn](https://github.com/Tencent/ncnn)
- [numpy](https://github.com/numpy/numpy)
- [opencv-python](https://github.com/opencv/opencv-python)
- [ipython](https://github.com/junhuanchen/ipython)
- [rtsp-rtp-stream](https://github.com/gabrieljablonski/rtsp-rtp-stream)
- [ipykernel](https://github.com/ipython/ipykernel)
- [jupyter](https://github.com/jupyter/jupyter)
- [MaixPy](https://github.com/sipeed/MaixPy)
- [rpyc_ikernel](https://github.com/sipeed/rpyc_ikernel)

The open source repositories that may be cited in the future are also grateful to them.

- [pyaudio](http://people.csail.mit.edu/hubert/pyaudio)
- [pyusb](https://github.com/pyusb/pyusb)
- [pybluez](https://github.com/pybluez/pybluez)
- [pyadi-iio](https://github.com/analogdevicesinc/pyadi-iio)
- [python-iio](https://github.com/zathras777/python-iio)

You are welcome to recommend the open source projects you need. If you find any missing projects, please let me know immediately.

## License

Copyright (c) Sipeed Corporation. All rights reserved.

Licensed under the MIT license.
