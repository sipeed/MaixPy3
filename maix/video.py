
class MaixVideo():

    def __init__(self, size=(640, 480)):
        self._width, self._height = size

    def width(self):
        return self._width

    def height(self):
        return self._height

    def write(self):
        pass  # for file

    def read(self):
        return b'\xFF\x00\x00' * (self._width * self._height)

    def config(self, size):
        pass

    def capture(self, pillow=False):
        tmp = self.read()
        if tmp:
            try:
                if pillow == False:
                    from maix import image
                    return image.Image().load(tmp, (self._width, self._height), "RGB")
            except Exception as e:
                pass
            try:
                from PIL import Image
                return Image.frombytes("RGB", (self._width, self._height), tmp)
            except Exception as e:
                pass
        return None

    def close(self):
        pass  # for file
