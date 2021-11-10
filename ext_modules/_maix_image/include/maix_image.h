#ifndef MAIX_IMAGE_H
#define MAIX_IMAGE_H

#include <iostream>
#include <sstream>
#include "any.h"

#include <pybind11/pybind11.h>
#include <pybind11/embed.h>
#include <pybind11/stl.h>
#include <vector>
#include <list>
#include <array>
#include <map>
#include <set>
#include <typeinfo>

#include "opencv2/core.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/videoio.hpp"
#include "opencv2/imgcodecs.hpp"

#include <opencv2/opencv.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/imgcodecs/legacy/constants_c.h>
#include "opencv2/core/types_c.h"

#include "libmaix_cv_image.h"
#include "libmaix_disp.h"
// #include "NanoLog.hpp"

#define enable_PIL //启用PIL支持

namespace py = pybind11;

class any_image
{
private:
public:
  Any py_to_pram[4][4];
  int get_to(std::string &mode);
  libmaix_image_t *_img;
  int _maix_image_width;
  int _maix_image_height;
  std::string _maix_image_type;
  int _maix_image_size;
  any_image();
  // ~any_image();
};

class maix_version : virtual public any_image
{
private:
  int Distance(int x1, int y1, int x2, int y2);

  // //==================================================================
  //   //函数名：  out_img_to_py_img
  //   //作者：    dianjixz
  //   //日期：    2021-10-27
  //   //功能：    将opencv的Mat图像转换成输出图像,和输入图像保持一致
  //   //输入参数：  py_img  _out
  //   //          py_img (py::object）输入图像
  //   //          _out    (cv::Mat） 将要转换的图像
  //   //返回值：  类型（py::object)
  //   //          返回转换好的图像
  //   //修改记录：
  //   //==================================================================
  //   py::object out_img_to_py_img(py::object &py_img, cv::Mat &_out)
  //   {
  //     if (py::isinstance<py::bytes>(py_img))
  //     {
  //       py::bytes tmp((const char *)_out.data, _out.rows * _out.cols * _out.channels());
  //       return tmp;
  //     }
  //     else
  //     {
  //       auto _PIL_ = py::module::import("PIL.Image");
  //       if (py::isinstance(py_img, this->PIL_))
  //       {
  //         // auto tobytes = PIL_.attr("tobytes");
  //         // auto img_bytes = tobytes(py_img);
  //         auto frombytes = _PIL_.attr("frombytes");
  //         // py::bytes tmp;
  //         py::bytes tmp((const char *)_out.data, _out.rows * _out.cols * _out.channels());
  //         if (_out.channels() == 3)
  //         {
  //           py::str str_tmp = "RGB";
  //           py::tuple sizek = py::make_tuple(_out.rows, _out.cols);
  //           auto PIL_img = frombytes(str_tmp, sizek, tmp);
  //           return PIL_img;
  //         }
  //         else if (_out.channels() == 4)
  //         {
  //           py::str str_tmp = "RGBA";
  //           py::tuple sizek = py::make_tuple(_out.rows, _out.cols);
  //           auto PIL_img = frombytes(str_tmp, sizek, tmp);
  //           return PIL_img;
  //         }
  //         else if (_out.channels() == 1)
  //         {
  //           py::str str_tmp = "L";
  //           py::tuple sizek = py::make_tuple(_out.rows, _out.cols);
  //           auto PIL_img = frombytes(str_tmp, sizek, tmp);
  //           return PIL_img;
  //         }
  //       }
  //     }
  //     return py::none();
  //   }

