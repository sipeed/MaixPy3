
class MaixVideo():

    def __init__(self, size=(640, 480)):
        self.width, self.height = size

    def write(self):
        pass  # for file

    def read(self):
        return b'\xFF\x00\x00' * (self.width * self.height)

    def config(self, size):
        pass

    def capture(self):
        from PIL import Image
        tmp = self.read()
        if tmp:
            return Image.frombytes("RGB", (self.width, self.height), tmp)
        return None

    def close(self):
        pass  # for file
