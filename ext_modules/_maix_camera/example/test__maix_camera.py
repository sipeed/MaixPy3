# -*- coding: utf-8 -*-
import py
import pytest

def test_import():
    import _maix_camera
    print(_maix_camera.VirtualCamera())

'''

import _maix_camera
cam = _maix_camera.Camera(240,240,0)
img_bytes = cam.read()
img_bytes =img_bytes[1]

'''
