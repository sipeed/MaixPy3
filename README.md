# MaixPy3

## install to your machine

- ifconfig (get your ip address)

- pip3 install maixpy3

- python3 -c "from maix import rpycs; rpycs.start()" &

## jupyter to your computer

- windows user can install [Anaconda3](https://www.anaconda.com/download) bring jupyter notebook & python3 & pip3 & more!.

- `pip3 install rpyc_ikernel

- check python >= 3.8.5

```shell
(base) PS C:\Users\dls> python -V
Python 3.8.5
```

- `python -m rpyc_ikernel.install`

```shell
(base) PS C:\Users\dls> python -m rpyc_ikernel.install
Installing IPython kernel spec of RPyc
C:\Users\dls\anaconda3\lib\site-packages\rpyc_ikernel\install.py:30: DeprecationWarning: replace is ignored. Installing a kernelspec always replaces an existing installation
  k.install_kernel_spec(td, 'RPyc', user=user,
...into C:\Users\dls\AppData\Roaming\jupyter\kernels\rpyc
(base) PS C:\Users\dls>
```

- jupyter notebook (or `jupyter lab`)

> more see https://github.com/sipeed/rpyc_ikernel

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
    tmp = (camera.capture())
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

## help

- [MaixPy3 CPython 开发文档](./docs/develop_zh.md)

- [MaixPy3 CPython develop_en.md](./docs/develop_en.md)

## pass

- glibc >= 27 on Ubuntu20.04 & manjaro20.03

## usage

- python3 setup.py build

- python3.8 setup.py build v831
