from glob import glob
from rpyc.core.service import SlaveService
from rpyc.utils.server import ThreadedServer
from abc import abstractmethod
from threading import Thread
from http.server import HTTPServer, ThreadingHTTPServer, BaseHTTPRequestHandler
import time
import os
import logging
# import queue


class Queue(object):

    def __init__(self, maxsize):
        self.__list = []
        self.maxs = maxsize

    def data(self):
        return self.__list

    def empty(self):
        return self.__list == []

    def size(self):
        return len(self.__list)

    def put(self, item):
        self.__list.append(item)

    def get(self):
        if (self.size()):
          return self.__list.pop(0)
        return None

    def clear(self):
        self.__list = []


logging.basicConfig(level=logging.WARN)

boundary = '--boundarydonotcross'


def request_headers():
    return {
        'Cache-Control':
        'no-store, no-cache, must-revalidate, pre-check=0, post-check=0, max-age=0',
        'Connection': 'close',
        'Content-Type': 'multipart/x-mixed-replace;boundary=%s' % boundary,
        'Expires': 'Mon, 1 Jan 2030 00:00:00 GMT',
        'Pragma': 'no-cache',
        'Access-Control-Allow-Origin': '*'  # CORS
    }


class Image:
    def image_headers(self, mimetype='image/jpeg'):
        return {
            'X-Timestamp': time.time(),
            'Content-Length': self.get_content_length(),
            'Content-Type': mimetype,
        }

    @abstractmethod
    def get_content_length(self):
        """
        :return: integer number of bytes
        """
        raise NotImplementedError()

    @abstractmethod
    def get_byte_generator(self):
        """
        :return: generator of binary strings (chunks)
        """
        raise NotImplementedError()


class FileImage(Image):
    def __init__(self, filename):
        self.filename = filename

    def image_headers(self):
        ext = self.filename.split('.')[-1].lower()
        if ext in ('jpg', 'jpeg'):
            return super().image_headers(mimetype='image/jpeg')
        else:
            raise NotImplementedError("Only implemented for jpeg for now")

    def get_content_length(self):
        return os.path.getsize(self.filename)

    def get_byte_generator(self):
        with open(self.filename, "rb") as f:
            yield f.read()


class BytesImage(Image):
    def __init__(self, data: bytes):
        self.data = data

    def get_content_length(self):
        return len(self.data)

    def get_byte_generator(self):
        yield self.data


class FileImageHandler(BaseHTTPRequestHandler):
    def do_GET(self):
        logging.debug('GET response code: 200')
        self.send_response(200)
        # Response headers (multipart)
        for k, v in request_headers().items():
            self.send_header(k, v)
            logging.debug('GET response header: ' + k + '=' + v)
        # Multipart content
        try:
          self.serve_images()
        except KeyboardInterrupt as e:
          # BrokenPipeError: [Errno 32] Broken pipe
          logging.debug('run: ' + str(e))

    def serve_image(self, image: Image):
        # Part boundary string
        self.end_headers()
        self.wfile.write(bytes(boundary, 'utf-8'))
        self.end_headers()
        # Part headers
        for k, v in image.image_headers().items():
            self.send_header(k, v)
            logging.debug('GET response header: %s = %s' % (k, v))
        self.end_headers()
        # Part binary
        logging.debug('GET response image: ' + str(Image))
        try:
            for chunk in image.get_byte_generator():
                self.wfile.write(chunk)
        except (ConnectionResetError, ConnectionAbortedError):
            return

    def serve_images(self):
        t_start = time.time()
        for i, filename in enumerate(glob('img/*.jpg')):
            image = FileImage(filename)
            logging.debug('GET response image: ' + filename)
            self.serve_image(image)
            fps = (i + 1) / (time.time() - t_start)
            logging.debug("served image %d, overall fps: %0.3f" % (i + 1, fps))

    def log_message(self, format, *args):
        return


