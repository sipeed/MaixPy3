<p align="center">
    <img height="120" src="https://wiki.sipeed.com/soft/maixpy/assets/maixpy/maixpy.png" title="" alt="MaixPy3 Logo">
</p>

# MaixPy3 ![GitHub forks](https://img.shields.io/github/forks/sipeed/maixpy3.svg?style=social) ![GitHub stars](https://img.shields.io/github/stars/sipeed/maixpy3.svg?style=social) ![GitHub watchers](https://img.shields.io/github/watchers/sipeed/maixpy3.svg?style=social)

[![MIT](https://img.shields.io/badge/license-MIT-blue.svg)](./LICENSE) [![PyPI version](https://badge.fury.io/py/maixpy3.svg)](https://badge.fury.io/py/maixpy3) ![Python](https://img.shields.io/badge/Python-3.5↗-ff69b4.svg) ![issue](https://img.shields.io/github/issues/sipeed/maixpy3.svg)

MaixPy3 is a Python3 toolkit based on [cpython](https://github.com/python/cpython), which simplifies the development of applications on Linux AI edge devices through Python programming.

### [See the wiki.sipeed.com for more information](https://wiki.sipeed.com/maixpy3)

## Support Jupyter Environment

Install [rpyc_ikernel](https://github.com/sipeed/rpyc_ikernel) kernel in jupyter notebook & lab to get an IDE that can remotely call Python code, videos, and image streaming.

## Develop

Under `linux x86_64`, use `python3 setup.py build` to complete the linux package construction.
If you need `.whl` pre-compiled package, please change `build` to `bdist_wheel`.
For other platforms, take the version of `maix_v83x` as an example, match the Python3 + cross-compilation chain of the corresponding platform, and run `python3.8 setup.py build maix_v83x` to complete the construction of the target platform package.

Welcome to provide configurations of different platforms to [MaixPy3/envs/](./envs/) to adapt to the MaixPy3 environment.

| **Platform**      | **Status** |  **Config** | **Progress** | **Toolchain** |
| --------------    | ---------- | ----------- | ------------ | ------------ |
| linux_pc & RPi_2B | working    | [linux_desktop](./envs/linux_desktop.py)  | 60%  | [develop.md](./docs/develop.md) & [maixpy3_build.yml](.github/workflows/maixpy3_build.yml)
| maix_v83x         | working    | [maix_v83x](./envs/maix_v83x.py)    | 90%  | [v83x_linux_x86_python3.8_toolchain](https://github.com/sipeed/MaixPy3/releases/tag/20210613)
| maix_r329         | working    | [maix_r329](./envs/maix_r329.py)    |  90%  | [r329_linux_x86_python3.9_toolchain](https://github.com/sipeed/MaixPy3/releases/tag/20211101)

> The development progress is in no particular order.

## License

Copyright (c) Sipeed Corporation. All rights reserved.

Licensed under the MIT license.
