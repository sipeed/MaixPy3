#!/usr/bin/env python

from PIL import Image, ImageFont, ImageDraw
from maix import display, camera
from maix import nn
from resc.classes_label import labels
from evdev import InputDevice
import asyncio
import time
import os
import socket

def get_host_ip():
    try:
        s = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
        s.connect(('8.8.8.8', 80))
        ip = "IP: " + s.getsockname()[0]
    except Exception as e:
        ip = "IP does not exist"
    finally:
        s.close()
    return ip

font = ImageFont.truetype("./resc/baars.ttf", 27, encoding="unic")

canvas = Image.new("RGBA", (240, 240), "#2c3e50")
with Image.open('./resc/logo.png') as logo:
    canvas.paste(logo, (50, 40, 50 + logo.size[0], 40 + logo.size[1]), logo)

draw = ImageDraw.Draw(canvas)
draw.text((10, 195), u'MaixPy.Sipeed.COM', "#bdc3c7", font)
draw.text((0, 0), u'<exit', "#7f8c8d", font)
draw.text((160, 0), u'demo> ', "#16a085", font)

exits = Image.new("RGBA", (240, 240), "#2c3e50")
draw = ImageDraw.Draw(exits)
draw.text((20, 40), u"Quit?\nLater can execute", "#1abc9c", font)
draw.text((30, 120), u"/home/app.py", "#16a085", font)

draw.text((20, 180), get_host_ip(), "#bdc3c7", font)
draw.text((0, 0), u'<No', "#2ecc71", font)
draw.text((180, 0), u'Yes> ', "#e74c3c", font)
# display.show(exits)


npu = nn.load({
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

camera.config(size=(224, 224))

# for i in range(320):
#   img = camera.capture()
#   if img:
#       out = npu.forward(img, quantize=True)
#       out = nn.F.softmax(out)
#       if out.max() > 0.1:
#           image = Image.new("RGBA", (240, 240), "#00000000")
#           draw = ImageDraw.Draw(image)
#           draw.text((0, 0), "{:.2f}: {}".format(
#               out.max(), labels[out.argmax()]), (255, 0, 0), font)
#           display.show(image)

# exit(0)

packet = {
    'selected': "main"
}

asyncio.set_event_loop(asyncio.new_event_loop())


keys = InputDevice('/dev/input/event0')


async def keys_events(packet, device):
    async for event in device.async_read_loop():
        # await asyncio.sleep(0.02)
        if event.value == 1 and event.code == 0x02:
            # print('press key S1')
            if packet["selected"] == "main":
                packet["selected"] = "demo"
            elif packet["selected"] == "exit":
                display.clear((0, 0, 0))
                if os.path.exists('/home/app.py') == False:
                  os.system('mv /home/main.py /home/app.py')
                  os.system('mv /home/startup.py /home/main.py')
                exit(233)
        elif event.value == 1 and event.code == 0x03:
            # print('press key S2')
            if packet["selected"] == "demo":
                packet["selected"] = "main"
            elif packet["selected"] == "main":
                packet["selected"] = "exit"
            elif packet["selected"] == "exit":
                packet["selected"] = "main"

for device in [keys]:
    asyncio.ensure_future(keys_events(packet, device))


async def main(packet):
    if packet["selected"] == "demo":
        img = camera.read()
        if img:
            out = npu.forward(img, quantize=True)
            out = nn.F.softmax(out)
            if out.max() > 0.1:
                image = Image.new("RGBA", (240, 240), "#00000000")
                draw = ImageDraw.Draw(image)
                draw.text((0, 0), "{:.2f}: {}".format(
                    out.max(), labels[out.argmax()]), (255, 0, 0), font)
                display.show(image)
                # print(out.max(), out.argmax())
        # else:
            # await asyncio.sleep(0.02)
    elif packet["selected"] == "save":
        img = camera.capture()
        if img:
            img.save('/mnt/UDISK/%d.jpg' % int(time.time()), quality=95)
            draw = ImageDraw.Draw(img)
            draw.text((0, 0), "cap!", (255, 0, 0), font)
            display.show(img)
            packet["selected"] = "demo"
    elif packet["selected"] == "main":
        display.show(canvas)
    elif packet["selected"] == "exit":
        display.show(exits)
    # else:
    #     # await asyncio.sleep(0.02)
    asyncio.ensure_future(main(packet))

asyncio.ensure_future(main(packet))

loop = asyncio.get_event_loop()
loop.run_forever()