def BytesImageHandlerFactory(q: Queue):
    class BytesImageHandler(FileImageHandler):
        def __init__(self, request, client_address, server):
            self.queue = q
            super().__init__(request, client_address, server)

        def serve_images(self):
            i = 0
            t_start = time.time()
            image = None
            try:
                while True:
                    if self.queue.empty() == False:
                        image = self.queue.get()
                        self.serve_image(image)
                        logging.debug('queue: ' + str(image))
                        fps = (i + 1) / (time.time() - t_start)
                        logging.debug("served image %d, overall fps: %0.3f" %
                                      (i + 1, fps))
                        i += 1
            except Exception as e:
                if (image):
                  self.queue.put(image)
                logging.error(e)

        def add_image(self, image: Image):
            self.queue.put(image)

    return BytesImageHandler


class MaixImage(object):

    def __init__(self):
        self.img = None

    def put(self, image=None):
        logging.info('[0]image: (%s)' % (image))
        self.img = image

    def get(self):
        return self.img


def MaixImageHandlerFactory(img: MaixImage):
    class MaixImageHandler(FileImageHandler):
        def __init__(self, request, client_address, server):
            self.image = img
            self.tmp, self.old, self.load = None, None, 1
            super().__init__(request, client_address, server)

        def serve_images(self):
            logging.debug('serve_images: ' + str(self.image))
            try:
                while True:
                    self.tmp = self.image.get()
                    # logging.info('[1]image: (%s, %s)' % (self.old, self.tmp))
                    if (self.tmp != None):
                        if self.load != 0:
                            logging.info('[2]image: (%s, %s)' % (self.old, self.tmp))
                            self.serve_image(self.tmp)
                            self.load -= 1
                            continue
                        if self.old != self.tmp:
                            logging.info('[3]image: (%s, %s)' % (self.old, self.tmp))
                            self.serve_image(self.tmp)
                            self.tmp, self.old = None, self.tmp
                    time.sleep(0.01)
            except Exception as e:
                # [Errno 32] Broken pipe & [Errno 104] Connection reset by peer
                logging.debug(e)

    return MaixImageHandler


class MjpgServerThread(Thread):

  def __init__(self, name, port, handler=FileImageHandler):
      super(MjpgServerThread, self).__init__()
      self.name, self.port, self.handler = name, port, handler
      self.server = None

  def run(self):
    try:
      self.server = ThreadingHTTPServer((self.name, self.port), self.handler)
      self.server.serve_forever()
    except Exception as e:
      # logging.debug('run: ' + str(e)) # [Errno 98] Address already in use
      pass

  def __del__(self):
    if self.server:
      self.server.shutdown()
      self.server = None


HostName, RpycPort, MjpgSrv, MjpgPort, MjpgImage = '0.0.0.0', 18812, None, 18811, None


def clear_mjpg():
    global MjpgImage
    if MjpgImage:
        MjpgImage.put(None)


def store_mjpg(img):
  global MjpgImage
  if MjpgImage:
    from maix import utils
    if isinstance(img, bytes):
        MjpgImage.put(BytesImage(img))
        return
    from maix import image, utils
    if isinstance(img, image.Image):
        MjpgImage.put(BytesImage(utils.rgb2jpg(
            img.tobytes(), img.width, img.height)))
        return
    from PIL import Image
    if isinstance(img, Image.Image):
        if (img.mode == 'RGB'):
            frame = utils.rgb2jpg(img.tobytes(), img.width, img.height)
        elif (img.mode == 'RGBA'):
            frame = utils.rgb2jpg(img.convert(
                "RGB").tobytes(), img.width, img.height)
        MjpgImage.put(BytesImage(frame))
        return

def start_mjpg():
  global MjpgSrv, MjpgImage
  if MjpgSrv == None or MjpgSrv.is_alive() == False:
    # MjpgQueue = Queue(maxsize=size)
    MjpgImage = MaixImage()
    MjpgSrv = MjpgServerThread(
        HostName, MjpgPort, MaixImageHandlerFactory(img=MjpgImage))
    MjpgSrv.start()
  return MjpgSrv.is_alive()


