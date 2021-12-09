<p align="center">
    <img height="120" src="https://wiki.sipeed.com/soft/maixpy/assets/maixpy/maixpy.png" title="" alt="MaixPy3 Logo">
</p>

# MaixPy3 ![GitHub forks](https://img.shields.io/github/forks/sipeed/maixpy3.svg?style=social) ![GitHub stars](https://img.shields.io/github/stars/sipeed/maixpy3.svg?style=social) ![GitHub watchers](https://img.shields.io/github/watchers/sipeed/maixpy3.svg?style=social)

[![MIT](https://img.shields.io/badge/license-MIT-blue.svg)](./LICENSE) [![PyPI version](https://badge.fury.io/py/maixpy3.svg)](https://badge.fury.io/py/maixpy3) ![Python](https://img.shields.io/badge/Python-3.5↗-ff69b4.svg) ![issue](https://img.shields.io/github/issues/sipeed/maixpy3.svg)

MaixPy3 is a Python3 toolkit based on [cpython](https://github.com/python/cpython), which simplifies the development of applications on Linux AI edge devices through Python programming.

### [See the wiki.sipeed.com for more information](https://wiki.sipeed.com/maixpy3)

## Support Jupyter

Install [rpyc_ikernel](https://github.com/sipeed/rpyc_ikernel) kernel in jupyter notebook & lab to get an IDE that can remotely call Python code, videos, and image streaming.

Click here to view the effect [usage_display_hook.ipynb](./examples/general/usage_display_hook.ipynb). Note that jupyter runs on your computer.

## Progress

| **Platform**         | **Status** |   **Example**    | **config** | **Memory usage** |
| --------------   | ---------- | ---------------- | --------- | --------- |
| linux x86_64     | working    | [general](./examples/general/) | [general](./envs/general.py) | unlimited |
| maix_v83x        | working    | [maix_v83x](./examples/maix_v83x/) | [maix_v83x](./envs/maix_v83x.py)     | 11M+
| maix_r329      | working    | [maix_r329](./examples/maix_r329/) |   [maix_r329](./envs/maix_r329.py)    |  unknown
| RPi_XXXX         | plan      |                  |   NULL    |
| RV11XX         | plan      |                  |   NULL    |

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

- [maix_v83x.py](./envs/maix_v83x.py)
- [maix_r329.py](./envs/maix_r329.py)

## License

Copyright (c) Sipeed Corporation. All rights reserved.

Licensed under the MIT license.
