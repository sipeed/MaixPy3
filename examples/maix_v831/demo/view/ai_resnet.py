# This file is part of MaixUI
# Copyright (c) sipeed.com 2018, 2021 
#
# Licensed under the MIT license:
#   http://www.opensource.org/licenses/mit-license.php
#

class bind:

    def __init__(self, vi_ui_base) -> None:
        self.result = None
        self.vi_ui_base = vi_ui_base
        # temp code to no-block 
        from maix import nn
        self.npu = nn.load({
            "param": "./resc/resnet.param",
            "bin": "./resc/resnet.bin"
        }, opt={
            "model_type":  "awnn",
            "inputs": {
                "input0": (224, 224, 3)
            },
            "outputs": {
                "output0": (1, 1, 1000)
            },
            "first_layer_conv_no_pad": False,
            "mean": [127.5, 127.5, 127.5],
            "norm": [0.00784313725490196, 0.00784313725490196, 0.00784313725490196],
        })
        self.tmp = 0

    def __enter__(self):
        from maix import camera
        camera.config(size=(224, 224))
        self.vi_ui_base.on_get_vi += self.on_data
        self.vi_ui_base.on_set_ui += self.on_draw
        return self.vi_ui_base

    def __exit__(self, exc_type, exc_val, exc_tb):
        try:
            self.vi_ui_base.on_get_vi -= self.on_data
            self.vi_ui_base.on_set_ui -= self.on_draw
            del self.vi_ui_base
            del self.npu
            if exc_type != None:
                print(exc_type, exc_val, exc_tb)
        except Exception as e:
            print(e)
        
    def on_draw(self, draw):
        if self.result != None:
            from resc.classes_label import labels
            from .common import common_font
            draw.text((0, 0), "{:.2f}: {}: {}".format(
                self.result[0], self.result[1], labels[self.result[1]]), (255, 0, 0), common_font)

    def on_data(self, rgb24):
        self.tmp += 1
        if self.tmp % 3 == 0:
            from maix import nn
            from maix import camera
            camera.config(size=(224, 224))
            out = self.npu.forward(rgb24, quantize=True)
            out = nn.F.softmax(out)
            if out.max() > 0.1:
                self.result = (out.max(), out.argmax())
                # print(self.result)