  //     //==================================================================
  //   //函数名：  py_img_to_in_img
  //   //作者：    dianjixz
  //   //日期：    2021-10-01
  //   //功能：    将函数的输入图像参数转换成opencv的Mat图像
  //   //输入参数：
  //   //          py_img  (py::object） 传递的python对象
  //   //          _out    (cv::Mat）    转换好输出的图像
  //   //          size    (vector<int>) 图像的尺寸(非必须)
  //   //          mode    (int)         图像的格式(非必须)
  //   //返回值：
  //   //          转换成功 0
  //   //          转换失败 -1
  //   //修改记录：
  //   //==================================================================
  //   int py_img_to_in_img(py::object &py_img, cv::Mat &_out, std::vector<int> &size, int mode)
  //   {
  //     if (py::isinstance<py::bytes>(py_img))
  //     {
  //       string tmp = py_img.cast<string>();
  //       if (size[0] == 0 || size[1] == 0)
  //       {
  //         // cv::Mat input(240, 240, CV_8UC3, const_cast<char *>(tmp.c_str()));
  //         cv::Mat input(240, 240, mode, const_cast<char *>(tmp.c_str()));
  //         input.copyTo(_out);
  //       }
  //       else
  //       {
  //         cv::Mat input(size[0], size[1], mode, const_cast<char *>(tmp.c_str()));
  //         // cv::Mat input(size[0], size[1], CV_8UC3, const_cast<char *>(tmp.c_str()));
  //         input.copyTo(_out);
  //       }
  //     }
  //     else
  //     {
  //       // auto PIL_ = py::module::import("PIL.Image").attr("Image");
  //       if (py::isinstance(py_img, this->PIL_))
  //       {
  //         // auto tobytes = PIL_.attr("tobytes");
  //         // auto img_bytes = tobytes(py_img);
  //         auto img_bytes = this->py_img_tobytes_(py_img);
  //         auto tmp = img_bytes.cast<string>();
  //         auto img_size = py_img.attr("size").cast<vector<int>>();
  //         auto img_mode = py_img.attr("mode").cast<string>();
  //         if (img_mode == "RGB")
  //         {
  //           // cv::Mat input(img_size[0], img_size[1], CV_8UC3, const_cast<char *>(tmp.c_str()));
  //           cv::Mat input(img_size[0], img_size[1], CV_8UC3, const_cast<char *>(tmp.c_str()));
  //           input.copyTo(_out);
  //         }
  //         else if (img_mode == "RGBA")
  //         {
  //           cv::Mat input(img_size[0], img_size[1], CV_8UC4, const_cast<char *>(tmp.c_str()));
  //           input.copyTo(_out);
  //         }
  //         else if (img_mode == "L" || img_mode == "1")
  //         {
  //           cv::Mat input(img_size[0], img_size[1], CV_8UC1, const_cast<char *>(tmp.c_str()));
  //           input.copyTo(_out);
  //         }
  //       }
  //     }
  //     return 0;
  //   }

public:
  maix_version();
  void version_test();

  //==================================================================
  //函数名：  _maix_vision_medianBlur
  //作者：    dianjixz
  //日期：    2021-10-27
  //功能：    图像中值滤波函数
  //输入参数：
  //          py::object py_img     python输入图像对象
  //          int m_size            中值滤波的核大小
  //          std::vector<int> size      图像的尺寸(非必须)
  //          int mode              图像的格式(非必须)
  //返回值：
  //          返回中值滤波后的图像,图像格式和输入保持一致；
  //修改记录：
  //==================================================================
  void _maix_vision_medianBlur(int m_size);

  //==================================================================
  //函数名：  _maix_vision_GaussianBlur
  //作者：    dianjixz
  //日期：    2021-10-27
  //功能：    图像高斯滤波函数
  //输入参数：
  //          py::object py_img               python输入图像对象
  //          Size ksize:                     高斯内核大小，这个尺寸与前面两个滤波kernel尺寸不同，ksize.width和ksize.height可以不相同但是这两个值必须为正奇数，如果这两个值为0，他们的值将由sigma计算。
  //          double sigmaX:                  高斯核函数在X方向上的标准偏差
  //          double sigmaY:                  高斯核函数在Y方向上的标准偏差，如果sigmaY是0，则函数会自动将sigmaY的值设置为与sigmaX相同的值，如果sigmaX和sigmaY都是0，这两个值将由ksize.width和ksize.height计算而来。具体可以参考getGaussianKernel()函数查看具体细节。建议将size、sigmaX和sigmaY都指定出来。
  //          int borderType=BORDER_DEFAULT:  推断图像外部像素的某种便捷模式，有默认值BORDER_DEFAULT，如果没有特殊需要不用更
  //          std::vector<int> size                图像的尺寸(非必须)
  //          int mode                        图像的格式(非必须)
  //返回值：
  //          返回高斯滤波后的图像,图像格式和输入保持一致；
  //修改记录：
  //==================================================================
  void _maix_vision_GaussianBlur(std::vector<int> ksize, double sigmaX, double sigmaY, int borderType, std::vector<int> size, int mode);

  //==================================================================
  //函数名：  _maix_vision_Canny
  //作者：    dianjixz
  //日期：    2021-10-27
  //功能：    边缘检测函数
  //输入参数：
  //          py::object py_img     python输入图像对象
  //          int thr_h             最大阈值
  //          int thr_l             最小阈值
  //          std::vector<int> size      图像的尺寸(非必须)
  //          int mode              图像的格式(非必须)
  //返回值：
  //          返回边缘检测函数后的图像,图像格式和输入保持一致；
  //修改记录：
  //==================================================================
  void _maix_vision_Canny(int thr_h, int thr_l);

