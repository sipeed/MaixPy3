# 该指南可用于个人开发者开发自己想要的maix_version功能

_maix_image模块采用c++语言进行开发,通过pybind11模块进行C++与python的函数绑定.  
模块采用轻耦合设计,对于图像的不同功能采用不同的C++类对象进行封装.软件实现上为菱形继承方法,请不要破坏整个软件实现结构.  
class any_image类是整个_maix_image模块的基础图像容器,请尽量保持该类的不变.该类应由所有图像处理类`虚继承`.  
class maix_image类是对应的python接口类,主要给pybind11提供统一的类绑定函数,你实现的图像方法类应由该类进行继承.  

~~~ bash
            any_image
         |              |
         V              V
    maix_version      other
                | |
                 T
                 V
            maix_image
~~~

~~~ C++
class any_image
{
private:
public:
  Any py_to_pram[4][4];     // 快速转换表
  libmaix_image_t *_img;    //图像指针
  int _maix_image_width;    //图像宽度
  int _maix_image_height;   //图像高度
  std::string _maix_image_type; //图像类型
  int _maix_image_size;     //图像大小
  any_image();              //构造函数
  py_input_image_mode_t get_img_type(py::object &input_img);  //python对象识别函数
  int get_to(std::string &mode);    //返回快速转换表第一维索引
};

~~~
当 class other类虚拟继承any_image类后,在other类内部可以使用this指针访问any_image类内容.请开发者注意内存安全性.  
当编写完自己的other类后可以在maix_image类中进行public继承,然后在py_maix_image.cpp文件中进行函数绑定,之后就可以在pyhton的image类对象中使用作者自己的方法了.  

相关编程请参考maix_version的实现.  
欢迎个人开发者参与_maix_image模块的开发,为开源社区贡献自己的力量.  





个人参与开发模块实例过程:(在ubuntu中的过程,windows开发者可以在wls中完成以下操作)

第一步,从github上拉取maixpy3的源代码
~~~ bash
git clone https://github.com/sipeed/MaixPy3.git
cd MaixPy3
git submodule update --init
~~~
第二步,安装交叉编译工具链
~~~ bash
sudo apt install libffi-dev

wget https://github.com/sipeed/libmaix/releases/download/v0.1.0/toolchain-sunxi-musl-pack-2021-01-09.tar.xz

sudo cp toolchain-sunxi-musl-pack-2021-01-09.tar.xz /opt
sudo xz -d toolchain-sunxi-musl-pack-2021-01-09.tar.xz
sudo tar xvf toolchain-sunxi-musl-pack-2021-01-09.tar
sudo mv toolchain-sunxi-musl-pack-2021-01-09 v83x
source /opt/v83x/envsetup.sh
~~~
第三步编译maixpy3
~~~ bash
python3.8 setup.py bdist_wheel maix_v83x
#如果出现libffi.so.6错误,请查看本机的libffi库,如果版本过高可以通过建立软链接的方式创建出libffi.so.6
#如果出现pybind11没有,可运行python3.8 -m pip install pybind11进行安装.
#注意在安装pybind11时可能会报没有权限,这个时候执行`sudo su`,进入root用户,然后执行`source /opt/v83x/envsetup.sh`
#命令,加载交叉编译环境,然后再执行python3.8 -m pip install pybind11就可以装上了
~~~
请开发者享受编写代码的乐趣.