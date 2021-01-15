from socketserver import BaseRequestHandler, TCPServer
import socket
import time

from threading import Thread

from rpyc.utils.server import ThreadedServer
from rpyc.core.service import SlaveService

from .utils.video_stream import VideoStream
from .utils.rtsp_packet import RTSPPacket
from .utils.rtp_packet import RTPPacket


class Server:
  FRAME_PERIOD = 1000 // VideoStream.DEFAULT_FPS  # in milliseconds
  SESSION_ID = '000001'
  DEFAULT_CHUNK_SIZE = 4 * 1024

  # for allowing simulated non-blocking operations
  # (useful for keyboard break)
  RTSP_SOFT_TIMEOUT = 100  # in milliseconds

  class STATE:
      INIT = 0
      PAUSED = 1
      PLAYING = 2
      FINISHED = 3
      TEARDOWN = 4

  def __init__(self: object):
      self._video_stream: Union[None, VideoStream] = None
      self._rtp_send_thread: Union[None, Thread] = None
      self._rtsp_connection: Union[None, socket.socket] = None
      self._rtp_socket: Union[None, socket.socket] = None
      self._client_address: (str, int) = None
      self.server_state: int = self.STATE.INIT

  def __del__(self):
    if self._rtsp_connection:
      self._rtsp_connection.close()
      self._rtsp_connection = None
    if self._video_stream:
      self._video_stream.close()
      self._video_stream = None
    if self._rtp_socket:
      self._rtp_socket.close()
      self._rtp_socket = None

  def _rtsp_recv(self, size=DEFAULT_CHUNK_SIZE) -> bytes:
      recv = None
      while True:
          try:
              recv = self._rtsp_connection.recv(size)
              break
          except socket.timeout:
              continue
      print(f"Received from client: {repr(recv)}")
      return recv

  def _rtsp_send(self, data: bytes) -> int:
      print(f"Sending to client: {repr(data)}")
      return self._rtsp_connection.send(data)

  def _get_rtsp_packet(self) -> RTSPPacket:
      return RTSPPacket.from_request(self._rtsp_recv())

  def _wait_connection(self, sock, addr):
      self._rtsp_connection, self._client_address = sock, addr
      self._rtsp_connection.settimeout(self.RTSP_SOFT_TIMEOUT/1000.)
      print(
          f"Accepted connection from {self._client_address[0]}:{self._client_address[1]}")

  def _wait_setup(self):
      if self.server_state != self.STATE.INIT:
          raise Exception('server is already setup')
      while True:
          packet = self._get_rtsp_packet()
          if packet.request_type == RTSPPacket.SETUP:
              self.server_state = self.STATE.PAUSED
              print('State set to PAUSED')
              self._client_address = self._client_address[0], packet.rtp_dst_port
              self._setup_rtp(packet.video_file_path)
              self._send_rtsp_response(packet.sequence_number)
              break  # will exit rtsp

  def _start_rtp_send_thread(self):
      self._rtp_send_thread = Thread(target=self._handle_video_send)
      self._rtp_send_thread.setDaemon(True)
      self._rtp_send_thread.start()

  def _setup_rtp(self, video_file_path: str):
      print(f"Opening up video stream for file {video_file_path}")
      self._video_stream = VideoStream(video_file_path)
      print('Setting up RTP socket...')
      self._rtp_socket = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
      self._start_rtp_send_thread()

  def handle_rtsp_requests(self):
      print("Waiting for RTSP requests...")
      # main thread will be running here most of the time
      while True:
          packet = self._get_rtsp_packet()
          # assuming state will only ever be PAUSED or PLAYING at this point
          if packet.request_type == RTSPPacket.PLAY:
              if self.server_state == self.STATE.PLAYING:
                  print('Current state is already PLAYING.')
                  continue
              self.server_state = self.STATE.PLAYING
              print('State set to PLAYING.')
          elif packet.request_type == RTSPPacket.PAUSE:
              if self.server_state == self.STATE.PAUSED:
                  print('Current state is already PAUSED.')
                  continue
              self.server_state = self.STATE.PAUSED
              print('State set to PAUSED.')
          elif packet.request_type == RTSPPacket.TEARDOWN:
              print('Received TEARDOWN request, shutting down...')
              self._send_rtsp_response(packet.sequence_number)
              self.__del__()
              self.server_state = self.STATE.TEARDOWN
              # for simplicity's sake, caught on main_server
              # raise ConnectionError('teardown requested')
              return None
          else:
              # will never happen, since exception is raised inside `parse_rtsp_request()`
              # raise InvalidRTSPRequest()
              pass
          self._send_rtsp_response(packet.sequence_number)

  def _send_rtp_packet(self, packet: bytes):
      to_send = packet[:]
      while to_send:
          try:
              if self._rtp_socket:
                  self._rtp_socket.sendto(
                      to_send[:self.DEFAULT_CHUNK_SIZE], self._client_address)
          except socket.error as e:
              print(f"failed to send rtp packet: {e}")
              return
          # trim bytes sent
          to_send = to_send[self.DEFAULT_CHUNK_SIZE:]

  def _handle_video_send(self):
      print(
          f"Sending video to {self._client_address[0]}:{self._client_address[1]}")
      while True:
          if self.server_state == self.STATE.TEARDOWN:
              return
          if self.server_state != self.STATE.PLAYING:
              time.sleep(0.5)  # diminish cpu hogging
              continue
          if self._video_stream.current_frame_number >= VideoStream.VIDEO_LENGTH-1:  # frames are 0-indexed
              print('Reached end of file.')
              self.server_state = self.STATE.FINISHED
              return
          frame = self._video_stream.get_next_frame()
          frame_number = self._video_stream.current_frame_number
          rtp_packet = RTPPacket(
              payload_type=RTPPacket.TYPE.MJPEG,
              sequence_number=frame_number,
              timestamp=frame_number*self.FRAME_PERIOD,
              payload=frame
          )
        #   print(f"Sending packet #{frame_number}")
        #   print('Packet header:')
        #   rtp_packet.print_header()
          packet = rtp_packet.get_packet()
          self._send_rtp_packet(packet)
          time.sleep(self.FRAME_PERIOD/1000.)

  def _send_rtsp_response(self, sequence_number: int):
      response = RTSPPacket.build_response(sequence_number, self.SESSION_ID)
      self._rtsp_send(response.encode())
      print('Sent response to client.')


