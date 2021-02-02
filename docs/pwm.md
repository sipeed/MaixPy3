## pwm.py

see this https://github.com/scottellis/pwmpy

A simple Python class to work with the Linux PWM kernel driver.

Written for the Raspberry Pi and the hardware pwm timers, but should work with other SOCs.

Requires that a PWM kernel driver is loaded appropriately which on the RPi normally means a dts overlay.

Instructions for building and loading an appropriate dts for the RPi can be found [here][rpi-hardware-pwm].

I'm using a custom [Yocto built O/S][rpi-yocto] where pwm overlays are installed by default.

Here is a simple interactive example using the PWM class

    root@rpi3:~# python3
    Python 3.5.2 (default, Nov 22 2016, 06:54:46)
    [GCC 6.2.0] on linux
    Type "help", "copyright", "credits" or "license" for more information.

Import the *PWM* class from `pwm.py`

    >>> from pwm import PWM

Create an instance of the class for the *PWM0* timer

    >>> pwm0 = PWM(0)

Call *export()* before first use

    >>> pwm0.export()

Setup a 1 ms period, 25% duty cycle pulse (period and duty\_cycle units are nanoseconds)

    >>> pwm0.period = 1000000
    >>> pwm0.duty_cycle = 250000

Start the pwm timer

    >>> pwm0.enable = True

Change the duty cycle to 50%

    >>> pwm0.duty_cycle = 500000

Turn off the timer

    >>> pwm0.enable = False

Create an instance for the *PWM1* timer

    >>> pwm1 = PWM(1)
    >>> pwm1.export()

Setup a servo-like pulse with a 50 ms period and 2 ms duty\_cycle

    >>> pwm1.period = 50000000
    >>> pwm1.duty_cycle = 2000000
    >>> pwm1.enable = True

Set the servo pulse to 2.5 ms

    >>> pwm1.duty_cycle = 2500000

Set the servo pulse to 1.5 ms

    >>> pwm1.duty_cycle = 1500000

Back to 2.0 ms

    >>> pwm1.duty_cycle = 2000000

Turn it off

    >>> pwm1.enable = False

Unexport both timers

    >>> pwm0.unexport()
    >>> pwm1.unexport()
    >>> quit()
 

[rpi-hardware-pwm]: http://www.jumpnowtek.com/rpi/Using-the-Raspberry-Pi-Hardware-PWM-timers.html
[rpi-yocto]: http://www.jumpnowtek.com/rpi/Raspberry-Pi-Systems-with-Yocto.html
