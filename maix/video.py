
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
        from PIL import Image
        from maix import image
        tmp = self.read()
        if tmp:
            if pillow:
                return Image.frombytes("RGB", (self._width, self._height), tmp)
            else:
                return image.Image().load(tmp, (self._width, self._height), "RGB")
        return None

    def close(self):
        pass  # for file