  //==================================================================
  //函数名：  _maix_vision_HoughCircles
  //作者：    dianjixz
  //日期：    2021-10-27
  //功能：    霍夫圆变换原理及圆检测
  //输入参数：
  //          py::object py_img     python输入图像对象
  //          int method            使用的检测方法，目前opencv只有霍夫梯度法一种方法可用，该参数填HOUGH_GRADIENT即可（opencv 4.1.0下）
  //          double dp             double类型的dp，用来检测圆心的累加器图像的分辨率于输入图像之比的倒数，且此参数允许创建一个比输入图像分辨率低的累加器。上述文字不好理解的话，来看例子吧。例如，如果dp= 1时，累加器和输入图像具有相同的分辨率。如果dp=2，累加器便有输入图像一半那么大的宽度和高度。
  //          double minDist        为霍夫变换检测到的圆的圆心之间的最小距离
  //          double param1         它是第三个参数method设置的检测方法的对应的参数。对当前唯一的方法霍夫梯度法CV_HOUGH_GRADIENT，它表示传递给canny边缘检测算子的高阈值，而低阈值为高阈值的一半。
  //          double param2         也是第三个参数method设置的检测方法的对应的参数，对当前唯一的方法霍夫梯度法HOUGH_GRADIENT，它表示在检测阶段圆心的累加器阈值。它越小的话，就可以检测到更多根本不存在的圆，而它越大的话，能通过检测的圆就更加接近完美的圆形了。
  //          int minRadius         表示圆半径的最小值
  //          int maxRadius         表示圆半径的最大值
  //          std::vector<int> size      图像的尺寸(非必须)
  //          int mode              图像的格式(非必须)
  //返回值：
  //          返回中值滤波后的图像,图像格式和输入保持一致；
  //修改记录：
  //==================================================================
  void _maix_vision_HoughCircles(int method, double dp, double minDist, double param1, double param2, int minRadius, int maxRadius, std::vector<int> size, int mode);

  //==================================================================
  //函数名：  _maix_vision_opencv_calcHist
  //作者：    dianjixz
  //日期：    2021-10-29
  //功能：    opencv计算图像直方图
  //输入参数：
  //          py::object py_img     python输入图像对象
  //          int channels          需要统计直方图的第几通道(默认为0)
  //          std::vector<int> &roi      掩膜，，计算掩膜内的直方图  感兴趣区域(默认全部区域)
  //          int histSize          直方图分成多少个区间，就是 bin的个数(默认256)
  //          std::vector<int> ranges     统计像素值得区间(默认(0,256))
  //          bool uniform          是否对得到的直方图数组进行归一化处理(默认true)
  //          bool accumulate       在多个图像时，是否累计计算像素值得个数(默认false)
  //          std::vector<int> size      图像的尺寸(非必须)(默认(0,0))
  //          int mode              图像的格式(非必须)(默认16)
  //返回值：
  //          返回中值滤波后的图像,图像格式和输入保持一致；
  //修改记录：
  //==================================================================
  py::object _maix_vision_opencv_calcHist(int channels, std::vector<int> &roi, int histSize, std::vector<int> ranges, bool uniform, bool accumulate, std::vector<int> size, int mode);

  //==================================================================
  //函数名：  get_blob_color_max
  //作者：    dianjixz
  //日期：    2021-8-01
  //功能：    获取框内颜色的最大值
  //输入参数：
  //          py::object py_img     python输入图像对象
  //          std::vector<int> roi       感兴趣区域最大阈值
  //          int critical          上下颜色范围宽度(简单扩大范围)
  //          int co                返回的颜色模式  0:rgb   1:Lab   2:HSV
  //          std::vector<int> size      图像的尺寸(非必须)
  //          int mode              图像的格式(非必须)
  //返回值：
  //          返回一个框内颜色的最大值的列表；
  //修改记录：
  //==================================================================
  py::list get_blob_color_max(std::vector<int> &roi, int critical, int co);

