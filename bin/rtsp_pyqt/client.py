import socket
from threading import Thread
from typing import Union, Optional, List, Tuple
from time import sleep
from PIL import Image
from io import BytesIO

import re
from typing import Optional


class InvalidRTSPRequest(Exception):
    pass


class RTSPPacket:
    RTSP_VERSION = 'RTSP/1.0'

    INVALID = -1
    SETUP = 'SETUP'
    PLAY = 'PLAY'
    PAUSE = 'PAUSE'
    TEARDOWN = 'TEARDOWN'
    RESPONSE = 'RESPONSE'

    def __init__(
            self,
            request_type,
            video_file_path: Optional[str] = None,
            sequence_number: Optional[int] = None,
            dst_port: Optional[int] = None,
            session_id: Optional[str] = None):
        self.request_type = request_type
        self.video_file_path = video_file_path
        self.sequence_number = sequence_number
        self.session_id = session_id

        # if request_type SETUP
        self.rtp_dst_port = dst_port

    def __str__(self):
        return (f"RTSPPacket({self.request_type}, "
                f"{self.video_file_path}, "
                f"{self.sequence_number}, "
                f"{self.rtp_dst_port}, "
                f"{self.session_id})")

    @classmethod
    def from_response(cls, response: bytes):
        # only response format implemented, taken from server class:
        # """
        #   <RTSP_VERSION> 200 OK\r\n
        #   CSeq: <SEQUENCE_NUMBER>\r\n
        #   Session: <SESSION_ID>\r\n
        # """
        match = re.match(
            r"(?P<rtsp_version>RTSP/\d+.\d+) 200 OK\r?\n"
            r"CSeq: (?P<sequence_number>\d+)\r?\n"
            r"Session: (?P<session_id>\d+)\r?\n",
            response.decode()
        )

        if match is None:
            raise Exception(f"failed to parse RTSP response: {response}")

        g = match.groupdict()

        # not used, defaults to 1.0
        # rtsp_version = g.get('rtsp_version')
        sequence_number = g.get('sequence_number')
        session_id = g.get('session_id')

        try:
            sequence_number = int(sequence_number)
        except (ValueError, TypeError):
            raise Exception(f"failed to parse sequence number: {response}")

        if session_id is None:
            raise Exception(f"failed to parse session id: {response}")

        return cls(
            request_type=RTSPPacket.RESPONSE,
            sequence_number=sequence_number,
            session_id=session_id
        )

    @classmethod
    def build_response(cls, sequence_number: int, session_id: str):
        response = '\r\n'.join((
            f"{cls.RTSP_VERSION} 200 OK",
            f"CSeq: {sequence_number}",
            f"Session: {session_id}",
        )) + '\r\n'
        return response

    @classmethod
    def from_request(cls, request: bytes):
        # loosely follows actual rtsp protocol, considering only SETUP, PLAY, PAUSE, and TEARDOWN
        # https://en.wikipedia.org/wiki/Real_Time_Streaming_Protocol
        match = re.match(
            r"(?P<request_type>\w+) rtsp://(?P<video_file_path>\S+) (?P<rtsp_version>RTSP/\d+.\d+)\r?\n"
            r"CSeq: (?P<sequence_number>\d+)\r?\n"
            r"(Range: (?P<play_range>\w+=\d+-\d+\r?\n))?"
            r"(Transport: .*client_port=(?P<dst_port>\d+).*\r?\n)?"  # in case of SETUP request
            r"(Session: (?P<session_id>\d+)\r?\n)?",
            request.decode()
        )

        if match is None:
            raise InvalidRTSPRequest(f"failed to parse request: {request}")

        g = match.groupdict()
        request_type = g.get('request_type')

        if request_type not in (RTSPPacket.SETUP,
                                RTSPPacket.PLAY,
                                RTSPPacket.PAUSE,
                                RTSPPacket.TEARDOWN):
            raise InvalidRTSPRequest(f"invalid request type: {request}")

        video_file_path = g.get('video_file_path')
        # not used, defaults to `RTSPPacket.RTSP_VERSION`
        # rtsp_version = g.get('rtsp_version')
        sequence_number = g.get('sequence_number')
        dst_port = g.get('dst_port')
        session_id = g.get('session_id')

        if request_type == RTSPPacket.SETUP:
            try:
                dst_port = int(dst_port)
            except (ValueError, TypeError):
                raise InvalidRTSPRequest(f"failed to parse RTP port")
        try:
            sequence_number = int(sequence_number)
        except (ValueError, TypeError):
            raise InvalidRTSPRequest(f"failed to parse sequence number: {request}")

        return cls(
            request_type,
            video_file_path,
            sequence_number,
            dst_port,
            session_id
        )

    def to_request(self) -> bytes:
        # loosely follows actual rtsp protocol, considering only SETUP, PLAY, PAUSE, and TEARDOWN
        # https://en.wikipedia.org/wiki/Real_Time_Streaming_Protocol
        if any((attr is None for attr in (self.request_type,
                                          self.sequence_number,
                                          self.session_id))):
            raise InvalidRTSPRequest('missing one attribute of: `request_type`, `sequence_number`, `session_id`')

        if self.request_type in (self.INVALID, self.RESPONSE):
            raise InvalidRTSPRequest(f"invalid request type: {self}")

        request_lines = [
            f"{self.request_type} rtsp://{self.video_file_path} {self.RTSP_VERSION}",
            f"CSeq: {self.sequence_number}",
        ]
        if self.request_type == self.SETUP:
            if self.rtp_dst_port is None:
                raise InvalidRTSPRequest(f"missing RTP destination port: {self}")
            request_lines.append(
                f"Transport: RTP/UDP;client_port={self.rtp_dst_port}"
            )
        else:
            request_lines.append(
                f"Session: {self.session_id}"
            )
        request = '\r\n'.join(request_lines) + '\r\n'
        return request.encode()

