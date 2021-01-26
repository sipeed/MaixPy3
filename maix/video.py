
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

    def capture(self):
        from PIL import Image
        tmp = self.read()
        if tmp:
            return Image.frombytes("RGB", (self._width, self._height), tmp)
        return None

    def close(self):
        pass  # for file
