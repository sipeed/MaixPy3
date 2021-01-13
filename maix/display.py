
import shutil
import io
import os

from PIL import Image, ImageShow

try:
    # use fbviewer on linux
    # os.system('ln -s /usr/sbin/fbviewer /usr/sbin/display')
    if shutil.which("fbviewer"):
        class fbViewer(ImageShow.UnixViewer):
            def get_command_ex(self, file, **options):
                command = executable = "fbviewer"
                return command, executable
        ImageShow.register(fbViewer, 0)
except ModuleNotFoundError as e:
    pass

# try:
#     from _maix import Display
#     class dispViewer(ImageShow.UnixViewer):
#         def get_command_ex(self, file, **options):
#             print(file, **options)
#             command = executable = ""
#             return command, executable

#         def show_file(self, file, **options):
#             print(file, **options)
#     ImageShow.register(dispViewer, 0)
# except ModuleNotFoundError as e:
#     pass
# except Exception as e:
#     pass

# options
clear_output = True # for jupyter
sync_show, local_show = True, True

try:
    __width__, __height__ = (240, 240)
    env = os.environ
    __width__, __height__ = (env['_MAIX_WIDTH_'], env['_MAIX_HEIGHT_'])
except Exception as e:
    print('[display] tips: os.environ not _MAIX_WIDTH_ or _MAIX_HEIGHT_.')
finally:
    __display__ = Image.new("RGB", (__width__, __height__), (255, 255, 255))

def tobytes():
    global __display__
    return __display__.tobytes()

def set_window(size=(640, 480), update=False):
    global __display__, __width__, __height__
    __width__, __height__ = size
    if update:
        __display__ = Image.new("RGB", size)

def __thumbnail__(im):
    w, h = im.size
    if w > __display__.width or h > __display__.height:
        # print(w, __width__, h, __height__)
        im.thumbnail((__display__.width, __display__.height))

def fill(box=(0, 0), color=(0, 0, 0)):
    global __display__
    if len(box) == 2:
        box = box + __display__.size
    __display__.paste(color, box)


def show(im=None, box=(0, 0)):
    global __display__, local_show, sync_show

    if local_show:
        if isinstance(im, bytes):
            im = Image.frombytes("RGB", box, im)
            __thumbnail__(im)
            __display__.paste(im, (0, 0))
        elif isinstance(im, Image.Image):
            __thumbnail__(im)
            __display__.paste(im, box)
        __display__.show()

    if sync_show:
        pass # send


def clear(c=(0, 0, 0)):
    fill(color=c)


if __name__ == '__main__':
    clear((255, 0, 0))
    fill((20, 20, 220, 220), (0, 0, 0))
    fill((40, 40, 200, 200), (0, 0, 255))
    # show()

    from PIL import Image, ImageDraw
    image = Image.new("RGB", (100, 60), "#FFFFFF")
    draw = ImageDraw.Draw(image)
    draw.text((10, 10), u'hello world', (0, 0, 0))
    image = image.rotate(90)
    # image.show()
    # show(image, (0, 120))

    import numpy as np
    tmp = np.ndarray((10, 10, 3), np.uint8)
    tmp[:, :, 0] = 0
    tmp[:, :, 1] = 255
    tmp[:, :, 2] = 0
    image = Image.fromarray(tmp)

    tmp = b'\x00\xFF\x00' * (300 * 240)
    # show(tmp, (300, 240))
    # image = Image.frombytes("RGB", (300, 240), tmp)
    # print(image.tobytes())

    from PIL import Image, ImageDraw
    image = Image.new("RGB", (320, 240), "#FFFFFF")
    draw = ImageDraw.Draw(image)
    draw.text((0, 0), u'hello world', (0, 0, 0))
    # image.show()
    show(image)