class InvalidPacketException(Exception):
    pass


class RTPPacket:
    # default header info
    HEADER_SIZE = 12   # bytes
    VERSION = 0b10     # 2 bits -> current version 2
    PADDING = 0b0      # 1 bit
    EXTENSION = 0b0    # 1 bit
    CC = 0x0           # 4 bits
    MARKER = 0b0       # 1 bit
    SSRC = 0x00000000  # 32 bits

    class TYPE:
        MJPEG = 26

    def __init__(
            self,
            payload_type: int = None,
            sequence_number: int = None,
            timestamp: int = None,
            payload: bytes = None):

        self.payload = payload
        self.payload_type = payload_type
        self.sequence_number = sequence_number
        self.timestamp = timestamp

        # b0 -> v0 v1 p x c0 c1 c2 c3
        zeroth_byte = (self.VERSION << 6) | (self.PADDING << 5) | (self.EXTENSION << 4) | self.CC
        # b1 -> m pt0 pt1 pt2 pt3 pt4 pt5 pt6
        first_byte = (self.MARKER << 7) | self.payload_type
        # b2 -> s0 s1 s2 s3 s4 s5 s6 s7
        second_byte = self.sequence_number >> 8
        # b3 -> s8 s9 s10 s11 s12 s13 s14 s15
        third_byte = self.sequence_number & 0xFF
        # b4~b7 -> timestamp
        fourth_to_seventh_bytes = [
            (self.timestamp >> shift) & 0xFF for shift in (24, 16, 8, 0)
        ]
        # b8~b11 -> ssrc
        eigth_to_eleventh_bytes = [
            (self.SSRC >> shift) & 0xFF for shift in (24, 16, 8, 0)
        ]
        self.header = bytes((
            zeroth_byte,
            first_byte,
            second_byte,
            third_byte,
            *fourth_to_seventh_bytes,
            *eigth_to_eleventh_bytes,
        ))

    @classmethod
    def from_packet(cls, packet: bytes):
        if len(packet) < cls.HEADER_SIZE:
            raise InvalidPacketException(f"The packet {repr(packet)} is invalid")

        header = packet[:cls.HEADER_SIZE]
        payload = packet[cls.HEADER_SIZE:]

        # b1 -> m pt0 ... pt6
        # i.e. payload type is whole byte except first bit
        payload_type = header[1] & 0x7F
        # b2 -> s0 ~ s7
        # b3 -> s8 ~ s15
        # i.e. sequence number is b2<<8 | b3
        sequence_number = header[2] << 8 | header[3]
        # b4 ~ b7 -> t0 ~ t31
        timestamp = 0
        for i, b in enumerate(header[4:8]):
            timestamp = timestamp | b << (3 - i) * 8

        return cls(
            payload_type,
            sequence_number,
            timestamp,
            payload
        )

    def get_packet(self) -> bytes:
        return bytes((*self.header, *self.payload))

    def print_header(self):
        # print header without SSRC
        for i, by in enumerate(self.header[:8]):
            s = ' '.join(f"{by:08b}")
            # break line after the third and seventh bytes
            print(s, end=' ' if i not in (3, 7) else '\n')


