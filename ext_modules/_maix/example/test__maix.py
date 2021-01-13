# -*- coding: utf-8 -*-
import py
import pytest

def test_import():
    import _maix
    print(_maix.help())
    tmp = _maix.rgb2jpg(b"\xff"*4*6*3, 4, 6)
    print(len(tmp))
