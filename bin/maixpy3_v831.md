# Sipeed V831 使用文档（初稿）

| 日期          | 编辑人 | 备注                                     |
| ------------- | ------ | ---------------------------------------- |
| 2021年1月21日 | 大佬鼠 | 该版本仅在交流群范围内使用，不对外流通。 |
| 2021年1月21日 | 大佬鼠 | 增加更多讯息 |

## 检查硬件

当你拿到硬件，先检查外观是否有破损，请先确认天线/屏幕/摄像头均无破损，通电后电源红灯（power）亮起，表示硬件进入工作状态，注意两个 TypeC 口的用途不同的。

### 如何连接串口？

- 确保插入电脑通电后硬件可以被识别成串口设备。

- 参考  [Maix USB 驱动安装 ](https://maixpy.sipeed.com/zh/get_started/install_driver/nano.html?h=%E5%AE%89%E8%A3%85%E9%A9%B1%E5%8A%A8) 完成驱动的安装。
- 懒人就用  [V831>驱动精灵](https://share.weiyun.com/9H2RSOND) 自动安装驱动。

## 给 V831 烧录系统

请参考 [V831 镜像烧录](https://thoughts.teambition.com/share/5ff5aa2068c1c000466f61f3) 完成系统的烧写。

## 配置你的 linux 系统

在烧录后进入系统需要长达两分钟的等待系统初始化，如果超时没有反应可以按复位键重新进入系统，若是一直存在问题请将开机过程中出现的问题附图提交至社区（bbs.sipeed.com）。

> 2021年01月23日：系统已经内置了 ssh 和 ftp 并在开机后自动启动，服务配置于 `/etc/init.d/S40network` 。

进入系统后看到 `root@sipeed:/#` 提示就可以进行 linux 系统交互操作，输入命令 `maixpy3_config.py` 进行系统配置，有如下配置。

- 配置你的 root 用户的密码（默认:`123`），与 ssh & ftp 共用。
- 配置你的 WIFI 热点账号（ssid）密码（pasw）

> WIFI 配置文件位于 `/etc/wpa_supplicant.conf` 。

确认存在连接网络并获得 IP 地址了后，会进行 `pip install maixpy3` 包安装和最后的 `opkg update` 软件源更新。

```shell
root@sipeed:/# maixpy3_config.py 

need config your password?(passwd) [Ctrl + C > Exit] 
Changing password for root
New password: 
Bad password: too short
Retype password: 
passwd: password for root changed by root

config your wifi... [Ctrl + C > Exit] 
input your wifi name... Sipeed_2.4G
input your wifi password... Sipeed123.

[wifi] path: /etc/wpa_supplicant.conf

[wifi] result: ctrl_interface=/var/run/wpa_supplicant
update_config=1

network={
	ssid="Sipeed_2.4G"
	psk="Sipeed123."
}
/etc/init.d/S40network restart
Stopping network...
kill: you need to specify whom to kill
Starting network...
Successfully initialized wpa_supplicant
udhcpc: started, v1.27.2
udhcpc: sending discover
udhcpc: sending discover
udhcpc: sending select for 192.168.0.156
udhcpc: lease of 192.168.0.156 obtained, lease time 7200
udhcpc: ifconfig wlan0 192.168.0.156 netmask 255.255.255.0 broadcast 192.168.0.255
udhcpc: setting default routers: 192.168.0.1
tcpsvd: bind: Address in use

[wifi] The IP address of your device, please remember.

           inet addr:192.168.0.156  Bcast:192.168.0.255  Mask:255.255.255.0


config your maxipy3... [Ctrl + C > Exit]


[maixpy3] pip install maixpy3 --upgrade
Collecting maixpy3
  Downloading MaixPy3-0.1.9-cp38-cp38-linux_armv7l.whl (1.0 MB)
     |████████████████████████████████| 1.0 MB 43 kB/s 
Collecting pexpect
  Downloading pexpect-4.8.0-py2.py3-none-any.whl (59 kB)
     |████████████████████████████████| 59 kB 71 kB/s 
Collecting rpyc
  Downloading rpyc-5.0.1-py3-none-any.whl (68 kB)
     |████████████████████████████████| 68 kB 42 kB/s 
Requirement already satisfied, skipping upgrade: Pillow in /usr/lib/python3.8/site-packages (from maixpy3) (7.2.0)
Collecting ptyprocess>=0.5
  Downloading ptyprocess-0.7.0-py2.py3-none-any.whl (13 kB)
Collecting plumbum
  Downloading plumbum-1.6.9-py2.py3-none-any.whl (115 kB)
     |████████████████████████████████| 115 kB 84 kB/s 
Installing collected packages: ptyprocess, pexpect, plumbum, rpyc, maixpy3
Successfully installed maixpy3-0.1.9 pexpect-4.8.0 plumbum-1.6.9 ptyprocess-0.7.0 rpyc-5.0.1
WARNING: You are using pip version 20.1.1; however, version 21.0 is available.
You should consider upgrading via the '/usr/bin/python3 -m pip install --upgrade pip' command.

[maixpy3] cp /etc/init.d/S40network /etc/init.d/S40network.old

[update] opkg update
Downloading http://mirror.sipeed.com/maix_ii/base/Packages.gz.
Updated list of available packages in /var/opkg-lists/base.
Downloading http://mirror.sipeed.com/maix_ii/kernel/Packages.gz.
Updated list of available packages in /var/opkg-lists/kernel.
Downloading http://mirror.sipeed.com/maix_ii/luci/Packages.gz.
Updated list of available packages in /var/opkg-lists/luci.
Downloading http://mirror.sipeed.com/maix_ii/management/Packages.gz.
Updated list of available packages in /var/opkg-lists/management.
Downloading http://mirror.sipeed.com/maix_ii/packages/Packages.gz.
Updated list of available packages in /var/opkg-lists/packages.
Downloading http://mirror.sipeed.com/maix_ii/routing/Packages.gz.
Updated list of available packages in /var/opkg-lists/routing.
Downloading http://mirror.sipeed.com/maix_ii/targets/Packages.gz.
Updated list of available packages in /var/opkg-lists/targets.
Downloading http://mirror.sipeed.com/maix_ii/telephony/Packages.gz.
Updated list of available packages in /var/opkg-lists/telephony.

[config] The configuration is finished, thanks for using. Mon Jan 25 02:42:51 2021
root@sipeed:/# 

```

如果未能连接到网络，则会提前退出配置流程，此时可以重新运行输入配置过程，如下所示。

```shell
root@sipeed:/# maixpy3_config.py 

need config your password?(passwd) [Ctrl + C > Exit] 
Changing password for root
New password: 
Bad password: too short
Retype password: 
passwd: password for root changed by root

config your wifi... [Ctrl + C > Exit] 
input your wifi name... sipeed_wifi
input your wifi password... sipeed_pswd

[wifi] path: /etc/wpa_supplicant.conf

[wifi] result: ctrl_interface=/var/run/wpa_supplicant
update_config=1

network={
	ssid="sipeed_wifi"
	psk="sipeed_pswd"
}
/etc/init.d/S40network restart
Stopping network...
kill: you need to specify whom to kill
killall: wpa_supplicant: no process killed
Starting network...
Successfully initialized wpa_supplicant
udhcpc: started, v1.27.2
udhcpc: sending discover
udhcpc: sending discover
udhcpc: sending discover
udhcpc: no lease, forking to background
tcpsvd: bind: Address in use

[wifi] Your device is not connected to the Internet, shell input >>> maixpy3_config.py -ssid wifi_name -pasw wifi_pasw

[maixpy3] No Connect Network ...


[config] The configuration is finished, thanks for using. Mon Jan 25 02:41:21 2021
root@sipeed:/# 

```

> 上述每个步骤不想配置就可以用 [Ctrl + C] 跳过。

最后使用命令 `reboot` 重启机器，此后系统会开机启动远程调用服务（RPyC），你可以在 `ps` 命令中得知。

## 启动 jupyter 软件

输入 `ifconfig` 命令即可出现你的 IP 地址即可，在上述配置过程中也可以看到。

Windows 用户安装 [anaconda](https://www.anaconda.com/download/) 软件，国内下载推荐[清华源](https://mirror.tuna.tsinghua.edu.cn/help/anaconda/)，安装后系统就会添加 `python / pip / jupyter` 等命令。

- 在 Linux 下运行 `pip3 install rpyc_ikernel && python3 -m rpyc_ikernel.install`
- 在 Windows 下运行 `pip install rpyc_ikernel && python -m rpyc_ikernel.install`

确保有环境后，输入 `jupyter notebook` 启动编辑器，选中 RPyC 内核即可。

> 你也可以使用 jupyter lab 来启动，或是在 VSCode 里直接打开 jupyter 文档运行代码（不需要额外执行 jupyter notebook）。

更详细的可以参考 [rpyc_ikernel](https://github.com/sipeed/rpyc_ikernel) 仓库说明。

## 运行代码

在运行代码前，请配置 IP 地址进行连接，否则默认连接到 "localhost" 的地址请求 rpyc 服务。

```python
$connect("192.168.43.44")
import platform
print(platform.uname())
```

可见返回结果如下：

```shell
uname_result(system='Linux', node='linux-lab', release='5.4.0-56-generic', version='#62-Ubuntu SMP Mon Nov 23 19:20:19 UTC 2020', machine='x86_64', processor='x86_64')
```

本地运行摄像头显示到屏幕的代码如下：

```python
from maix import display, camera
while True:
  im = camera.capture()
  if im:
      display.show(im)
```

如需实时图传到本机，请查阅该文档[examples/hook_display.ipynb](https://github.com/sipeed/rpyc_ikernel/blob/master/examples/hook_display.ipynb)获取使用方法，运行它会把屏幕显示的图像传到浏览器的 jupyter notebook 编辑框中，在谷歌内核或360极速浏览器上测试通过。

## 常见问题

- 系统烧录步骤严格按照，先打开软件，拔 SD 卡后插入电脑，等待提示确认后再插入 SD 卡自动完成安装。

- 系统出现乱码进不去系统，此时应当重烧镜像，如果仍然出现，则需要更换 SD 卡重来。

- 由于有些国内网络环境不佳导致 maixpy3 安装不上，可以通过 `pip install MaixPy3 -i http://pypi.python.org/pypi --trusted-host pypi.python.org` 指定国内的 pip 软件源进行安装。

### 使用 jupyter 连不上的硬件

如果发现 jupyter 连接不上硬件，确定你已经选中并使用 RPyc 的内核。

优先检查本地网络防火墙端口放行规则（如 360 安全软件），确保能够 `ping` 通硬件 IP 地址。

```shell
juwan@juwan-N85-N870HL:~$ ping 192.168.0.156
PING 192.168.0.156 (192.168.0.156) 56(84) bytes of data.
64 bytes from 192.168.0.156: icmp_seq=1 ttl=64 time=304 ms
64 bytes from 192.168.0.156: icmp_seq=2 ttl=64 time=3.94 ms
```

确定本机网络没有问题后，可以确认硬件中的 `ps | grep python` 打印的进程中是否还存在 ` root     14880 S    python3 -c from maix import rpycs; rpycs.start()` 的进程状态为 S （运行中），如果不存在或状态为 R 则需要重启或自行手动运行 `python3 -c from maix import rpycs; rpycs.start()` 命令，系统默认自启。

```ps
root@sipeed:/# ps | grep python
  815 root     14944 S    python3 -c from maix import rpycs; rpycs.start()
  880 root      1144 S    grep python
root@sipeed:/# 
```

如果有其他问题可以提交到社区帖子进行反馈。

## 相关参考资料

- [MaixII_Dock(V831)_WIFI_使用/测试标准](https://thoughts.teambition.com/share/5ffd4d100d65cf00467d502c)

- [v831_PIN_引脚和（GPIO）功能的使用](https://thoughts.teambition.com/share/600659e9823a3d004a4e1c7a)

- [I2C_功能测试](https://thoughts.teambition.com/share/600950323cf39100462f8782)

- [CPython3 MaixPy3](https://github.com/sipeed/MaixPy3)

- [jupyter rpyc_ikernel](https://github.com/sipeed/rpyc_ikernel)
