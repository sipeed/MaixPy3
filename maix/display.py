

_width, _height, __mode__ = 240, 240, "RGB"

def config(size=(240, 240)):
    global _width, _height
    if size[0] > 0 and size[1] > 0:
        _width, _height = size[0], size[1]

try:
    __show__ = None
    from _maix_display import Display
    __show__ = Display()
    config(size=(__show__.width, __show__.height))
    def __draw__(img):
        global __show__, _width, _height
        if isinstance(img, bytes):
          __show__.draw(img, _width, _height)
    from maix import camera
except ModuleNotFoundError as e:
    pass
except Exception as e:
    pass

__display__ = None

def as_image():
    from maix import image
    global __display__
    if __display__ is None:
      global _width, _height, __mode__
      __display__ = image.new(mode=__mode__, size=(_width, _height))
    return __display__

def width():
    global _width
    return _width

def height():
    global _height
    return _height

def show(img=None, box=(0, 0), local_show=True, remote_show=True):
    global __display__, __mode__, _width, _height
    if img is None:
        img = as_image()
    else:
        as_image()

    if remote_show:
        from maix import mjpg
        mjpg.store_mjpg(img)

    if local_show:
        try:
            from maix import image
            if isinstance(img, image.Image):
                if img.width != _width or img.height != _height:
                    img = img.resize(_width, _height)
                img = img.tobytes()
        except ImportError as e:
          pass
        if __show__:
            __draw__(img)
