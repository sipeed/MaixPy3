# Copyright 2014 The Chromium OS Authors. All rights reserved.
# Use of this source code is governed by a BSD-style license that can be
# found in the LICENSE file.

"""Provides Linux kernel Watchdog interface.
https://www.kernel.org/doc/Documentation/watchdog/watchdog-api.txt
"""

import array
import fcntl
import struct

IO_WRITE = 0x40000000
IO_READ = 0x80000000
IO_READ_WRITE = 0xC0000000
IO_SIZE_INT = 0x00040000
IO_SIZE_40 = 0x00280000
IO_TYPE_WATCHDOG = ord('W') << 8

WDR_INT = IO_READ | IO_SIZE_INT | IO_TYPE_WATCHDOG
WDR_40 = IO_READ | IO_SIZE_40 | IO_TYPE_WATCHDOG
WDWR_INT = IO_READ_WRITE | IO_SIZE_INT | IO_TYPE_WATCHDOG

WDIOC_GETSUPPORT = 0 | WDR_40
WDIOC_GETSTATUS = 1 | WDR_INT
WDIOC_GETBOOTSTATUS = 2 | WDR_INT
WDIOC_GETTEMP = 3 | WDR_INT
WDIOC_SETOPTIONS = 4 | WDWR_INT
WDIOC_KEEPALIVE = 5 | WDR_INT
WDIOC_SETTIMEOUT = 6 | WDWR_INT
WDIOC_GETTIMEOUT = 7 | WDR_INT
WDIOC_SETPRETIMEOUT = 8 | WDWR_INT
WDIOC_GETPRETIMEOUT = 9 | WDR_INT
WDIOC_GETTIMELEFT = 10 | WDR_INT

WDIOF_OVERHEAT = 0x0001
WDIOF_FANFAULT = 0x0002
WDIOF_EXTERN1 = 0x0004
WDIOF_EXTERN2 = 0x0008
WDIOF_POWERUNDER = 0x0010
WDIOF_CARDRESET = 0x0020
WDIOF_POWEROVER = 0x0040
WDIOF_SETTIMEOUT = 0x0080
WDIOF_MAGICCLOSE = 0x0100
WDIOF_PRETIMEOUT = 0x0200
WDIOF_ALARMONLY = 0x0400
WDIOF_KEEPALIVEPING = 0x8000

WDIOS_DISABLECARD = 0x0001
WDIOS_ENABLECARD = 0x0002
WDIOS_TEMPPANIC = 0x0004

WATCHDOG_DEVICE = '/dev/watchdog'
WATCHDOG_STOP = 'V'
WATCHDOG_START = 'S'


class Watchdog:
  """Linux Kernel watchdog class."""

  def __init__(self, stop=True):
    super(Watchdog, self).__init__()
    self.fd = open(WATCHDOG_DEVICE, 'w')
    self.stop = stop
    if stop:
      self.Stop()

  def __del__(self) -> None: # for v831
    try:
      self.fd.close()
      import os
      os.system("echo V > " + WATCHDOG_DEVICE)
    except Exception as e:
      pass

  def Stop(self):
    self.fd.write(WATCHDOG_STOP)

  def Start(self):
    self.stop = False
    self.fd.write(WATCHDOG_START)

  def _IoctlInt(self, cmd, write_value=0):
    """Writes and reads integer.
    Args:
      cmd: ioctl command contains read/write flag.
      write_value: the 32bit value to write.
    Returns:
      32bit integer from ioctl mutable buffer.
    """
    buf = array.array('I', [write_value])
    start_temporary = self.stop
    # For stopped watchdog, we need to start it temporary to set a value as
    # some hardware accepts commands only when watchdog is running.
    if start_temporary:
      self.Start()
    fcntl.ioctl(self.fd, cmd, buf, True)
    if start_temporary:
      self.Stop()
    return buf[0]

  def GetStatus(self):
    """Gets watchdog status flags.
    The status and boot status are used to fetch the current status, and the
    status at the last reboot. Enabled options are described in GetSupport()
    return value. Status/option WDIOF_flags:
      WDIOF_OVERHEAT: CPU overheat triggered.
      WDIOF_FANFAULT: Fan failed triggered.
      WDIOF_EXTERN1/2: External source triggered.
      WDIOF_POWEROVER: Overvoltage detected.
      WDIOF_KEEPALIVEPING: Got a keep alive ping after last query.
      WDIOF_TIMEOUT: Reset due to timeout.
    """
    return self._IoctlInt(WDIOC_GETSTATUS)

  def GetBootStatus(self):
    return self._IoctlInt(WDIOC_GETBOOTSTATUS)

  def GetTemp(self):
    return self._IoctlInt(WDIOC_GETTEMP)

  def SetOptions(self, options):
    return self._IoctlInt(WDIOC_SETOPTIONS, options)

  def KeepAlive(self):
    if self.stop:
      self.Start()
    return self._IoctlInt(WDIOC_KEEPALIVE)

  def SetTimeout(self, timeout):
    timeout = int(timeout)
    if timeout <= 0:
      raise ValueError('timeout <= 0')
    return self._IoctlInt(WDIOC_SETTIMEOUT, timeout)

  def GetTimeout(self):
    return self._IoctlInt(WDIOC_GETTIMEOUT)

  def SetPreTimeout(self, timeout):
    timeout = int(timeout)
    if timeout <= 0 or timeout > 16:
      raise ValueError('0 < timeout <= 16')
    return self._IoctlInt(WDIOC_SETPRETIMEOUT, timeout)

  def GetPreTimeout(self):
    return self._IoctlInt(WDIOC_GETPRETIMEOUT)

  def GetTimeLeft(self):
    return self._IoctlInt(WDIOC_GETTIMELEFT)

if __name__ == '__main__':
  import wtd
  t = wtd.Watchdog()
  t.SetTimeout(1)
  import time
  t.KeepAlive()
  time.sleep(0.5)
  t.KeepAlive()
  time.sleep(0.5)
  t.__del__()
  del t
  t = wtd.Watchdog()
  del t
  t = wtd.Watchdog()
  t.SetTimeout(2)
  time.sleep(2)
  del t

