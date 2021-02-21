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

your arm

- python3.8 setup.py build v831

## pypi upload

- python3 setup.py sdist build
- python3.8 setup.py bdist_wheel maix_v831

> pip install twine

- twine upload dist/* --verbose