def start(host='0.0.0.0', mjpg=18811, rpyc=18812, level=0):
  if level:
    log = logging.getLogger()
    console = logging.StreamHandler()
    formatter = logging.Formatter(
        '%(asctime)s - %(filename)s[line:%(lineno)d] - %(levelname)s: %(message)s')
    console.setFormatter(formatter)
    log.handlers = []
    log.addHandler(console)
    log.setLevel(level=level) # logging.DEBUG == 10 INFO 20

  global HostName, MjpgPort, RpycPort
  logging.info('start %s %s %s %s' % (__file__, host, mjpg, rpyc))
  HostName, MjpgPort, RpycPort = host, mjpg, rpyc

  try:
    if start_mjpg():
      rpyc_server = ThreadedServer(
          SlaveService, hostname=HostName, port=RpycPort, reuse_addr=True)
      rpyc_server.start()
  except OSError as e:
    # logging.debug('[%s] OSError: %s' % (__file__, str(e))) # [Errno 98] Address already in use
    exit(0)

  global MjpgSrv
  MjpgSrv.__del__()


class MjpgReader():
    """
    MJPEG format

    Content-Type: multipart/x-mixed-replace; boundary=--BoundaryString
    --BoundaryString
    Content-type: image/jpg
    Content-Length: 12390

    ... image-data here ...


    --BoundaryString
    Content-type: image/jpg
    Content-Length: 12390

    ... image-data here ...
    """

    def __init__(self, url: str):
        self._url = url

    def iter_content(self):
        """
        Raises:
            RuntimeError
        """
        import io
        import requests
        r = requests.get(self._url, stream=True)

        # parse boundary
        content_type = r.headers['content-type']
        index = content_type.rfind("boundary=")
        assert index != 1
        boundary = content_type[index+len("boundary="):] + "\r\n"
        boundary = boundary.encode('utf-8')

        rd = io.BufferedReader(r.raw)
        while True:
            self._skip_to_boundary(rd, boundary)
            length = self._parse_length(rd)
            yield rd.read(length)

    def _parse_length(self, rd) -> int:
        length = 0
        while True:
            line = rd.readline()
            if line == b'\r\n':
                return length
            if line.startswith(b"Content-Length"):
                length = int(line.decode('utf-8').split(": ")[1])
                assert length > 0

    def _skip_to_boundary(self, rd, boundary: bytes):
        for _ in range(10):
            if boundary in rd.readline():
                break
        else:
            raise RuntimeError("Boundary not detected:", boundary)


if __name__ == '__main__':

    start()  # test rpyc & mjpg

    def unit_test_s():
      from_files = False
      if from_files:
          # from files
          server = MjpgServerThread(HostName, MjpgPort)
          server.start()
      else:
          # from bytes; which could be coming from a bytestream or generated using e.g., opencv
          image_queue = Queue(maxsize=100)
          handler_class = BytesImageHandlerFactory(q=image_queue)
          server = MjpgServerThread(HostName, MjpgPort, handler_class)
          server.start()

          for filename in glob('img/*.jpg'):
              image = FileImage(filename)
              logging.debug('GET response image: ' + filename)
              image_queue.put(image)
          #   wait until the current queue has been served before quiting
          while not image_queue.empty():
              time.sleep(1)
    # unit_test_s()

    def unit_test_c():
        mr = MjpgReader("http://127.0.0.1:18811")
        try:
          for content in mr.iter_content():
            print(len(content))

            from PIL import Image
            from io import BytesIO
            tmp = Image.open(BytesIO(content))
            tmp.show()
            # import cv2
            # import numpy as np
            # i = cv2.imdecode(np.frombuffer(content, dtype=np.uint8), cv2.IMREAD_COLOR)
            # cv2.imshow('i', i)
            # if cv2.waitKey(1) == 27:
            #     break
        except ValueError as e:
          print(e)
    # unit_test_c()
    # test_mjpg html <img src="http://localhost:18811" />
    from maix import camera, mjpg
    # import queue
    import _maix

    queue = Queue(maxsize=8)
    mjpg.MjpgServerThread(
        "0.0.0.0", 18811, mjpg.BytesImageHandlerFactory(q=queue)).start()

    while True:
        img = camera.capture()
        jpg = _maix.rgb2jpg(img.convert("RGB").tobytes(),
                            img.width, img.height)
        Queue.put(mjpg.BytesImage(jpg))
        print(len(jpg))
