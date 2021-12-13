import _maix_nn

model = {
    "bin": "/root/video/resnet.bin"
}

options = {
    "model_type":  "normal",
    "inputs": {
        "input0": (224, 224, 3)
    },
    "outputs": {
        "output0": (1, 1, 1000)
    },
    "first_layer_conv_no_pad": False,
    "mean": [127.5, 127.5, 127.5],
    "norm": [0.0176, 0.0176, 0.0176],
}
print("-- load model:", model)
m = _maix_nn.load(model, opt=options)
print("-- load ok")