class _Rtsp_(BaseRequestHandler):
  def handle(self):  # from BaseRequestHandler
    try:
      s = Server()
      s._wait_connection(self.request, self.client_address)
      s._wait_setup()
      s.handle_rtsp_requests()  # keep rtp udp
    except BrokenPipeError as e:
      print(e)

class RtspServerThread(Thread):

  def __init__(self, name, port):
      super(RtspServerThread, self).__init__()
      self.HostName, self.RtspPort = name, port

  def run(self):
    try:
      server = TCPServer((self.HostName, self.RtspPort), _Rtsp_, bind_and_activate=True)
      server.serve_forever()
    except Exception as e:
      print(e)

RtspServer, HostName, RtspPort, RpycPort = None, '0.0.0.0', 18811, 18812

def start_rtsp():
  global RtspServer
  if RtspServer == None or RtspServer.isAlive() == False:
    RtspServer = RtspServerThread(HostName, RtspPort)
    RtspServer.start()
  return RtspServer.isAlive()

def start(host='0.0.0.0', rtsp=18811, rpyc=18812):
  global HostName, RtspPort, RpycPort
  print(__file__, 'start', host, rtsp, rpyc)
  HostName, RtspPort, RpycPort = host, rtsp, rpyc

  start_rtsp()

  rpyc_server = ThreadedServer(
      SlaveService, hostname=HostName, port=RpycPort, reuse_addr=True)
  rpyc_server.start()
  
  global RtspServer
  RtspServer.server.shutdown()  # close rtsp_server


if __name__ == '__main__':
  start()
