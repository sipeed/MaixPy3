# usage

- python setup.py build

- pip install .

- pip uninstall python3-maix -y

> *sudo python setup.py install*

## result

```shell
[dls@dls-pc python3-maix-0.1.1]$ python3 help.py
<module '_maix' from '/home/dls/.local/lib/python3.8/site-packages/_maix.cpython-38-x86_64-linux-gnu.so'>
<class 'I2CDevice'>
<module 'maix' from '/home/dls/desktop/sdk/package/lang/python/python3-maix/python3-maix-0.1.1/maix/__init__.py'>
<module 'maix.display' from '/home/dls/desktop/sdk/package/lang/python/python3-maix/python3-maix-0.1.1/maix/display.py'>
```

```python
[dls@dls-pc ~]$ python
Python 3.8.6 (default, Sep 30 2020, 04:00:38) 
[GCC 10.2.0] on linux
Type "help", "copyright", "credits" or "license" for more information.
>>> import maix
>>> maix
<module 'maix' from '/home/dls/.local/lib/python3.8/site-packages/maix/__init__.py'>
>>> maix.
maix.Video    maix.camera   maix.display  
>>> 
>>> import _maix
>>> _maix.I2CDevice(    
KeyboardInterrupt
>>> 
[7]+  已停止               python

```
