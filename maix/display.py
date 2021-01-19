import io
import os

from PIL import Image

# remote jupyter options
remote = None


def hook(clear_output=True):
  global remote
  try:
    if remote != None:
      remote.clear_output = clear_output
      remote._start_display()
  except Exception as e:
    remote = None


try:
    __width__, __height__ = (640, 480)
    env = os.environ
    __width__, __height__ = (
        int(env['_MAIX_WIDTH_']), int(env['_MAIX_HEIGHT_']))
except Exception as e:
    print('[display] tips: os.environ(export) not _MAIX_WIDTH_ or _MAIX_HEIGHT_.')
finally:
    __display__ = Image.new("RGB", (__width__, __height__), (0, 0, 0))


def tobytes():
    global __display__
    return __display__.tobytes()


def set_window(size=(640, 480)):
    global __display__, __width__, __height__
    __width__, __height__ = size
    __display__ = Image.new("RGB", size, (0, 0, 0))


def __thumbnail__(src, dst):
    w, h = src.size
    if w > dst.width or h > dst.height:
        src.thumbnail((dst.width, dst.height))


def fill(box=(0, 0), color=(0, 0, 0)):
    global __display__
    if len(box) == 2:
        box = box + __display__.size
    __display__.paste(color, box)


try:
    __fastview__ = None
    from _maix_display import V831Display
    __fastview__ = V831Display(240, 240)

    def __draw__(im):
        global __fastview__
        im = im.resize(
            (__fastview__.width, __fastview__.height), Image.ANTIALIAS)
        __fastview__.draw(im.tobytes(), __fastview__.width,
                          __fastview__.height)
except ModuleNotFoundError as e:
    pass
except Exception as e:
    pass

_local_show = True


def local_show(value=True):
  global _local_show
  _local_show = value


def show(im=None, box=(0, 0), fast=True):
    global __display__, local_show
    if __fastview__ and fast:
      __display__ = im
      if _local_show and isinstance(im, Image.Image):
        __draw__(im)  # underlying optimization
    else:
      if isinstance(im, bytes):
        im = Image.frombytes("RGB", box, im)
        __thumbnail__(im, __display__)
        __display__.paste(im, (0, 0))
      elif isinstance(im, Image.Image):
        __thumbnail__(im, __display__)
        __display__.paste(im, box)
      if _local_show:
        __display__.show()


def clear(c=(0, 0, 0)):
    fill(color=c)
    show()


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
