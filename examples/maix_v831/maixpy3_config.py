#!/usr/bin/env python
import os
import argparse
import time

os.system('clear')

class state:
  network = False  # [2021-01-23] only get ip not ping


def get_ipconfig():
  ADDR = None
  cmd = os.popen('ifconfig')
  for i in cmd.readlines():
      if 'inet ' in i and '127.0.0.1' not in i:
          ADDR = i
          break
  if ADDR:
    state.network = True
    print("\n[wifi] The IP address of your device, please remember.\r\n\r\n", ADDR)
  else:
    state.network = False
    print("\n[wifi] Your device is not connected to the Internet, shell input >>> maixpy3_config.py -ssid wifi_name -pasw wifi_pasw")


def set_wifi(SSID="Sipeed_2.4G", PASW="Sipeed123."):
  CFG_PATH = "/etc/wpa_supplicant.conf"
  wifi_cfg = 'ctrl_interface=/var/run/wpa_supplicant\nupdate_config=1\n\nnetwork={{\n\tssid="{SSID}"\n\tpsk="{PASW}"\n}}'
  print('\n[wifi] path:', CFG_PATH)

  config = wifi_cfg.format(SSID=SSID, PASW=PASW)
  with open("/etc/wpa_supplicant.conf", "w") as f:
    f.write(config)

  with open("/etc/wpa_supplicant.conf", "r+") as f:
    print('\n[wifi] result:', f.read())

  print('/etc/init.d/S40network restart')
  os.system('/etc/init.d/S40network restart')


def config_wifi():

  if args.ssid and args.pasw:
    set_wifi(args.ssid, args.pasw)
  else:
    try:
      print('\nconfig your wifi... [Ctrl + C > Exit] ')
      ssid = input('input your wifi name... ')
      pasw = input('input your wifi password... ')
      set_wifi(ssid, pasw)
    except KeyboardInterrupt as e:
      pass

  get_ipconfig()


def config_time():
  pass


def config_source():
  pass


def config_maixpy3():
  if state.network == False:
    print('\n[maixpy3] No Connect Network ...\n')
    return
  # /etc/init.d/S40network replace
  start_old = 'tcpsvd -E 0.0.0.0 21 ftpd -w / &'
  start_cfg = start_old + ' python3 -c "from maix import rpycs; rpycs.start()" &'
  stop_old = 'killall tcpsvd &'
  stop_cfg = stop_old + ' killall python3 &'

  try:
    print('\nconfig your maxipy3... [Ctrl + C > Exit]\n')

    print('\n[maixpy3] pip install maixpy3 --upgrade')
    os.system('pip install maixpy3 --upgrade')

    print('\n[maixpy3] cp /etc/init.d/S40network /etc/init.d/S40network.old')
    os.system('cp /etc/init.d/S40network /etc/init.d/S40network.old')
    config = ""
    with open('/etc/init.d/S40network', 'r+') as f:
        config = f.read()
    if start_cfg not in config and stop_cfg not in config:
      config = config.replace(start_old, start_cfg)
      config = config.replace(stop_old, stop_cfg)
    with open('/etc/init.d/S40network', 'w') as f:
        f.write(config)
  except KeyboardInterrupt as e:
    raise Exception('[maixpy3] restore config...')
  except Exception as e:
    print('\n[maixpy3] have bug %s' % str(e))
    print('\n[maixpy3] cp /etc/init.d/S40network.old /etc/init.d/S40network')
    os.system('cp /etc/init.d/S40network.old /etc/init.d/S40network')
  # finally:
  #   os.system('/etc/init.d/S40network restart')


if __name__ == '__main__':
  parser = argparse.ArgumentParser(
      description='Sipeed MaixPy3 system-config tools.')
  parser.add_argument('-ssid', type=str, help='your wifi name(ssid)')
  parser.add_argument('-pasw', type=str, help='your wifi password(pasw)')

  args = parser.parse_args()

  print('\nneed config your password?(passwd) [Ctrl + C > Exit] ')
  os.system('passwd')

  # print(args)
  config_wifi()

  config_maixpy3()

  if state.network:

    print('\n[update] opkg update')
    os.system('opkg update')

  print('\n[config] The configuration is finished, thanks for using.', time.asctime())