class Client:
    DEFAULT_CHUNK_SIZE = 128 * 1024
    DEFAULT_RECV_DELAY = 20  # in milliseconds

    DEFAULT_LOCAL_HOST = '0.0.0.0'

    RTP_SOFT_TIMEOUT = 5  # in milliseconds
    # for allowing simulated non-blocking operations
    # (useful for keyboard break)
    RTSP_SOFT_TIMEOUT = 1# in milliseconds
    # if it's present at the end of chunk, client assumes
    # it's the last chunk for current frame (end of frame)
    PACKET_HEADER_LENGTH = 5

    def __init__(
            self,
            file_path: str,
            remote_host_address: str,
            remote_host_port: int,
            rtp_port: int):
        self._rtsp_connection: Union[None, socket.socket] = None
        self._rtp_socket: Union[None, socket.socket] = None
        self._rtp_receive_thread: Union[None, Thread] = None
        self._frame_buffer: List[Image.Image] = []
        self._current_sequence_number = 0
        self.session_id = ''

        self.current_frame_number = -1

        self.is_rtsp_connected = False
        self.is_receiving_rtp = False

        self.file_path = file_path
        self.remote_host_address = remote_host_address
        self.remote_host_port = remote_host_port
        self.rtp_port = rtp_port

    def get_next_frame(self) -> Optional[Tuple[Image.Image, int]]:
        if self._frame_buffer:
            self.current_frame_number += 1
            # skip 5 bytes which contain frame length in bytes
            return self._frame_buffer.pop(0), self.current_frame_number
        return None

    @staticmethod
    def _get_frame_from_packet(packet: RTPPacket) -> Image.Image:
        # the payload is already the jpeg
        raw = packet.payload
        frame = Image.open(BytesIO(raw))
        return frame

    def _recv_rtp_packet(self, size=DEFAULT_CHUNK_SIZE) -> RTPPacket:
        recv = bytes()
        print('Waiting RTP packet...')
        while True:
            try:
                recv += self._rtp_socket.recv(size)
                print('packet', len(recv))
                if recv.endswith(b'\xff\xd9'): # VideoStream.JPEG_EOF = b'\xff\xd9'
                    break
            except socket.timeout:
                continue
            except Exception as e:
                print(e)

        # print(f"Received from server: {repr(recv)}")
        return RTPPacket.from_packet(recv)

    def _start_rtp_receive_thread(self):
        self._rtp_receive_thread = Thread(target=self._handle_video_receive)
        self._rtp_receive_thread.setDaemon(True)
        self._rtp_receive_thread.start()

    def _handle_video_receive(self):
        self._rtp_socket = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
        self._rtp_socket.bind((self.DEFAULT_LOCAL_HOST, self.rtp_port))
        self._rtp_socket.settimeout(self.RTP_SOFT_TIMEOUT / 1000.)
        while True:
            if not self.is_receiving_rtp:
                sleep(self.RTP_SOFT_TIMEOUT/1000.)  # diminish cpu hogging
                continue
            packet = self._recv_rtp_packet()
            frame = self._get_frame_from_packet(packet)
            self._frame_buffer.append(frame)

    def establish_rtsp_connection(self):
        if self.is_rtsp_connected:
            print('RTSP is already connected.')
            return
        print(f"Connecting to {self.remote_host_address}:{self.remote_host_port}...")
        self._rtsp_connection = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        self._rtsp_connection.connect((self.remote_host_address, self.remote_host_port))
        self._rtsp_connection.settimeout(self.RTSP_SOFT_TIMEOUT / 1000.)
        self.is_rtsp_connected = True

    def close_rtsp_connection(self):
        if not self.is_rtsp_connected:
            print('RTSP is not connected.')
            return
        self._rtsp_connection.close()
        self.is_rtsp_connected = False
        if self._rtp_socket:
          self._rtp_socket.close()

    def _send_request(self, request_type=RTSPPacket.INVALID) -> RTSPPacket:
        if not self.is_rtsp_connected:
            raise Exception('rtsp connection not established. run `setup_rtsp_connection()`')
        request = RTSPPacket(
            request_type,
            self.file_path,
            self._current_sequence_number,
            self.rtp_port,
            self.session_id
        ).to_request()
        print(f"Sending request: {repr(request)}")
        self._rtsp_connection.send(request)
        self._current_sequence_number += 1
        return self._get_response()

    def send_setup_request(self) -> RTSPPacket:
        response = self._send_request(RTSPPacket.SETUP)
        self._start_rtp_receive_thread()
        self.session_id = response.session_id
        return response

    def send_play_request(self) -> RTSPPacket:
        response = self._send_request(RTSPPacket.PLAY)
        self.is_receiving_rtp = True
        return response

    def send_pause_request(self) -> RTSPPacket:
        response = self._send_request(RTSPPacket.PAUSE)
        self.is_receiving_rtp = False
        return response

    def send_teardown_request(self) -> RTSPPacket:
        response = self._send_request(RTSPPacket.TEARDOWN)
        self.is_receiving_rtp = False
        self.is_rtsp_connected = False
        return response

    def _get_response(self, size=DEFAULT_CHUNK_SIZE) -> RTSPPacket:
        rcv = None
        while True:
            try:
                rcv = self._rtsp_connection.recv(size)
                break
            except socket.timeout:
                continue
        # print(f"Received from server: {repr(rcv)}")
        response = RTSPPacket.from_response(rcv)
        return response
