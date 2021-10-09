"""Linux PWM driver sysfs interface"""

import os
__author__ = 'Scott Ellis'
__version__ = '1.0'
__license__ = 'New BSD'
__copyright__ = 'Copyright (c) 2016 Scott Ellis'

from types import (
    TracebackType,
)

from typing import (
    Optional,
    Type,
)

class PWM(object):
    """
    A class to work with the Linux PWM driver sysfs interface
    """

    def __init__(self, channel: int = 0, chip: int = 0) -> None:
        """ Specify channel and chip when creating an instance
        The Linux kernel driver exports a sysfs interface like this
            /sys/class/pwm/pwmchip<chip>/pwm<channel>
        A <chip> can have multiple <channels>.
        The channel and chip are determined by the kernel driver.
        For example the two PWM timers from the RPi kernel driver
        show up like this
            /sys/class/pwm/pwmchip0/pwm0
            /sys/class/pwm/pwmchip0/pwm1
        To use the RPi timers create instances this way
            pwm0 = PWM(0) or PWM(0,0)
            pwm1 = PWM(1) or PWM(1,0)
        """
        self._channel = channel
        self._chip = chip
        self.base = '/sys/class/pwm/pwmchip{:d}'.format(self._chip)
        self.path = self.base + '/pwm{:d}'.format(self._channel)
        if not os.path.isdir(self.base):
            raise FileNotFoundError('Directory not found: ' + self.base)

    # enable class as a context manager
    def __enter__(self) -> 'PWM':
        self.export()
        return self

    def __exit__(self,
            exc_type: Optional[Type[BaseException]],
            exc_value: Optional[BaseException],
            traceback: Optional[TracebackType]) -> None:
        self.enable = False
        self.inversed = False
        self.unexport()
        with open("/sys/class/sunxi_dump/write","wb") as f:
            f.write(self.gpio)
        return

    def export(self) -> None:
        """Export the channel for use through the sysfs interface.
        Required before first use.
        """
        if not os.path.isdir(self.path):
            with open(self.base + '/export', 'w') as f:
                f.write('{:d}'.format(self._channel))

    def unexport(self) -> None:
        """Unexport the channel.
        The sysfs interface is no longer usable until it is exported again.
        """
        if os.path.isdir(self.path):
            with open(self.base + '/unexport', 'w') as f:
                f.write('{:d}'.format(self._channel))

    @property
    def channel(self) -> int:
        """The channel used by this instance.
        Read-only, set in the constructor.
        """
        return self._channel

    @property
    def chip(self) -> int:
        """The chip used by this instance.
        Read-only, set in the constructor.
        """
        return self._chip

    @property
    def period(self) -> int:
        """The period of the pwm timer in nanoseconds."""
        with open(self.path + '/period', 'r') as f:
            value = f.readline().strip()

        return int(value)

    @period.setter
    def period(self, value: int) -> None:
        with open(self.path + '/period', 'w') as f:
            f.write('{:d}'.format(value))

    @property
    def duty_cycle(self) -> int:
        """The duty_cycle (the ON pulse) of the timer in nanoseconds."""
        with open(self.path + '/duty_cycle', 'r') as f:
            value = f.readline().strip()

        return int(value)

    @duty_cycle.setter
    def duty_cycle(self, value: int) -> None:
        with open(self.path + '/duty_cycle', 'w') as f:
            f.write('{:d}'.format(value))

    @property
    def enable(self) -> bool:
        """Enable or disable the timer, boolean"""
        with open(self.path + '/enable', 'r') as f:
            value = f.readline().strip()

        return True if value == '1' else False

    @enable.setter
    def enable(self, value: bool) -> None:
        with open(self.path + '/enable', 'w') as f:
            if value:
                f.write('1')
            else:
                f.write('0')

    @property
    def inversed(self) -> bool:
        """normal polarity or inversed, boolean"""
        with open(self.path + '/polarity', 'r') as f:
            value = f.readline().strip()

        return True if value == 'inversed' else False

    @inversed.setter
    def inversed(self, value: bool) -> None:
        with open(self.path + '/polarity', 'w') as f:
            if value:
                f.write('inversed')
            else:
                f.write('normal')


