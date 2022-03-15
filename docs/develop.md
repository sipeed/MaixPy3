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

such as libopencv-*-dev

- sudo apt-get install libopencv-*-dev

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

- python3 setup.py bdist_wheel

your v83x

- source /opt/v83x_linux_x86_python3.8_toolchain/envsetup.sh

- python3.8 setup.py bdist_wheel maix_v83x

- adb shell "pip install /root/maixpy3-0.3.5-cp38-cp38-linux_armv7l.whl --upgrade"

- python3.8 setup.py clean --all bdist_wheel maix_v83x && adb push ./dist/maixpy3-0.4.0-cp38-cp38-linux_armv7l.whl /root/ && adb shell "pip install /root/maixpy3-0.4.0-cp38-cp38-linux_armv7l.whl --upgrade"

-  python3.8 setup.py clean --all bdist_wheel maix_v83x && adb push ./dist/maixpy3-0.4.0-cp38-cp38-linux_armv7l.whl /root/ &&  adb shell "pip install /root/maixpy3-0.4.0-cp38-cp38-linux_armv7l.whl --upgrade"

your r329

- source /opt/r329_linux_x86_python3.9_toolchain/envsetup.sh

- python3.9 setup.py bdist_wheel maix_r329

- python3.9 setup.py clean --all bdist_wheel maix_r329 && scp ./dist/maixpy3-0.4.3-cp39*.whl root@192.168.0.153:~/

- pip uninstall maixpy3 -y && pip install ~/maixpy3-0.4.*-cp39*.whl

> pypi upload manylinux2014_aarch64

## pypi upload

- python3 setup.py sdist build
- python3.8 setup.py bdist_wheel maix_v83x

> pip3 install twine

- twine upload dist/* --verbose

[    7.560531] armchina 3050000.aipu: AIPU KMD probe start...
[    7.560552] armchina 3050000.aipu: KMD version: release 3.0.13
[    7.560562] armchina 3050000.aipu: no interrupts-reg specified
[    7.564726] armchina 3050000.aipu: AIPU is not behind an IOMMU
[    7.564751] armchina 3050000.aipu: AIPU DRAM mem type is [CMA default]
[    7.588795] armchina 3050000.aipu: init DRAM region done: pa [0x47a00000, 0x49ffffff]
[    7.588827] armchina 3050000.aipu: AIPU core #0 detected: zhouyi-v1-0600
[    7.614772] armchina 3050000.aipu: AIPU Initial Status: 0x70000
[    7.614795] armchina 3050000.aipu: ########## AIPU CORE 0: ZHOUYI V1 ##########
[    7.614803] armchina 3050000.aipu: # ISA Version Register: 0x0
[    7.614810] armchina 3050000.aipu: # TPC Feature Register: 0x10101
[    7.614817] armchina 3050000.aipu: # SPU Feature Register: 0x0
[    7.614824] armchina 3050000.aipu: # HWA Feature Register: 0x10111
[    7.614831] armchina 3050000.aipu: # Revision ID Register: 0x0
[    7.614837] armchina 3050000.aipu: # Memory Hierarchy Feature Register: 0x170
[    7.614845] armchina 3050000.aipu: # Instruction RAM Feature Register:  0x0
[    7.614851] armchina 3050000.aipu: # TEC Local SRAM Feature Register:   0x312
[    7.614858] armchina 3050000.aipu: # Global SRAM Feature Register:      0x10
[    7.614865] armchina 3050000.aipu: # Instruction Cache Feature Register:0x233
[    7.614872] armchina 3050000.aipu: # Data Cache Feature Register:       0x70030
[    7.614880] armchina 3050000.aipu: # L2 Cache Feature Register:         0x0
[    7.614887] armchina 3050000.aipu: ############################################
[    7.614895] armchina 3050000.aipu: initialize AIPU core #0 done
