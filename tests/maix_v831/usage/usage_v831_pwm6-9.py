from maix import pwm

import time

# period = 1000000 # 表示 pwm 的周期，单位 ns
# duty_cycle = 500000 # 表示占空比，单位 ns
# polarity = False   # 表示 pwm 极性 (normal/inversed)
# enable = True    # 表示是否使能 pwm

try:
  pwm6 = pwm.PWM(6)
  pwm6.export()
  pwm6.period = 25000000  # 表示 pwm 的周期，单位 ns
  pwm6.duty_cycle = 100  # 表示占空比，单位 ns
  pwm6.enable = True        # 表示是否使能 pwm
  for t in range (3):
    for i in range(0, 25000000, +1000000):
      pwm6.duty_cycle = i
      time.sleep(0.05)
    for i in range(25000000, 0, -1000000):
      pwm6.duty_cycle = i
      time.sleep(0.05)
finally:
  pwm6.unexport()

with pwm.PWM(6) as pwm6:
  pwm6.period = 25000000  # 表示 pwm 的周期，单位 ns
  pwm6.duty_cycle = 100  # 表示占空比，单位 ns
  pwm6.enable = True        # 表示是否使能 pwm
  for t in range (3):
    for i in range(0, 25000000, +1000000):
      pwm6.duty_cycle = i
      time.sleep(0.05)
    for i in range(25000000, 0, -1000000):
      pwm6.duty_cycle = i
      time.sleep(0.05)
