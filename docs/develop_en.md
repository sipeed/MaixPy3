# develop readme

## Py module usage

see setup.py

- python setup.py build

- pip install .

- pip uninstall MaixPy3 -y

> *sudo python setup.py install*

- python3.8 setup.py bdist_wheel

- python3.8 setup.py bdist_egg

- python3.8 setup.py bdist

- pip install MaixPy3*whl -y --upgrade

add xxxx/__init__.py

## Py Call C module

such as libjpeg

- sudo apt-get install libjpeg-dev

see ext_modules/_maix

## C module usage

see Makefile

- cd ext_modules/libi2c

- make all

- make clean

add ext_modules/xxxx
header file add MANIFEST.in.

## tox test usage

see tox.ini

- tox

add tests/test_xxxx.py

auto run test_xxxx.py

## py call usage

- pip install --upgrade xxxx

- python -c "import maix"

## clean cmd

- find . -name '__pycache__' -type d -exec rm -rf {} \;

- python3 setup.py clean --all

## build cmd

your x86

- python setup.py build

your v83x

- source /opt/v83x_linux_x86_python3.8_toolchain/envsetup.sh

- python3.8 setup.py build maix_v83x

- adb shell "pip install /root/maixpy3-0.3.5-cp38-cp38-linux_armv7l.whl --upgrade"

- python3.8 setup.py clean --all bdist_wheel maix_v83x && adb push ./dist/maixpy3-0.3.5-cp38-cp38-linux_armv7l.whl /root/ && adb shell "pip install /root/maixpy3-0.3.5-cp38-cp38-linux_armv7l.whl --upgrade"

your r329

- source /opt/r329_linux_x86_python3.9_toolchain/envsetup.sh

- python3.9 setup.py clean --all bdist_wheel maix_r329 && scp ./dist/maixpy3-0.3.5-cp39*.whl root@192.168.0.5:~/

- pip uninstall maixpy3 -y && pip install ~/maixpy3-0.3.5-cp39*.whl

> pypi upload manylinux2014_aarch64

## pypi upload

- python3 setup.py sdist build
- python3.8 setup.py bdist_wheel maix_v83x

> pip3 install twine

- twine upload dist/* --verbose
