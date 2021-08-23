import time
import os
import logging
import queue
from glob import glob
from http.server import HTTPServer, ThreadingHTTPServer, BaseHTTPRequestHandler
from threading import Thread
from abc import abstractmethod
from rpyc.utils.server import ThreadedServer
from rpyc.core.service import SlaveService
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
    def __init__(self, bytes: list):
        self.bytes = bytes

    def get_content_length(self):
        return len(self.bytes)

    def get_byte_generator(self):
        yield self.bytes


class FileImageHandler(BaseHTTPRequestHandler):
    def do_GET(self):
        logging.debug('GET response code: 200')
        self.send_response(200)
        # Response headers (multipart)
        for k, v in request_headers().items():
            self.send_header(k, v)
            logging.debug('GET response header: ' + k + '=' + v)
        # Multipart content
        self.serve_images()

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
        # logging.debug('GET response image: ' + filename)
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


def BytesImageHandlerFactory(q: queue.Queue):
    class BytesImageHandler(FileImageHandler):
        def __init__(self, request, client_address, server):
            self.queue = q
            super().__init__(request, client_address, server)

        def serve_images(self):
            i = 0
            t_start = time.time()
            while True:
                image = self.queue.get()
                self.serve_image(image)
                fps = (i + 1) / (time.time() - t_start)
                logging.debug("served image %d, overall fps: %0.3f" %
                              (i + 1, fps))
                i += 1

        def add_image(self, image: Image):
            self.queue.put(image)

    return BytesImageHandler


class MjpgServerThread(Thread):

  def __init__(self, name, port, handler=FileImageHandler):
      super(MjpgServerThread, self).__init__()
      self.name, self.port, self.handler = name, port, handler
      self.server = None

  def run(self):
    try:
      self.server = HTTPServer((self.name, self.port), self.handler)
      self.server.serve_forever()
    except Exception as e:
      # logging.debug('run: ' + str(e)) # [Errno 98] Address already in use
      pass

  def __del__(self):
    if self.server:
      self.server.shutdown()
      self.server = None


MjpgVar, HostName, MjpgPort, RpycPort = None, '0.0.0.0', 18811, 18812


def start_mjpg():
  global MjpgVar
  if MjpgVar == None or MjpgVar.is_alive() == False:
    MjpgVar = MjpgServerThread(HostName, MjpgPort)
    MjpgVar.start()
  return MjpgVar.is_alive()


def start(host='0.0.0.0', mjpg=18811, rpyc=18812, debug=True):
  if debug:
    logging.getLogger().setLevel(level=logging.DEBUG)

  global HostName, MjpgPort, RpycPort
  logging.debug('start %s %s %s %s' % (__file__, host, mjpg, rpyc))
  HostName, MjpgPort, RpycPort = host, mjpg, rpyc

  try:
    if start_mjpg():
      rpyc_server = ThreadedServer(
          SlaveService, hostname=HostName, port=RpycPort, reuse_addr=True)
      rpyc_server.start()
  except OSError as e:
    # logging.debug('[%s] OSError: %s' % (__file__, str(e))) # [Errno 98] Address already in use
    exit(0)

  global MjpgVar
  MjpgVar.__del__()


if __name__ == '__main__':
    def unit_test():
      from_files = False
      if from_files:
          # from files
          server = MjpgServerThread(HostName, MjpgPort)
          server.start()
      else:
          # from bytes; which could be coming from a bytestream or generated using e.g., opencv
          image_queue = queue.Queue(maxsize=100)
          handler_class = BytesImageHandlerFactory(q=image_queue)
          server = MjpgServerThread(HostName, MjpgPort, handler_class)
          server.start()
          for filename in glob('img/*.jpg'):
              image = FileImage(filename)
              logging.debug('GET response image: ' + filename)
              image_queue.put(image)
          # wait until the current queue has been served before quiting
          while not image_queue.empty():
              time.sleep(1)
    # unit_test()
    start()
