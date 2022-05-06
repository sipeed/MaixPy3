# -*- coding: utf-8 -*-
import py
import pytest

def test_import():
    try:
      import _maix_nn
      print(_maix_nn.__version__)
      _maix_nn.help()
    except ModuleNotFoundError as e:
      pass
