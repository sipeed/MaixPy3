import time
import os
from socketserver import BaseHTTPRequestHandler, TCPServer
import socket

from glob import glob

from threading import Thread

from rpyc.utils.server import ThreadedServer
from rpyc.core.service import SlaveService

import logging
logging.basicConfig(level=logging.WARNING)

boundary = '--boundarydonotcross'

def request_headers():
    return {
        'Cache-Control': 'no-store, no-cache, must-revalidate, pre-check=0, post-check=0, max-age=0',
        'Connection': 'close',
        'Content-Type': 'multipart/x-mixed-replace;boundary=%s' % boundary,
        'Expires': 'Mon, 1 Jan 2030 00:00:00 GMT',
        'Pragma': 'no-cache',
      		'Access-Control-Allow-Origin': '*'  # CORS
    }

def image_headers(filename):
    return {
        'X-Timestamp': time.time(),
        'Content-Length': os.path.getsize(filename),
        #FIXME: mime-type must be set according file content
        'Content-Type': 'image/jpeg',
    }

# FIXME: should take a binary stream

def image_data(filename):
    with open(filename, "rb") as f:
        # for byte in f.read(1) while/if byte ?
        byte = f.read(1)
        while byte:
            yield byte
            # Next byte
            byte = f.read(1)


class MjpgServerThread(Thread):

  class Server(TCPServer):

      allow_reuse_address = True  # [Errno 98] Address already in use
      daemon_threads = True

      class _Mjpg_(BaseHTTPRequestHandler):

        def do_GET(self):
            logging.debug('GET response code: 200')
            self.send_response(200)
            # Response headers (multipart)
            for k, v in request_headers().items():
                self.send_header(k, v)
                logging.debug('GET response header: ' + k + '=' + v)
            # Multipart content
            for filename in glob('img/*.jpg'):
                logging.debug('GET response image: ' + filename)
                # Part boundary string
                self.end_headers()
                self.wfile.write(bytes(boundary, 'utf-8'))
                self.end_headers()
                # Part headers
                for k, v in image_headers(filename).items():
                    self.send_header(k, v)
                    # logging.debug('GET response header: ' + k + '=' + v)
                self.end_headers()
                # Part binary
                # logging.debug('GET response image: ' + filename)
                for chunk in image_data(filename):
                    self.wfile.write(chunk)

        def log_message(self, format, *args):
            return

      def __init__(self, Address):
          """Set up an initially empty mapping between a user' s nickname
          and the file-like object used to send data to that user."""
          TCPServer.__init__(
              self, Address, MjpgServerThread.Server._Mjpg_, bind_and_activate=True)

  def __init__(self, name, port):
      super(MjpgServerThread, self).__init__()
      self.name, self.port = name, port
      self.server = None

  def run(self):
    try:
      self.server = MjpgServerThread.Server((self.name, self.port))
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
  if MjpgVar == None or MjpgVar.isAlive() == False:
    MjpgVar = MjpgServerThread(HostName, MjpgPort)
    MjpgVar.start()
  return MjpgVar.isAlive()


def start(host='0.0.0.0', mjpg=18811, rpyc=18812, debug=False):
  global HostName, MjpgPort, RpycPort
  # logging.debug('start %s %s %s %s' % (__file__, host, mjpg, rpyc))
  HostName, MjpgPort, RpycPort = host, mjpg, rpyc

  if debug:
    logging.basicConfig(level=logging.debug)

  start_mjpg()
  try:
    rpyc_server = ThreadedServer(
        SlaveService, hostname=HostName, port=RpycPort, reuse_addr=True)
    rpyc_server.start()
  except OSError as e:
    # logging.debug('[%s] OSError: %s' % (__file__, str(e))) # [Errno 98] Address already in use
    exit(0)

  global MjpgVar
  MjpgVar.__del__()


if __name__ == '__main__':
  start()
