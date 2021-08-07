<p align="center">
    <img height="120" src="https://maixpy.sipeed.com/static/image/logo.png" title="" alt="MaixPy3 Logo">
</p>

# MaixPy3 ![GitHub forks](https://img.shields.io/github/forks/sipeed/MaixPy3.svg?style=social) ![GitHub stars](https://img.shields.io/github/stars/sipeed/MaixPy3.svg?style=social) ![GitHub watchers](https://img.shields.io/github/watchers/sipeed/MaixPy3.svg?style=social)

[English](./README.md)  | 简体中文

[![MIT](https://img.shields.io/badge/license-MIT-blue.svg)](./LICENSE) [![PyPI version](https://badge.fury.io/py/MaixPy3.svg)](https://badge.fury.io/py/MaixPy3) ![Python](https://img.shields.io/badge/Python-3.5↗-ff69b4.svg) ![issue](https://img.shields.io/github/issues/sipeed/maixpy3.svg)

MaixPy3 是基于 [cpython](https://github.com/python/cpython) 的 Python3 工具包，通过 Python 编程简化在 Linux AI 边缘设备上开发应用。

[TOC](#maixpy3---)
-  [Usage](#Usage)
-  [Jupyter](#Jupyter)
-  [Progress](#Progress)
-  [Build](#Build)
-  [Develop](#Develop)
-  [Thanks](#Thanks)
-  [License](#License)

## Usage

在屏幕上显示摄像机的图像。

```python
from maix import display, camera
display.show(camera.capture())
```

将图像输入模型后，将返回正向算法的结果。

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

一些访问硬件外设的示例

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

### [更多信息请参见文档](https://maixpy.sipeed.com/)

## Jupyter

在 jupyter notebook & lab 安装 [rpyc_ikernel]( https://github.com/sipeed/rpyc_ikernel) 内核即可得到一个可以远程调用 Python 代码和视频、图像推流的集成开发环境。

运行效果点此查看 [usage_display_hook.ipynb](./examples/general/usage_display_hook.ipynb) ，注意 jupyter 安装且运行在你的计算机。

## Progress

| **Platform**         | **Status** |   **Example**    | **config** | **Memory usage** |
| --------------   | ---------- | ---------------- | --------- | --------- |
| linux x86_64     | working    | [general](./examples/general/) | [general](./envs/general.py) | unlimited |
| maix_v831        | working    | [maix_v831](./examples/maix_v831/) | [maix_v831](./envs/maix_v831.py)     | 11M ~ 17M (Max 48M)
| maix_rv1126      | prepare    |                  |   NULL    |
| RPi_XXXX         | maybe      |                  |   NULL    |
| Windows          | build fail |                  |           |

> 开发进度不分先后顺序。

## Build

在 `linux x86_64` 下使用 `python3 setup.py build` 完成通用包的构建。

若是其他平台，以 `maix_v831` 的版型为例，搭配对应平台的 Python3 + 交叉编译链([toolchain_v83x_linux_x86](https://github.com/sipeed/MaixPy3/releases/tag/20210613))，运行 `python3.8 setup.py build maix_v831` 从而完成目标平台包的构建。

欢迎提供不同平台的配置到 [MaixPy3/envs/](./envs/) 来适配 MaixPy3 环境。

> 如需要 `.whl` 预编译包请将 `build` 改成 `bdist_wheel` 即可。

## Develop

> Tested glibc >= 27 on Ubuntu20.04 & manjaro20.03.

项目的各目录功能。

- [docs](./docs/) [ 存放一些通用的开发文档 ]
- [envs](./envs/) [ 存放不同平台的编译配置 ]
- [examples](./examples/) [ 存放不同平台的示例或应用 ]
- [ext_modules](./ext_modules/) [ 存放需要编译的项目模块 ]
- [maix](./maix/) [ 提供 maix 入口 Python 模块 ]
- [tests](./tests/) [ 提供 tox 的通用测试项目 ]
- [setup.py](./setup.py) [ MaixPy3 项目编译入口 ]

如果你想要提交其他平台的配置，参考如下：

- [general.py](./envs/general.py)
- [maix_v831.py](./envs/maix_v831.py)

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

如果你想要提交一些需要编译的 Python 编译 C 的模块，推荐使用子仓库导入，参考如下：

- [_maix](./ext_modules/_maix)
- [_maix_nn](./ext_modules/_maix_nn)
- [libi2c](./ext_modules/libi2c)

如果你想要提交一些好用的 Python 工具或示例代码，参考如下：

- [general](./examples/general/)
- [maix_v831](./examples/general/)
- [rtsp_pyqt](./examples/rtsp_pyqt/)

## Thanks

这一切都来自于开源的力量，感谢它们，排名不分先后。

- [cpython](https://github.com/python/cpython)
- [rpyc](https://github.com/tomerfiliba-org/rpyc)
- [py-spidev](https://github.com/doceme/py-spidev)
- [libi2c](https://github.com/amaork/libi2c)
- [python-evdev](https://github.com/gvalkov/python-evdev)
- [python3-gpiod](https://github.com/hhk7734/python3-gpiod)
- [pwmpy](https://github.com/scottellis/pwmpy)
- [pyserial](https://github.com/pyserial/pyserial)
- [Pillow](https://github.com/python-pillow/Pillow)
- [numpy](https://github.com/numpy/numpy)
- [opencv](https://github.com/opencv/opencv)
- [ncnn](https://github.com/Tencent/ncnn)
- [opencv-python](https://github.com/opencv/opencv-python)
- [ipython](https://github.com/junhuanchen/ipython)
- [rtsp-rtp-stream](https://github.com/gabrieljablonski/rtsp-rtp-stream)
- [ipykernel](https://github.com/ipython/ipykernel)
- [jupyter](https://github.com/jupyter/jupyter)
- [MaixPy](https://github.com/sipeed/MaixPy)
- [rpyc_ikernel](https://github.com/sipeed/rpyc_ikernel)

未来可能会引用的开源仓库，也同样感谢它们。

- [pyaudio](http://people.csail.mit.edu/hubert/pyaudio)
- [pyusb](https://github.com/pyusb/pyusb)
- [pybluez](https://github.com/pybluez/pybluez)
- [pyadi-iio](https://github.com/analogdevicesinc/pyadi-iio)
- [python-iio](https://github.com/zathras777/python-iio)

欢迎你推荐所需要的开源项目，如有发现遗漏的项目，请即刻告知。

## License

Copyright (c) Sipeed Corporation. All rights reserved.

Licensed under the MIT license.
