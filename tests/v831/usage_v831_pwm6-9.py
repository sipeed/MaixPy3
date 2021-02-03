from maix import pwm
import time

# pwm6.export()

with pwm.PWM(6) as pwm6:
  pwm6.period = 1000000
  pwm6.duty_cycle = 10000
  pwm6.enable = True
  while True:
    for i in range(50, 1, -10):
      pwm6.duty_cycle = i * 10000
      time.sleep(0.1)
    for i in range(1, 50, +10):
      pwm6.duty_cycle = i * 10000
      time.sleep(0.1)

# pwm6.unexport()
