
# ext_modules/libi2c/README.md

'''
from maix import i2c
print(i2c.scan())
dev = i2c.I2C('/dev/i2c-2', 0x26)
print(dev)
print(dev.read(0x1, 1))

>>> from maix import i2c
>>> print(i2c.scan())
[38, 44, 118]
>>> dev = i2c.I2C('/dev/i2c-2', 0x26)
>>> print(dev)
Device address: 0x26, tenbit: False, internal(word) address: 1 bytes, page max 8 bytes, delay: 1ms
>>> print(dev.read(0x1, 1))
bytearray(b'\x13')
>>>
'''

try:
  from pylibi2c import I2CDevice as I2C, I2C_M_IGNORE_NAK, I2C_M_NOSTART, I2C_M_NO_RD_ACK
  def scan(path='/dev/i2c-2'):
    addrs = []
    for addr in range(0x00, 0x7F):
      try:
        tmp = None
        tmp = I2C(path, addr)
        tmp.ioctl_read(0, 1)
        addrs.append(addr)
      except OSError as e:
        pass
      finally:
        if tmp:
          tmp.close()
    for addr in range(0x7F, 0xFF):
      try:
        tmp = None
        tmp = I2C(path, addr, tenbit=True)
        tmp.ioctl_read(0, 1)
        addrs.append(addr)
      except OSError as e:
        pass
      finally:
        if tmp:
          tmp.close()
    return addrs
except ModuleNotFoundError as e:
  pass

if __name__ == '__main__':
  print(scan())
  bus, addr = '/dev/i2c-2', 0x26
  tmp = I2C(bus, addr)
  tmp.close()
  # i2c = I2CDevice(bus, addr, tenbit=False, iaddr_bytes=1, page_bytes=8, delay=1, flags=0)
  # iaddr_bytes(defualt 1 byte internal address)  I2C device internal(word) address bytes, such as: 24C04 1 byte, 24C64 2 bytes
  # page_bytes(default 8 bytes per page)          I2C max number of bytes per page, 1K/2K 8, 4K/8K/16K 16, 32K/64K 32 etc
  # flags(defualt 0)                              I2C i2c_ioctl_read/write flags, I2C_M_IGNORE_NAK, I2C_M_NOSTART, I2C_M_NO_RD_ACK
  # delay(defualt 1ms)                            I2C internal operate delay, unit millisecond
  # tenbit(defult False, 7-bit)                   I2C is 10 bit device address
