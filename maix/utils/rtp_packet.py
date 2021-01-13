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