  //==================================================================
  //函数名：  _maix_vision_find_blob
  //作者：    dianjixz
  //日期：    2021-8-01
  //功能：    查找颜色区域
  //输入参数：
  //          ector<vector<int>> &thresholds      颜色的阈值
  //          std::vector<int> roi                     感兴趣区域最大阈值
  //          int x_stride                        x方向的最小像素
  //          int y_stride                        y方向的最小像素
  //          bool invert                         是否反转图像
  //          int area_threshold                  最小面积阈值
  //          int pixels_threshold                最小像素阈值
  //          bool merge                          是否合并相邻的像素块
  //          int margin                          合并的最小距离
  //          int tilt                            寻找包裹最小的矩形
  //          int co                              传入的颜色阈值模式  0:rgb   1:Lab   2:HSV    3:L
  //          std::vector<int> size                    图像的尺寸(非必须)
  //          int mode                            图像的格式(非必须)
  //返回值：
  //          返回一个查找到颜色的列表
  //修改记录：
  //==================================================================
  py::list _maix_vision_find_blob(std::vector<std::vector<int>> &thresholds, std::vector<int> roi, int x_stride, int y_stride, bool invert, int area_threshold, int pixels_threshold, bool merge, int margin, int tilt, int co);

  //==================================================================
  //函数名：  _maix_vision_find_ball_blob
  //作者：    dianjixz
  //日期：    2021-8-01
  //功能：    查找符合颜色的小球
  //输入参数：
  //          py::object py_img                   python输入图像对象
  //          ector<vector<int>> &thresholds      颜色的阈值
  //          int co                              传入的颜色阈值模式  0:rgb   1:Lab   2:HSV
  //          std::vector<int> size                    图像的尺寸(非必须)
  //          int mode                            图像的格式(非必须)
  //返回值：
  //          返回一个查找到小球的列表
  //修改记录：
  //==================================================================
  py::list _maix_vision_find_ball_blob(std::vector<int> &thresholds, int co);

  py::dict find_line();

  //==================================================================
  //函数名：  _maix_vision_get_histogram
  //作者：    dianjixz
  //日期：    2021-10-29
  //功能：    在 roi 的所有颜色通道上进行标准化直方图运算，并返回 histogram 对象
  //输入参数：
  //          py::object py_img                        python输入图像对象
  //          vector<vector<int>> &thresholds          thresholds 必须是元组列表。 [(lo, hi), (lo, hi), ..., (lo, hi)] 定义你想追踪的颜色范围,对于灰度图像，每个元组需要包含两个值 - 最小灰度值和最大灰度值。
  //          bool invert                              反转阈值操作，像素在已知颜色范围之外进行匹配，而非在已知颜色范围内。
  //          std::vector<int> &roi                         是感兴趣区域的矩形元组(x，y，w，h)。如果未指定，ROI即整个图像的图像矩形。操作范围仅限于 roi 区域内的像素。
  //          int bins                                 和其他bin是用于直方图通道的箱数。对于灰度图像，使用 bins
  //          std::vector<int> size      图像的尺寸(非必须)(默认(0,0))
  //          int mode              图像的格式(非必须)(默认16)
  //返回值：
  //          返回中值滤波后的图像,图像格式和输入保持一致；
  //修改记录：
  //==================================================================
  py::object _maix_vision_get_histogram(std::vector<std::vector<int>> &thresholds, bool invert, std::vector<int> &roi, int bins, std::vector<int> size, int mode);
};

//最终类的继承,采用虚继承方案进行继承
class maix_image : virtual public any_image, public maix_version
{
private:
public:
  // libmaix_image_t *img;
  // int _maix_image_width;
  // int _maix_image_height;
  // std::string _maix_image_type;
  // int _maix_image_size;
  maix_image();
  ~maix_image();
  void v_init();
  void v_close();

  std::string str__();
  int len__();

  void _show();
  int _save(std::string file_path, std::string format);
  py::bytes _tobytes();
  maix_image &_new(std::vector<int> size, std::vector<int> color, std::string mode);
  maix_image &_load(py::object data, std::vector<int> size, std::string mode);
  maix_image &_open_file(std::string path);
  py::object _get_to(std::string im);
  maix_image &_clear();

  // maix_image &_load_freetype();
  maix_image &_resize(int w, int h);
  maix_image &_draw_line(int x1, int y1, int x2, int y2, std::vector<int> color, int thickness);
  maix_image &_draw_rectangle(int x, int y, int w, int h, std::vector<int> color, int thickness);
  maix_image &_draw_circle(int x, int y, int r, std::vector<int> color, int thickness);
  maix_image &_draw_string(int x, int y, std::string str, std::vector<int> color, double scale, int thickness);
  maix_image &_draw_ellipse(int cx, int cy, int rx, int ry, double angle, double startAngle, double endAngle, std::vector<int> color, int thickness);
  maix_image &_rotate(double rotate);
  maix_image &_convert(std::string mode);
  maix_image &_draw_crop(int x, int y, int w, int h);
  std::vector<int> _get_pixe(int x, int y);
};

#endif