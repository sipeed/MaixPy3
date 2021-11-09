
#include <iostream>

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
#include "any.hpp"
// using namespace cv;
using namespace std;

namespace py = pybind11;
#define heigh_t 10
#define debug_line printf("%s:%d %s %s %s \r\n", __FILE__, __LINE__, __FUNCTION__, __DATE__, __TIME__)





class _maix_image
{

private:
  libmaix_image_t *img;
    int _maix_image_img_num;
  any maix_pram1[4];
  any maix_pram2[4];
  any opencv_parm1[4];


  int get_to(string &mode)
  {
    if (mode == "L")
    {
      return 0;
    }
    else if (mode == "RGB16")
    {
      return 1;
    }
    else if (mode == "RGB")
    {
      return 2;
    }
    else if (mode == "RGBA")
    {
      return 3;
    }
    else
    {
      PyErr_SetString(PyExc_RuntimeError, "libmaix_disp_create err!");
      throw py::error_already_set();
      return -1;
    }
  }

public:
  std::string _maix_image_format;
  std::string _maix_image_mode;
  std::vector<int> _maix_image_size;
  py::dict info;

  py::bytes test(py::bytes &rgb)
  {
    //   puts("test _maix_image");
    return py::none();
  }

  _maix_image &_new(vector<int> size, vector<int> color, string image_mode)
  {
    this->~_maix_image();
    this->_maix_image_img_num = this->get_to(image_mode);
    this->img = libmaix_image_create(size[0], size[1], any_cast<libmaix_image_mode_t>(maix_pram1[this->_maix_image_img_num]), LIBMAIX_IMAGE_LAYOUT_HWC, NULL, true);
    libmaix_cv_image_draw_rectangle(this->img, 0, 0, size[0], size[1], MaixColor(color[0], color[1], color[2]), -1);
    this->_maix_image_mode = image_mode;
    this->_maix_image_size[0] = this->img->width;
    this->_maix_image_size[1] = this->img->height;
    this->_maix_image_format = image_mode;
    return *this;
  }

  void _show()
  {
    if(NULL == this->img)
    {
      return;
    }
      struct libmaix_disp *disp = libmaix_disp_create(0);
      if(disp)
      {
        libmaix_image_t *rgb888 = libmaix_image_create(disp->width, disp->height, LIBMAIX_IMAGE_MODE_RGB888, LIBMAIX_IMAGE_LAYOUT_HWC, NULL, true);
        libmaix_cv_image_draw_rectangle(rgb888, 0, 0, 240, 240, MaixColor(0, 0, 0), -1);
        libmaix_cv_image_draw_image(this->img, 0, 0, rgb888);

        disp->draw_image(disp, rgb888);
        libmaix_image_destroy(&rgb888);
        
      }
      libmaix_disp_destroy(&disp);
  }


  _maix_image &load(py::object data, vector<int> size, string image_mode)
  {
    if (py::isinstance<py::bytes>(data))
    {
      this->~_maix_image();
      this->_maix_image_img_num = this->get_to(image_mode);
      this->img = libmaix_image_create(size[0], size[1], any_cast<libmaix_image_mode_t>(maix_pram1[this->_maix_image_img_num]), LIBMAIX_IMAGE_LAYOUT_HWC, NULL, true);
      if (this->img)
      {
        string tmp = data.cast<string>();
        memcpy(this->img->data, tmp.c_str(), this->img->width * this->img->height * any_cast<int>(maix_pram2[this->_maix_image_img_num]));
      }
      this->_maix_image_mode = image_mode;
      this->_maix_image_size[0] = this->img->width;
      this->_maix_image_size[1] = this->img->height;
      this->_maix_image_format = image_mode;

      return *this;

      /*
      if (image_mode == "RGB")
      {
        this->img = libmaix_image_create(size[0], size[1], LIBMAIX_IMAGE_MODE_RGB888, LIBMAIX_IMAGE_LAYOUT_HWC, NULL, true);
        if (this->img)
        {
          string tmp = data.cast<string>();
          memcpy(this->img->data, tmp.c_str(), this->img->width * this->img->height * 3);
        }
        this->mode = "RGB";
        this->size[0] = this->img->width;
        this->size[1] = this->img->height;
        this->format = "RGB";

        return *this;
      }
      else if (image_mode == "RGBA")
      {
        this->img = libmaix_image_create(size[0], size[1], LIBMAIX_IMAGE_MODE_RGBA8888, LIBMAIX_IMAGE_LAYOUT_HWC, NULL, true);
        if (this->img)
        {
          string tmp = data.cast<string>();
          memcpy(this->img->data, tmp.c_str(), this->img->width * this->img->height * 4);
        }
        this->mode = "RGBA";
        this->size[0] = this->img->width;
        this->size[1] = this->img->height;
        this->format = "RGBA";
        return *this;
      }
      else if (image_mode == "RGB16")
      {
        this->img = libmaix_image_create(size[0], size[1], LIBMAIX_IMAGE_MODE_RGB565, LIBMAIX_IMAGE_LAYOUT_HWC, NULL, true);
        if (this->img)
        {
          string tmp = data.cast<string>();
          memcpy(this->img->data, tmp.c_str(), this->img->width * this->img->height * 2);
        }
        this->mode = "RGB16";
        this->size[0] = this->img->width;
        this->size[1] = this->img->height;
        this->format = "RGB16";
        return *this;
      }
      else if (image_mode == "L")
      {
        if (size[0] == 0 || size[1] == 0)
        {
          size[0] = 240;
          size[1] = 240;
        }
        this->img = libmaix_image_create(size[0], size[1], LIBMAIX_IMAGE_MODE_GRAY, LIBMAIX_IMAGE_LAYOUT_HWC, NULL, true);
        if (this->img)
        {
          string tmp = data.cast<string>();
          memcpy(this->img->data, tmp.c_str(), this->img->width * this->img->height);
        }
        this->mode = "L";
        this->size[0] = this->img->width;
        this->size[1] = this->img->height;
        this->format = "L";
        return *this;
      }
      */
    }
    return *this;
  }
  int save(string path)
  {
    if (this->img)
    {
      libmaix_cv_image_draw_image_save(this->img, path.c_str());
    }
    return 0;
  }

  py::bytes tobytes()
  {
    if (this->img)
    {

      return py::bytes((const char *)this->img->data, this->img->width * this->img->height * any_cast<int>(maix_pram2[this->_maix_image_img_num]));

      /*
      switch (this->img->mode)
      {
      case LIBMAIX_IMAGE_MODE_RGB888:
        return py::bytes((const char *)this->img->data, this->img->width * this->img->height * 3);
        break;
      case LIBMAIX_IMAGE_MODE_RGBA8888:
        return py::bytes((const char *)this->img->data, this->img->width * this->img->height * 4);
        break;
      case LIBMAIX_IMAGE_MODE_GRAY:
        return py::bytes((const char *)this->img->data, this->img->width * this->img->height * 1);
        break;
      case LIBMAIX_IMAGE_MODE_RGB565:
        return py::bytes((const char *)this->img->data, this->img->width * this->img->height * 2);
        break;
      default:
        break;
      }
      */
    }
    return py::none();
  }

  _maix_image &resize(int w, int h)
  {
    if (this->img)
    {
      libmaix_image_t *tmp = libmaix_image_create(w, h, this->img->mode, LIBMAIX_IMAGE_LAYOUT_HWC, NULL, true);
      if (tmp)
      {

        cv::Mat src(this->img->width, this->img->height, any_cast<int>(opencv_parm1[this->_maix_image_img_num]), this->img->data);
        cv::Mat dst(w, h, CV_8UC3, tmp->data);
        cv::resize(src, dst, cv::Size(w, h));
        libmaix_image_destroy(&this->img), this->img = tmp;
        this->_maix_image_size[0] = w;
        this->_maix_image_size[1] = h;

        /*
        switch (int(this->img->mode))
        {
        case (3): //LIBMAIX_IMAGE_MODE_RGB888
        {
          cv::Mat src(this->img->width, this->img->height, CV_8UC3, this->img->data);
          cv::Mat dst(w, h, CV_8UC3, tmp->data);
          cv::resize(src, dst, cv::Size(w, h));
          libmaix_image_destroy(&this->img), this->img = tmp;
        }

        break;
        case (4): //LIBMAIX_IMAGE_MODE_RGB565
        {
          cv::Mat src(this->img->width, this->img->height, CV_8UC2, this->img->data);
          cv::Mat dst(w, h, CV_8UC2, tmp->data);
          cv::resize(src, dst, cv::Size(w, h));
          libmaix_image_destroy(&this->img), this->img = tmp;
        }

        break;
        case (5): //LIBMAIX_IMAGE_MODE_RGBA8888
        {
          cv::Mat src(this->img->width, this->img->height, CV_8UC4, this->img->data);
          cv::Mat dst(w, h, CV_8UC4, tmp->data);
          cv::resize(src, dst, cv::Size(w, h));
          libmaix_image_destroy(&this->img), this->img = tmp;
        }

        break;
        case (2): //LIBMAIX_IMAGE_MODE_GRAY
        {
          cv::Mat src(this->img->width, this->img->height, CV_8UC1, this->img->data);
          cv::Mat dst(w, h, CV_8UC1, tmp->data);
          cv::resize(src, dst, cv::Size(w, h));
          libmaix_image_destroy(&this->img), this->img = tmp;
        }

        break;
        default:
          break;
        }

        */
      }
    }
    return *this;
  }

  _maix_image &rotate(double rotate)
  {
    if (this->img)
    {
      libmaix_image_t *tmp = libmaix_image_create(this->img->width, this->img->height, this->img->mode, LIBMAIX_IMAGE_LAYOUT_HWC, NULL, true);
      libmaix_cv_image_rotate(this->img, rotate, &tmp);
      libmaix_image_destroy(&this->img), this->img = tmp;
    }
    return *this;
  }

  _maix_image &draw_crop(vector<int> thr)
  {

    if (this->img)
    {
      libmaix_image_t *tmp = libmaix_image_create(thr[2], thr[3], this->img->mode, LIBMAIX_IMAGE_LAYOUT_HWC, NULL, true);
      libmaix_cv_image_crop(this->img, thr[0], thr[1], thr[2], thr[3], &tmp);
      libmaix_image_destroy(&this->img), this->img = tmp;
      this->_maix_image_size[0] = thr[2];
      this->_maix_image_size[1] = thr[3];
    }
    return *this;
  }

  _maix_image &draw_convert(string mode)
  {
    if (this->img)
    {
      int mode_tmp = this->get_to(mode);
      libmaix_image_t *tmp = libmaix_image_create(this->img->width, this->img->height, any_cast<libmaix_image_mode_t>(maix_pram1[mode_tmp]), LIBMAIX_IMAGE_LAYOUT_HWC, NULL, true);
      libmaix_cv_image_convert(this->img, any_cast<libmaix_image_mode_t>(maix_pram1[mode_tmp]), &tmp);
      libmaix_image_destroy(&this->img), this->img = tmp;
      this->_maix_image_mode = mode;

      /*
      if (mode == "RGB")
      {
        libmaix_image_t *tmp = libmaix_image_create(this->img->width, this->img->height, LIBMAIX_IMAGE_MODE_RGB888, LIBMAIX_IMAGE_LAYOUT_HWC, NULL, true);
        libmaix_cv_image_convert(this->img, LIBMAIX_IMAGE_MODE_RGB888, &tmp);
        libmaix_image_destroy(&this->img), this->img = tmp;
      }
      else if (mode == "RGBA")
      {
        libmaix_image_t *tmp = libmaix_image_create(this->img->width, this->img->height, LIBMAIX_IMAGE_MODE_RGBA8888, LIBMAIX_IMAGE_LAYOUT_HWC, NULL, true);
        libmaix_cv_image_convert(this->img, LIBMAIX_IMAGE_MODE_RGBA8888, &tmp);
        libmaix_image_destroy(&this->img), this->img = tmp;
      }
      else if (mode == "RGB16")
      {
        libmaix_image_t *tmp = libmaix_image_create(this->img->width, this->img->height, LIBMAIX_IMAGE_MODE_RGB565, LIBMAIX_IMAGE_LAYOUT_HWC, NULL, true);
        libmaix_cv_image_convert(this->img, LIBMAIX_IMAGE_MODE_RGB565, &tmp);
        libmaix_image_destroy(&this->img), this->img = tmp;
      }
      else if (mode == "L")
      {
        libmaix_image_t *tmp = libmaix_image_create(this->img->width, this->img->height, LIBMAIX_IMAGE_MODE_GRAY, LIBMAIX_IMAGE_LAYOUT_HWC, NULL, true);
        libmaix_cv_image_convert(this->img, LIBMAIX_IMAGE_MODE_GRAY, &tmp);
        libmaix_image_destroy(&this->img), this->img = tmp;
      }
      */
    }
    return *this;
  }

  _maix_image &draw_ellipse(vector<int> &rect, double angle, double startAngle, double endAngle, vector<int> color, int thickness)
  {
    if (this->img)
    {
      libmaix_cv_image_draw_ellipse(this->img, rect[0], rect[1], rect[2], rect[3], angle, startAngle, endAngle, MaixColor(color[0], color[1], color[2]), thickness);
    }
    return *this;
  }
  _maix_image &draw_string(int x, int y, const char *str, double scale, vector<int> color, int thickness)
  {
    if (this->img)
    {
      libmaix_cv_image_draw_string(this->img, x, y, str, MaixColor(color[0], color[1], color[2]), scale, thickness);
    }
    return *this;
  }

  _maix_image &draw_circle(vector<int> &circ, vector<int> &color, int thickness)
  {
    if (this->img)
    {
      libmaix_cv_image_draw_circle(this->img, circ[0], circ[1], circ[2], MaixColor(color[0], color[1], color[2]), thickness);
    }
    return *this;
  }

  _maix_image &draw_rectangle(vector<int> &rect, vector<int> color, int thickness)
  {
    if (img)
    {
      libmaix_cv_image_draw_rectangle(img, rect[0], rect[1], rect[2], rect[3], MaixColor(color[0], color[1], color[2]), thickness);
    }
    return *this;
  }

  _maix_image &draw_line(vector<int> &lin, vector<int> color, int thickness)
  {
    if (img)
    {
      libmaix_cv_image_draw_line(this->img, lin[0], lin[1], lin[2], lin[3], MaixColor(color[0], color[1], color[2]), thickness);
    }
    return *this;
  }

  // _maix_image &load_freetype(string &path)
  // {

  //     libmaix_cv_image_load_freetype(const char *path);

  // }

  _maix_image()
  {
    img = NULL;
    maix_pram1[0] = LIBMAIX_IMAGE_MODE_GRAY;
    maix_pram2[0] = 1;
    opencv_parm1[0] = CV_8UC1;

    maix_pram1[1] = LIBMAIX_IMAGE_MODE_RGB565;
    maix_pram2[1] = 2;
    opencv_parm1[1] = CV_8UC2;

    maix_pram1[2] = LIBMAIX_IMAGE_MODE_RGB888;
    maix_pram2[2] = 3;
    opencv_parm1[2] = CV_8UC3;

    maix_pram1[3] = LIBMAIX_IMAGE_MODE_RGBA8888;
    maix_pram2[3] = 4;
    opencv_parm1[3] = CV_8UC4;

    this->_maix_image_size.push_back(0);
    this->_maix_image_size.push_back(0);
  }
  ~_maix_image()
  {
    if (img)
    {
      libmaix_image_destroy(&img);
    }
  }
};







_maix_image Image_new(vector<int> size, vector<int> color, string image_mode)
{




}

















class _maix_vision_histogram : public py::list
{
private:
  /* data */
public:
  _maix_vision_histogram()
  {
  }
  _maix_vision_histogram(py::list hist)
  {
    this->append(hist);
  }
  ~_maix_vision_histogram()
  {
  }
  py::object bins()
  {
  }
  py::object l_bins()
  {
  }
  py::object a_bins()
  {
  }
  py::object b_bins()
  {
  }
  py::object get_percentile(float percentile)
  {
  }
  py::object get_threshold()
  {
  }
  py::object get_statistics()
  {
  }
};

class _maix_vision
{

private:
  /* data */
  py::object PIL_;
  py::object py_img_tobytes_;
  //==================================================================
  //函数名：  Distance
  //作者：    dianjixz
  //日期：    2021-10-01
  //功能：    计算两个二维空间坐标点的欧式距离
  //输入参数：
  //          (x1,y1) 坐标一
  //          (x2,y2) 坐标二
  //返回值：  int
  //          两个坐标点的欧式距离
  //修改记录：
  //==================================================================
  int Distance(int x1, int y1, int x2, int y2)
  {
    int x = abs(x1 - x2);
    int y = abs(y1 - y2);
    return int(round(sqrt(x * x + y * y)));
  }
  //==================================================================
  //函数名：  out_img_to_py_img
  //作者：    dianjixz
  //日期：    2021-10-27
  //功能：    将opencv的Mat图像转换成输出图像,和输入图像保持一致
  //输入参数：  py_img  _out
  //          py_img (py::object）输入图像
  //          _out    (cv::Mat） 将要转换的图像
  //返回值：  类型（py::object)
  //          返回转换好的图像
  //修改记录：
  //==================================================================
  py::object out_img_to_py_img(py::object &py_img, cv::Mat &_out)
  {
    if (py::isinstance<py::bytes>(py_img))
    {
      py::bytes tmp((const char *)_out.data, _out.rows * _out.cols * _out.channels());
      return tmp;
    }
    else
    {
      auto _PIL_ = py::module::import("PIL.Image");
      if (py::isinstance(py_img, this->PIL_))
      {
        // auto tobytes = PIL_.attr("tobytes");
        // auto img_bytes = tobytes(py_img);
        auto frombytes = _PIL_.attr("frombytes");
        // py::bytes tmp;
        py::bytes tmp((const char *)_out.data, _out.rows * _out.cols * _out.channels());
        if (_out.channels() == 3)
        {
          py::str str_tmp = "RGB";
          py::tuple sizek = py::make_tuple(_out.rows, _out.cols);
          auto PIL_img = frombytes(str_tmp, sizek, tmp);
          return PIL_img;
        }
        else if (_out.channels() == 4)
        {
          py::str str_tmp = "RGBA";
          py::tuple sizek = py::make_tuple(_out.rows, _out.cols);
          auto PIL_img = frombytes(str_tmp, sizek, tmp);
          return PIL_img;
        }
        else if (_out.channels() == 1)
        {
          py::str str_tmp = "L";
          py::tuple sizek = py::make_tuple(_out.rows, _out.cols);
          auto PIL_img = frombytes(str_tmp, sizek, tmp);
          return PIL_img;
        }
      }
    }
    return py::none();
  }
  //==================================================================
  //函数名：  py_img_to_in_img
  //作者：    dianjixz
  //日期：    2021-10-01
  //功能：    将函数的输入图像参数转换成opencv的Mat图像
  //输入参数：
  //          py_img  (py::object） 传递的python对象
  //          _out    (cv::Mat）    转换好输出的图像
  //          size    (vector<int>) 图像的尺寸(非必须)
  //          mode    (int)         图像的格式(非必须)
  //返回值：
  //          转换成功 0
  //          转换失败 -1
  //修改记录：
  //==================================================================
  int py_img_to_in_img(py::object &py_img, cv::Mat &_out, vector<int> &size, int mode)
  {
    if (py::isinstance<py::bytes>(py_img))
    {
      string tmp = py_img.cast<string>();
      if (size[0] == 0 || size[1] == 0)
      {
        // cv::Mat input(240, 240, CV_8UC3, const_cast<char *>(tmp.c_str()));
        cv::Mat input(240, 240, mode, const_cast<char *>(tmp.c_str()));
        input.copyTo(_out);
      }
      else
      {
        cv::Mat input(size[0], size[1], mode, const_cast<char *>(tmp.c_str()));
        // cv::Mat input(size[0], size[1], CV_8UC3, const_cast<char *>(tmp.c_str()));
        input.copyTo(_out);
      }
    }
    else
    {
      // auto PIL_ = py::module::import("PIL.Image").attr("Image");
      if (py::isinstance(py_img, this->PIL_))
      {
        // auto tobytes = PIL_.attr("tobytes");
        // auto img_bytes = tobytes(py_img);
        auto img_bytes = this->py_img_tobytes_(py_img);
        auto tmp = img_bytes.cast<string>();
        auto img_size = py_img.attr("size").cast<vector<int>>();
        auto img_mode = py_img.attr("mode").cast<string>();
        if (img_mode == "RGB")
        {
          // cv::Mat input(img_size[0], img_size[1], CV_8UC3, const_cast<char *>(tmp.c_str()));
          cv::Mat input(img_size[0], img_size[1], CV_8UC3, const_cast<char *>(tmp.c_str()));
          input.copyTo(_out);
        }
        else if (img_mode == "RGBA")
        {
          cv::Mat input(img_size[0], img_size[1], CV_8UC4, const_cast<char *>(tmp.c_str()));
          input.copyTo(_out);
        }
        else if (img_mode == "L" || img_mode == "1")
        {
          cv::Mat input(img_size[0], img_size[1], CV_8UC1, const_cast<char *>(tmp.c_str()));
          input.copyTo(_out);
        }
      }
    }
    return 0;
  }

public:
  int COLOR_RGB;
  int COLOR_RGBA;
  int COLOR_L;
  py::bytes test(py::bytes &rgb)
  {
    std::string tmp = static_cast<std::string>(rgb);
    cv::Mat input(240, 240, CV_8UC3, const_cast<char *>(tmp.c_str()));

    cv::Mat output = cv::Mat::zeros(240, 240, CV_8UC4);

    cv::Mat gray, edges;
    // Mat standard_hough, probabilistic_hough;
    int min_threshold = 50;
    int max_trackbar = 150;
    int s_trackbar = max_trackbar;
    // int p_trackbar = max_trackbar;

    cvtColor(input, gray, cv::COLOR_RGB2GRAY);

    cv::Canny(gray, edges, 50, 200, 3);

    cvtColor(edges, output, cv::COLOR_GRAY2BGRA);

    vector<cv::Vec2f> s_lines;
    // cvtColor(edges, standard_hough, COLOR_GRAY2BGR);

    /// 1. Use Standard Hough Transform
    cv::HoughLines(edges, s_lines, 1, CV_PI / 180, min_threshold + s_trackbar, 0, 0);

    /// Show the result
    for (size_t i = 0; i < s_lines.size(); i++)
    {
      float r = s_lines[i][0], t = s_lines[i][1];
      double cos_t = cos(t), sin_t = sin(t);
      double x0 = r * cos_t, y0 = r * sin_t;
      double alpha = 1000;

      cv::Point pt1(cvRound(x0 + alpha * (-sin_t)), cvRound(y0 + alpha * cos_t));
      cv::Point pt2(cvRound(x0 - alpha * (-sin_t)), cvRound(y0 - alpha * cos_t));
      cv::line(output, pt1, pt2, cv::Scalar(255, 0, 0, 200), 3, cv::LINE_AA);
    }

    // vector<Vec4i> p_lines;
    // cvtColor(edges, probabilistic_hough, COLOR_GRAY2BGR);

    // /// 2. Use Probabilistic Hough Transform
    // HoughLinesP(edges, p_lines, 1, CV_PI / 180, min_threshold + p_trackbar, 30, 10);

    // /// Show the result
    // for (size_t i = 0; i < p_lines.size(); i++)
    // {
    //   Vec4i l = p_lines[i];
    //   line(output, Point(l[0], l[1]), Point(l[2], l[3]), Scalar(255, 0, 0, 200), 3, LINE_AA);
    // }

    int size = output.total() * output.elemSize();
    return py::bytes((char *)output.data, size);
    // std::vector buff;
    // cv::imencode(".bmp", image, buff);
    // std::string image_string(reinterpret_cast<char*>(&buff[0]), buff.size());
    // return py::bytes();
  }

  _maix_vision()
  {
    this->PIL_ = py::module::import("PIL.Image").attr("Image");
    this->py_img_tobytes_ = this->PIL_.attr("tobytes");
    this->COLOR_RGB = CV_8UC3;
    this->COLOR_RGBA = CV_8UC4;
    this->COLOR_L = CV_8UC1;
  }

  ~_maix_vision()
  {
  }

  py::bytes opencv_test(py::bytes &rgb)
  {
    std::string tmp = static_cast<std::string>(rgb);
    cv::Mat input(240, 240, CV_8UC3, const_cast<char *>(tmp.c_str()));
    int size = input.total() * input.elemSize();
    return py::bytes((char *)input.data, size);
  }

  //==================================================================
  //函数名：  _maix_vision_medianBlur
  //作者：    dianjixz
  //日期：    2021-10-27
  //功能：    图像中值滤波函数
  //输入参数：
  //          py::object py_img     python输入图像对象
  //          int m_size            中值滤波的核大小
  //          vector<int> size      图像的尺寸(非必须)
  //          int mode              图像的格式(非必须)
  //返回值：
  //          返回中值滤波后的图像,图像格式和输入保持一致；
  //修改记录：
  //==================================================================
  py::object _maix_vision_medianBlur(py::object py_img, int m_size, vector<int> size, int mode)
  {
    cv::Mat in_img;
    py_img_to_in_img(py_img, in_img, size, mode); //获取图像
    cv::Mat dist;
    medianBlur(in_img, dist, m_size);
    py::object tmp = out_img_to_py_img(py_img, dist);
    return tmp;
  }
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
  //          vector<int> size                图像的尺寸(非必须)
  //          int mode                        图像的格式(非必须)
  //返回值：
  //          返回高斯滤波后的图像,图像格式和输入保持一致；
  //修改记录：
  //==================================================================
  py::object _maix_vision_GaussianBlur(py::object py_img, std::vector<int> ksize, double sigmaX, double sigmaY, int borderType, std::vector<int> size, int mode)
  {
    cv::Mat in_img;
    py_img_to_in_img(py_img, in_img, size, mode); //获取图像
    cv::Mat dist;
    cv::GaussianBlur(in_img, dist, cv::Size(ksize[0], ksize[1]), sigmaX, sigmaY);
    py::object tmp = out_img_to_py_img(py_img, dist);
    return tmp;
  }
  //==================================================================
  //函数名：  _maix_vision_Canny
  //作者：    dianjixz
  //日期：    2021-10-27
  //功能：    边缘检测函数
  //输入参数：
  //          py::object py_img     python输入图像对象
  //          int thr_h             最大阈值
  //          int thr_l             最小阈值
  //          vector<int> size      图像的尺寸(非必须)
  //          int mode              图像的格式(非必须)
  //返回值：
  //          返回边缘检测函数后的图像,图像格式和输入保持一致；
  //修改记录：
  //==================================================================
  py::object _maix_vision_Canny(py::object py_img, int thr_h, int thr_l, vector<int> size, int mode)
  {
    cv::Mat in_img;
    py_img_to_in_img(py_img, in_img, size, mode); //获取图像
    cv::Mat dist;
    cv::Canny(in_img, dist, thr_h, thr_l);
    py::object tmp = out_img_to_py_img(py_img, dist);
    return tmp;
  }
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
  //          vector<int> size      图像的尺寸(非必须)
  //          int mode              图像的格式(非必须)
  //返回值：
  //          返回中值滤波后的图像,图像格式和输入保持一致；
  //修改记录：
  //==================================================================
  py::object _maix_vision_HoughCircles(py::object py_img, int method, double dp, double minDist, double param1, double param2, int minRadius, int maxRadius, vector<int> size, int mode)
  {
    cv::Mat in_img;
    this->py_img_to_in_img(py_img, in_img, size, mode); //获取图像
    cv::Mat dist;
    vector<cv::Vec3f> circles;
    cv::HoughCircles(in_img, circles, method, dp, minDist, param1, param2, minRadius, maxRadius);
    py::list return_val;
    for (size_t i = 0; i < circles.size(); i++)
    {
      py::list tmp;
      tmp.append(circles[i][0]);
      tmp.append(circles[i][1]);
      tmp.append(circles[i][2]);
      return_val.append(tmp);
    }
    return return_val;
  }
  //==================================================================
  //函数名：  _maix_vision_opencv_calcHist
  //作者：    dianjixz
  //日期：    2021-10-29
  //功能：    opencv计算图像直方图
  //输入参数：
  //          py::object py_img     python输入图像对象
  //          int channels          需要统计直方图的第几通道(默认为0)
  //          vector<int> &roi      掩膜，，计算掩膜内的直方图  感兴趣区域(默认全部区域)
  //          int histSize          直方图分成多少个区间，就是 bin的个数(默认256)
  //          vector<int> ranges     统计像素值得区间(默认(0,256))
  //          bool uniform          是否对得到的直方图数组进行归一化处理(默认true)
  //          bool accumulate       在多个图像时，是否累计计算像素值得个数(默认false)
  //          vector<int> size      图像的尺寸(非必须)(默认(0,0))
  //          int mode              图像的格式(非必须)(默认16)
  //返回值：
  //          返回中值滤波后的图像,图像格式和输入保持一致；
  //修改记录：
  //==================================================================
  py::object _maix_vision_opencv_calcHist(py::object py_img, int channels, vector<int> &roi, int histSize, vector<int> ranges, bool uniform, bool accumulate, vector<int> size, int mode)
  {
    py::list return_val;
    cv::Mat in_img;
    this->py_img_to_in_img(py_img, in_img, size, mode); //获取图像
    cv::Rect rect;
    if (roi[2] != 0 && roi[3] != 0)
    {
      rect.x = roi[0];
      rect.y = roi[1];
      rect.width = roi[2];
      rect.height = roi[3];
    }
    else
    {
      rect.x = 0;
      rect.y = 0;
      rect.width = in_img.size[0];
      rect.height = in_img.size[1];
    }
    cv::Mat mask = cv::Mat::zeros(in_img.size(), CV_8UC1);
    mask(rect).setTo(255);
    float range[] = {ranges[0], ranges[1]};
    const float *histRanges = {range};
    cv::Mat _hist;
    calcHist(&in_img, 1, (const int *)&channels, mask, _hist, 1, (const int *)&histSize, &histRanges, uniform, accumulate);
    for (int i = 0; i < histSize; i++)
    {
      return_val.append(_hist.at<float>(i));
    }
    return return_val;
  }

  //==================================================================
  //函数名：  get_blob_color_max
  //作者：    dianjixz
  //日期：    2021-8-01
  //功能：    获取框内颜色的最大值
  //输入参数：
  //          py::object py_img     python输入图像对象
  //          vector<int> roi       感兴趣区域最大阈值
  //          int critical          上下颜色范围宽度(简单扩大范围)
  //          int co                返回的颜色模式  0:rgb   1:Lab   2:HSV
  //          vector<int> size      图像的尺寸(非必须)
  //          int mode              图像的格式(非必须)
  //返回值：
  //          返回一个框内颜色的最大值的列表；
  //修改记录：
  //==================================================================
  py::list get_blob_color_max(py::object py_img, vector<int> &roi, int critical, int co, vector<int> size, int mode)
  {
    py::list return_val;
    cv::Mat in_img;
    this->py_img_to_in_img(py_img, in_img, size, mode); //获取图像

    critical = critical > 100 ? 100 : critical;
    critical = critical < 0 ? 0 : critical;

    cv::Rect rect;
    rect.x = roi[0];
    rect.y = roi[1];
    rect.width = roi[2];
    rect.height = roi[3];
    cv::Mat lab_img;
    lab_img = in_img(rect);
    vector<cv::Mat> lab_planes;
    split(lab_img, lab_planes);

    int histSize = 256;
    float range[] = {0, 256};
    const float *histRanges = range;
    cv::Mat l_hist, a_hist, b_hist;
    calcHist(&lab_planes[0], 1, 0, cv::Mat(), l_hist, 1, &histSize, &histRanges, true, false);
    calcHist(&lab_planes[1], 1, 0, cv::Mat(), a_hist, 1, &histSize, &histRanges, true, false);
    calcHist(&lab_planes[2], 1, 0, cv::Mat(), b_hist, 1, &histSize, &histRanges, true, false);

    float lmax = 0, lnum = 0;
    float amax = 0, anum = 0;
    float bmax = 0, bnum = 0;
    for (int i = 0; i < histSize; i++)
    {
      if (l_hist.at<float>(i) > lmax)
      {
        lmax = l_hist.at<float>(i);
        lnum = i;
      }
      if (a_hist.at<float>(i) > amax)
      {
        amax = a_hist.at<float>(i);
        anum = i;
      }
      if (b_hist.at<float>(i) > bmax)
      {
        bmax = b_hist.at<float>(i);
        bnum = i;
      }
    }
    switch (co)
    {
    case 0: //rgb
    {
      return_val.append(lnum);
      return_val.append(anum);
      return_val.append(bnum);
      // return_val.append(int(max_lnum * 100 / 255));
      // return_val.append(max_anum - 128);
      // return_val.append(max_bnum - 128);
      return return_val;
    }
    break;
    case 1: //lab
    {

      cv::Mat rgb(1, 1, CV_8UC3, cv::Scalar(lnum, anum, bnum));
      cv::Mat lab;
      cvtColor(rgb, lab, cv::COLOR_RGB2Lab);
      lnum = lab.at<cv::Vec3b>(0, 0)[0];
      anum = lab.at<cv::Vec3b>(0, 0)[1];
      bnum = lab.at<cv::Vec3b>(0, 0)[2];

      int min_lnum = int(lnum - critical);
      min_lnum = min_lnum < 0 ? 0 : min_lnum;
      int max_lnum = int(lnum + critical);
      max_lnum = max_lnum > 180 ? 180 : max_lnum;
      int min_anum = int(anum - critical);
      min_anum = min_anum < 0 ? 0 : min_anum;
      int max_anum = int(anum + critical);
      max_anum = max_anum > 255 ? 255 : max_anum;
      int min_bnum = int(bnum - critical);
      min_bnum = min_bnum < 0 ? 0 : min_bnum;
      int max_bnum = int(bnum + critical);
      max_bnum = max_bnum > 255 ? 255 : max_bnum;
      return_val.append(int(min_lnum * 100 / 255));
      return_val.append(min_anum - 128);
      return_val.append(min_bnum - 128);
      return_val.append(int(max_lnum * 100 / 255));
      return_val.append(max_anum - 128);
      return_val.append(max_bnum - 128);
      return return_val;
    }
    break;
    case 2: //hsv
    {

      cv::Mat rgb(1, 1, CV_8UC3, cv::Scalar(lnum, anum, bnum));
      cv::Mat lab;
      cvtColor(rgb, lab, cv::COLOR_RGB2HSV);
      lnum = lab.at<cv::Vec3b>(0, 0)[0];
      anum = lab.at<cv::Vec3b>(0, 0)[1];
      bnum = lab.at<cv::Vec3b>(0, 0)[2];

      int min_lnum = int(lnum - critical);
      min_lnum = min_lnum < 0 ? 0 : min_lnum;
      int max_lnum = int(lnum + critical);
      max_lnum = max_lnum > 180 ? 180 : max_lnum;
      int min_anum = int(anum - critical);
      min_anum = min_anum < 0 ? 0 : min_anum;
      int max_anum = int(anum + critical);
      max_anum = max_anum > 255 ? 255 : max_anum;
      int min_bnum = int(bnum - critical);
      min_bnum = min_bnum < 0 ? 0 : min_bnum;
      int max_bnum = int(bnum + critical);
      max_bnum = max_bnum > 255 ? 255 : max_bnum;
      return_val.append(min_lnum);
      return_val.append(min_anum);
      return_val.append(min_bnum);
      return_val.append(max_lnum);
      return_val.append(max_anum);
      return_val.append(max_bnum);
      return return_val;
    }
    break;
    default:

      break;
    }
    return return_val;
  }
  //==================================================================
  //函数名：  _maix_vision_find_blob
  //作者：    dianjixz
  //日期：    2021-8-01
  //功能：    查找颜色区域
  //输入参数：
  //          py::object py_img                   python输入图像对象
  //          ector<vector<int>> &thresholds      颜色的阈值
  //          vector<int> roi                     感兴趣区域最大阈值
  //          int x_stride                        x方向的最小像素
  //          int y_stride                        y方向的最小像素
  //          bool invert                         是否反转图像
  //          int area_threshold                  最小面积阈值
  //          int pixels_threshold                最小像素阈值
  //          bool merge                          是否合并相邻的像素块
  //          int margin                          合并的最小距离
  //          int tilt                            寻找包裹最小的矩形
  //          int co                              传入的颜色阈值模式  0:rgb   1:Lab   2:HSV    3:L
  //          vector<int> size                    图像的尺寸(非必须)
  //          int mode                            图像的格式(非必须)
  //返回值：
  //          返回一个查找到颜色的列表
  //修改记录：
  //==================================================================
  py::list _maix_vision_find_blob(py::object py_img, vector<vector<int>> &thresholds, vector<int> roi, int x_stride, int y_stride, bool invert, int area_threshold, int pixels_threshold, bool merge, int margin, int tilt, int co, vector<int> size, int mode)
  {
    py::list return_val;
    cv::Mat in_img;
    py_img_to_in_img(py_img, in_img, size, mode); //获取输入图像
    cv::Mat lab, mask1;
    bool grasy = 0;
    switch (co) //转换颜色空间以及颜色阈值
    {
    case 0: //rgb
      if (in_img.channels() != 3)
        return return_val;
      if (roi[2] != 0 && roi[3] != 0)
      {
        cv::Rect rect(roi[0], roi[1], roi[2], roi[3]);
        lab = in_img(rect);
      }
      else
      {
        lab = in_img;
      }
      break;
    case 1: //lab
      if (in_img.channels() != 3)
        return return_val;
      if (roi[2] != 0 && roi[3] != 0)
      {
        cv::Rect rect(roi[0], roi[1], roi[2], roi[3]);
        cvtColor(in_img(rect), lab, cv::COLOR_RGB2Lab);
      }
      else
      {
        cvtColor(in_img, lab, cv::COLOR_RGB2Lab);
      }
      for (size_t i = 0; i < thresholds.size(); i++)
      {
        thresholds[i][0] = int((thresholds[i][0] * 255) / 100);
        thresholds[i][1] = thresholds[i][1] + 128;
        thresholds[i][2] = thresholds[i][2] + 128;
        thresholds[i][3] = int((thresholds[i][3] * 255) / 100);
        thresholds[i][4] = thresholds[i][4] + 128;
        thresholds[i][5] = thresholds[i][5] + 128;
      }
      break;
    case 2: //hsv
      if (in_img.channels() != 3)
        return return_val;
      if (roi[2] != 0 && roi[3] != 0)
      {
        cv::Rect rect(roi[0], roi[1], roi[2], roi[3]);
        cvtColor(in_img(rect), lab, cv::COLOR_RGB2HSV);
      }
      else
      {
        cvtColor(in_img, lab, cv::COLOR_RGB2HSV);
      }
      for (size_t i = 0; i < thresholds.size(); i++)
      {
        thresholds[i][0] = int(thresholds[i][0] / 2);
        thresholds[i][1] = int(thresholds[i][1] * 2.55);
        thresholds[i][2] = int(thresholds[i][2] * 2.55);
        thresholds[i][3] = int(thresholds[i][3] / 2);
        thresholds[i][4] = int(thresholds[i][4] * 2.55);
        thresholds[i][5] = int(thresholds[i][5] * 2.55);
      }
      break;
    case 3: //灰度
      if (in_img.channels() != 1)
        return return_val;
      if (roi[2] != 0 && roi[3] != 0)
      {
        cv::Rect rect(roi[0], roi[1], roi[2], roi[3]);
        lab = in_img(rect);
      }
      else
      {
        lab = in_img;
      }
      grasy = 1;
      break;
    default: //不在颜色阈值内,返回
      return return_val;
      break;
    }
    cv::Mat mask = cv::Mat::zeros(lab.size(), CV_8UC1);
    if (grasy)
    {
      for (size_t i = 0; i < thresholds.size(); i++)
      {
        cv::inRange(lab, cv::Scalar(thresholds[i][0]), cv::Scalar(thresholds[i][1]), mask1); //分割通道,阈值化
        mask = mask + mask1;
      }
    }
    else
    {
      for (size_t i = 0; i < thresholds.size(); i++)
      {
        inRange(lab, cv::Scalar(thresholds[i][0], thresholds[i][1], thresholds[i][2]), cv::Scalar(thresholds[i][3], thresholds[i][4], thresholds[i][5]), mask1); //分割通道,阈值化
        mask = mask + mask1;
      }
    }
    if (invert) //是否反转图像
    {
      bitwise_not(mask, mask);
    }

    cv::Mat se = getStructuringElement(cv::MORPH_RECT, cv::Size(x_stride, y_stride), cv::Point(-1, -1)); //开运算,去除噪点
    morphologyEx(mask, mask, cv::MORPH_OPEN, se);
    if (margin != 0)
    {
      cv::Mat se_t = getStructuringElement(cv::MORPH_RECT, cv::Size(margin, margin), cv::Point(-1, -1)); //闭运算,链接相邻比较近的色块
      morphologyEx(mask, mask, cv::MORPH_CLOSE, se_t);
    }

    vector<vector<cv::Point>> contours;
    vector<cv::Vec4i> hiearchy;
    findContours(mask, contours, hiearchy, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_NONE); //查找最小矩形轮廓
    if (contours.size() == 0)
    {
      return return_val;
    }
    for (size_t i = 0; i < contours.size(); i++)
    {
      py::dict val;
      cv::Rect rects = boundingRect(contours[i]);
      val["x"] = int(rects.x);
      val["y"] = int(rects.y);
      val["w"] = int(rects.width);
      val["h"] = int(rects.height);
      if (int(rects.width) * int(rects.width) < area_threshold)
      {
        continue;
      }
      if (int(contourArea(contours[i])) < pixels_threshold)
      {
        continue;
      }
      val["pixels"] = int(contourArea(contours[i]));

      val["cx"] = int(rects.x + rects.width / 2);
      val["cy"] = int(rects.y + rects.height / 2);

      if (tilt) //是否需要最小包裹矩形轮廓
      {
        cv::RotatedRect minRect = minAreaRect(contours[i]);
        cv::Point2f rect_points[4];
        minRect.points(rect_points);
        py::tuple tmp3 = py::make_tuple(rect_points[0].x, rect_points[0].y, rect_points[1].x, rect_points[1].y, rect_points[2].x, rect_points[2].y, rect_points[3].x, rect_points[3].y);
        val["tilt_Rect"] = tmp3;
        int tmp1 = Distance(int(rect_points[0].x), int(rect_points[0].y), int(rect_points[1].x), int(rect_points[1].y));
        int tmp2 = Distance(int(rect_points[0].x), int(rect_points[0].y), int(rect_points[3].x), int(rect_points[3].y));
        float x1, y1, k;
        if (tmp1 > tmp2)
        {
          x1 = rect_points[1].x - rect_points[0].x;
          y1 = rect_points[1].y - rect_points[0].y;
          k = atan(y1 / x1);
        }
        else
        {
          x1 = rect_points[3].x - rect_points[0].x;
          y1 = rect_points[3].y - rect_points[0].y;
          k = atan(y1 / x1);
        }
        val["rotation"] = k;
      }
      return_val.append(val);
    }
    return return_val;
  }
  //==================================================================
  //函数名：  _maix_vision_find_ball_blob
  //作者：    dianjixz
  //日期：    2021-8-01
  //功能：    查找符合颜色的小球
  //输入参数：
  //          py::object py_img                   python输入图像对象
  //          ector<vector<int>> &thresholds      颜色的阈值
  //          int co                              传入的颜色阈值模式  0:rgb   1:Lab   2:HSV
  //          vector<int> size                    图像的尺寸(非必须)
  //          int mode                            图像的格式(非必须)
  //返回值：
  //          返回一个查找到小球的列表
  //修改记录：
  //==================================================================
  py::list _maix_vision_find_ball_blob(py::object py_img, vector<int> &thresholds, int co, vector<int> size, int mode)
  {
    cv::Mat in_img;
    py::list out;
    this->py_img_to_in_img(py_img, in_img, size, mode);
    cv::Mat hsv, mask;
    switch (co) //转换颜色空间以及颜色阈值
    {
    case 0: //rgb
      if (in_img.channels() != 3)
        return out;
      hsv = in_img;
      break;
    case 1: //lab
      if (in_img.channels() != 3)
        return out;
      cv::cvtColor(in_img, hsv, cv::COLOR_RGB2Lab);
      thresholds[0] = int((thresholds[0] * 255) / 100);
      thresholds[1] = thresholds[1] + 128;
      thresholds[2] = thresholds[2] + 128;
      thresholds[3] = int((thresholds[3] * 255) / 100);
      thresholds[4] = thresholds[4] + 128;
      thresholds[5] = thresholds[5] + 128;
      break;
    case 2: //hsv
      if (in_img.channels() != 3)
        return out;
      cv::cvtColor(in_img, hsv, cv::COLOR_RGB2HSV);
      thresholds[0] = int(thresholds[0] / 2);
      thresholds[1] = int(thresholds[1] * 2.55);
      thresholds[2] = int(thresholds[2] * 2.55);
      thresholds[3] = int(thresholds[3] / 2);
      thresholds[4] = int(thresholds[4] * 2.55);
      thresholds[5] = int(thresholds[5] * 2.55);
      break;
    default: //不在颜色阈值内,返回
      return out;
      break;
    }
    cv::inRange(hsv, cv::Scalar(thresholds[0], thresholds[1], thresholds[2]), cv::Scalar(thresholds[3], thresholds[4], thresholds[5]), mask);
    // cout << hsv_da <<endl;
    cv::Mat se = getStructuringElement(cv::MORPH_RECT, cv::Size(5, 5), cv::Point(-1, -1));
    morphologyEx(mask, mask, cv::MORPH_OPEN, se);
    vector<vector<cv::Point>> contours;
    vector<cv::Vec4i> hiearchy;
    findContours(mask, contours, hiearchy, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_NONE);
    for (size_t i = 0; i < contours.size(); i++)
    {
      /* 当拟合点数少于6个时，不进行拟合 */
      if (contours[i].size() < 6)
      {
        break;
      }
      // 圆拟合
      cv::RotatedRect rrt = fitEllipse(contours[i]);
      int cr_x, cr_y, cr_w, cr_h;
      cr_x = rrt.center.x;
      cr_y = rrt.center.y;
      cr_w = rrt.size.width;
      cr_h = rrt.size.height;
      /* 当图形长宽相差太大 或者 图形面积太小时，不进行处理 */
      if ((abs(cr_w - cr_h) > 10) ||
          (cr_w * cr_h) < 400)
      {
        break;
      }
      py::list tmp;
      tmp.append(cr_x);
      tmp.append(cr_y);
      tmp.append(cr_w);
      tmp.append(cr_h);
      out.append(tmp);
    }
    return std::move(out);
  }

  enum adaptiveMethod
  {
    meanFilter,
    gaaussianFilter,
    medianFilter
  };

  void AdaptiveThreshold(cv::Mat &src, cv::Mat &dst, double Maxval, int Subsize, double c, adaptiveMethod method = meanFilter)
  {

    if (src.channels() > 1)
      cv::cvtColor(src, src, cv::COLOR_RGB2GRAY);

    cv::Mat smooth;
    switch (method)
    {
    case meanFilter:
      cv::blur(src, smooth, cv::Size(Subsize, Subsize)); //均值滤波
      break;
    case gaaussianFilter:
      cv::GaussianBlur(src, smooth, cv::Size(Subsize, Subsize), 0, 0); //高斯滤波
      break;
    case medianFilter:
      cv::medianBlur(src, smooth, Subsize); //中值滤波
      break;
    default:
      break;
    }

    smooth = smooth - c;

    //阈值处理
    src.copyTo(dst);
    for (int r = 0; r < src.rows; ++r)
    {
      const uchar *srcptr = src.ptr<uchar>(r);
      const uchar *smoothptr = smooth.ptr<uchar>(r);
      uchar *dstptr = dst.ptr<uchar>(r);
      for (int c = 0; c < src.cols; ++c)
      {
        if (srcptr[c] > smoothptr[c])
        {
          dstptr[c] = Maxval;
        }
        else
          dstptr[c] = 0;
      }
    }
  }

  // A* c = new A(1);
  py::dict find_line(py::object py_img, vector<int> size, int mode)
  {
    cv::Mat src_gray, dst;
    py::dict return_val;
    cv::Mat in_img;
    py_img_to_in_img(py_img, in_img, size, mode);

    // cv::imwrite("/tmp/src.jpg", in_img);

    cv::Mat src_gary;
    cvtColor(in_img, src_gray, cv::COLOR_RGB2GRAY); //将图片变成灰度图

    // cv::imwrite("/tmp/src.jpg", src_gray);

    cv::Mat element = getStructuringElement(cv::MORPH_RECT, cv::Size(5, 5));

    cv::dilate(src_gray, src_gray, element); // 放大主线

    // cv::imwrite("/tmp/src_gray_0.jpg", src_gray);

    cv::erode(src_gray, src_gray, element); // 消除细线

    // cv::imwrite("/tmp/src_gray_1.jpg", src_gray);

    // AdaptiveThreshold(src_gray, src_gray, 255, block_size, offset, meanFilter); //

    AdaptiveThreshold(src_gray, src_gray, 255, 21, 10, meanFilter); //

    // cv::imwrite("/tmp/src_gray_2.jpg", src_gray);

    cv::dilate(src_gray, dst, element);

    // cv::imwrite("/tmp/dst.jpg", dst);

    cv::Rect rect;
    rect.x = 0;
    rect.y = 0;
    rect.width = dst.cols;
    rect.height = heigh_t;
    dst(rect).setTo(255);
    rect.x = 0;
    rect.y = dst.rows - heigh_t;
    rect.width = dst.cols;
    rect.height = heigh_t;
    dst(rect).setTo(255);
    rect.x = 0;
    rect.y = 0;
    rect.width = heigh_t;
    rect.height = dst.rows;
    dst(rect).setTo(255);
    rect.x = dst.cols - heigh_t;
    rect.y = 0;
    rect.width = heigh_t;
    rect.height = dst.rows;
    dst(rect).setTo(255);

    vector<vector<cv::Point>> contours;
    vector<cv::Vec4i> hierarchy;

    cv::findContours(dst, contours, hierarchy, cv::RETR_TREE, cv::CHAIN_APPROX_NONE, cv::Point());

    if (contours.size() == 0)
    {

      return std::move(return_val);
    }
    int area = 0, a_n = 0;
    for (size_t i = 1; i < contours.size(); i++)
    {
      int ar = contourArea(contours[i]);
      if (ar > area)
      {
        area = ar;
        a_n = i;
      }
    }
    cv::RotatedRect minRect = minAreaRect(contours[a_n]);
    cv::Point2f rect_points[4];
    minRect.points(rect_points);
    py::list tmps;
    tmps.append(int(rect_points[0].x));
    tmps.append(int(rect_points[0].y));
    tmps.append(int(rect_points[1].x));
    tmps.append(int(rect_points[1].y));
    tmps.append(int(rect_points[2].x));
    tmps.append(int(rect_points[2].y));
    tmps.append(int(rect_points[3].x));
    tmps.append(int(rect_points[3].y));
    return_val["rect"] = tmps;
    return_val["pixels"] = area;
    int cx, cy;
    cx = int((((rect_points[0].x - rect_points[1].x) + (rect_points[2].x - rect_points[1].x)) / 2) + rect_points[1].x);
    cy = int((((rect_points[0].y - rect_points[1].y) + (rect_points[2].y - rect_points[1].y)) / 2) + rect_points[1].y);
    return_val["cx"] = cx;
    return_val["cy"] = cy;
    int tmp1 = Distance(int(rect_points[0].x), int(rect_points[0].y), int(rect_points[1].x), int(rect_points[1].y));
    int tmp2 = Distance(int(rect_points[0].x), int(rect_points[0].y), int(rect_points[3].x), int(rect_points[3].y));
    float x1, y1, k;
    if (tmp1 > tmp2)
    {
      x1 = rect_points[1].x - rect_points[0].x;
      y1 = rect_points[1].y - rect_points[0].y;
      k = atan(y1 / x1);
    }
    else
    {
      x1 = rect_points[3].x - rect_points[0].x;
      y1 = rect_points[3].y - rect_points[0].y;
      k = atan(y1 / x1);
    }
    return_val["rotation"] = k;
    return std::move(return_val);
  }
  //==================================================================
  //函数名：  _maix_vision_get_histogram
  //作者：    dianjixz
  //日期：    2021-10-29
  //功能：    在 roi 的所有颜色通道上进行标准化直方图运算，并返回 histogram 对象
  //输入参数：
  //          py::object py_img                        python输入图像对象
  //          vector<vector<int>> &thresholds          thresholds 必须是元组列表。 [(lo, hi), (lo, hi), ..., (lo, hi)] 定义你想追踪的颜色范围,对于灰度图像，每个元组需要包含两个值 - 最小灰度值和最大灰度值。
  //          bool invert                              反转阈值操作，像素在已知颜色范围之外进行匹配，而非在已知颜色范围内。
  //          vector<int> &roi                         是感兴趣区域的矩形元组(x，y，w，h)。如果未指定，ROI即整个图像的图像矩形。操作范围仅限于 roi 区域内的像素。
  //          int bins                                 和其他bin是用于直方图通道的箱数。对于灰度图像，使用 bins
  //          vector<int> size      图像的尺寸(非必须)(默认(0,0))
  //          int mode              图像的格式(非必须)(默认16)
  //返回值：
  //          返回中值滤波后的图像,图像格式和输入保持一致；
  //修改记录：
  //==================================================================
  py::object _maix_vision_get_histogram(py::object py_img, vector<vector<int>> &thresholds, bool invert, vector<int> &roi, int bins, vector<int> size, int mode)
  {
    py::list return_val;
    cv::Mat in_img;
    this->py_img_to_in_img(py_img, in_img, size, mode); //获取图像
    cv::Rect rect;
    if (roi[2] != 0 && roi[3] != 0)
    {
      rect.x = roi[0];
      rect.y = roi[1];
      rect.width = roi[2];
      rect.height = roi[3];
    }
    else
    {
      rect.x = 0;
      rect.y = 0;
      rect.width = in_img.size[0];
      rect.height = in_img.size[1];
    }
    cv::Mat mask = cv::Mat::zeros(in_img.size(), CV_8UC1);
    mask(rect).setTo(255);
    // vector<float> ranges1;

    // float range[] = {ranges[0], ranges[1]};
    // const float *histRanges = {range};
    // cv::Mat _hist;
    // calcHist(&in_img, 1, (const int*)&channels, mask, _hist, 1, (const int*)&histSize, &histRanges, uniform, accumulate);
    // for(int i=0;i<histSize;i++)
    // {
    //   return_val.append(_hist.at<float>(i));
    // }
    // return return_val;
  }
};





PYBIND11_MODULE(_maix_opencv, m)
{
  pybind11::class_<_maix_vision>(m, "Vision")
      .def(pybind11::init<>())
      //图像参数
      .def_readonly("COLOR_RGB", &_maix_vision::COLOR_RGB)
      .def_readonly("COLOR_RGBA", &_maix_vision::COLOR_RGBA)
      .def_readonly("COLOR_L", &_maix_vision::COLOR_L)
      //opencv原生函数
      .def("opecv_medianBlur", &_maix_vision::_maix_vision_medianBlur, py::arg("py_img"), py::arg("m_size") = 5, py::arg("size") = std::vector<int>{240, 240}, py::arg("mode") = 16)
      .def("opecv_GaussianBlur", &_maix_vision::_maix_vision_GaussianBlur, py::arg("py_img"), py::arg("ksize"), py::arg("sigmaX"), py::arg("sigmaY"), py::arg("borderType"), py::arg("size") = std::vector<int>{240, 240}, py::arg("mode") = 16)
      .def("opecv_Canny", &_maix_vision::_maix_vision_Canny, py::arg("py_img"), py::arg("thr_h"), py::arg("thr_l"), py::arg("size") = std::vector<int>{240, 240}, py::arg("mode") = 16)
      .def("opecv_HoughCircles", &_maix_vision::_maix_vision_HoughCircles, py::arg("py_img"), py::arg("method") = 3, py::arg("dp"), py::arg("minDist"), py::arg("param1"), py::arg("param2"), py::arg("minRadius"), py::arg("maxRadius"), py::arg("size") = std::vector<int>{240, 240}, py::arg("mode") = 16)
      //基于opencv编写MaixPy3特有函数
      .def("get_blob_lab", &_maix_vision::get_blob_color_max, py::arg("py_img"), py::arg("roi") = std::vector<int>{0, 0, 0, 0}, py::arg("critical") = 0, py::arg("color") = 0, py::arg("size") = std::vector<int>{240, 240}, py::arg("mode") = 16)
      .def("get_blob_color", &_maix_vision::get_blob_color_max, py::arg("py_img"), py::arg("roi") = std::vector<int>{0, 0, 0, 0}, py::arg("critical") = 0, py::arg("color") = 0, py::arg("size") = std::vector<int>{240, 240}, py::arg("mode") = 16)
      .def("find_blob_lab", &_maix_vision::_maix_vision_find_blob, py::arg("py_img"), py::arg("thresholds"), py::arg("roi") = std::vector<int>{0, 0, 0, 0}, py::arg("x_stride") = 2, py::arg("y_stride") = 2, py::arg("invert") = 0, py::arg("area_threshold") = 10, py::arg("pixels_threshold") = 10, py::arg("merge") = 0, py::arg("margin") = 0, py::arg("tilt") = 0, py::arg("co") = 1, py::arg("size") = std::vector<int>{240, 240}, py::arg("mode") = 16)
      .def("find_ball_lab", &_maix_vision::_maix_vision_find_ball_blob, py::arg("py_img"), py::arg("thresholds"), py::arg("co") = 1, py::arg("size") = std::vector<int>{240, 240}, py::arg("mode") = 16)
      .def("find_circles_blob", &_maix_vision::_maix_vision_find_ball_blob, py::arg("py_img"), py::arg("thresholds"), py::arg("co") = 1, py::arg("size") = std::vector<int>{240, 240}, py::arg("mode") = 16)
      .def("find_line", &_maix_vision::find_line, py::arg("py_img"), py::arg("size") = std::vector<int>{240, 240}, py::arg("mode") = 16)
      //基于opencv编写兼容openmv图像处理函数
      .def("get_histogram", &_maix_vision::test)
      .def("get_percentile", &_maix_vision::test)
      .def("get_threshold", &_maix_vision::test)
      .def("get_statistics", &_maix_vision::test)
      .def("find_blobs", &_maix_vision::_maix_vision_find_blob, py::arg("py_img"), py::arg("thresholds"), py::arg("roi") = std::vector<int>{0, 0, 0, 0}, py::arg("x_stride") = 2, py::arg("y_stride") = 2, py::arg("invert") = 0, py::arg("area_threshold") = 10, py::arg("pixels_threshold") = 10, py::arg("merge") = 0, py::arg("margin") = 0, py::arg("tilt") = 0, py::arg("co") = 1, py::arg("size") = std::vector<int>{240, 240}, py::arg("mode") = 16)
      .def("find_lines", &_maix_vision::test)
      .def("find_line_segments", &_maix_vision::test)
      .def("get_regression", &_maix_vision::test)
      .def("find_circles", &_maix_vision::test)
      .def("find_rects", &_maix_vision::test);

  // pybind11::class_<_maix_image>(m, "Image")
  //     .def(pybind11::init<>())
  //     .def_readonly("format", &_maix_image::_maix_image_format)
  //     .def_readonly("size", &_maix_image::_maix_image_size)
  //     .def_readonly("mode", &_maix_image::_maix_image_mode)
  //     .def("new", &_maix_image::_new, py::arg("size") = std::vector<int>{240, 240}, py::arg("color") = std::vector<int>{0, 0, 0}, py::arg("mode") = "RGB")
  //     .def("show", &_maix_image::_show)
  //     .def("load", &_maix_image::load, py::arg("data"), py::arg("size") = std::vector<int>{240, 240}, py::arg("mode") = "RGB")
  //     .def("save", &_maix_image::save, py::arg("path"))
  //     .def("tobytes", &_maix_image::tobytes)
  //     .def("resize", &_maix_image::resize, py::arg("w"), py::arg("h"))
  //     .def("rotate", &_maix_image::rotate, py::arg("rotate"))
  //     .def("crop", &_maix_image::draw_crop, py::arg("thr"))
  //     .def("convert", &_maix_image::draw_convert, py::arg("mode") = "RGB")
  //     .def("draw_ellipse", &_maix_image::draw_ellipse, py::arg("rect"), py::arg("angle"), py::arg("startAngle"), py::arg("endAngle"), py::arg("color"), py::arg("thickness"))
  //     .def("draw_string", &_maix_image::draw_string, py::arg("x"), py::arg("y"), py::arg("str"), py::arg("scale") = 1.0, py::arg("color") = std::vector<int>{127, 127, 127}, py::arg("thickness") = 1)
  //     .def("draw_circle", &_maix_image::draw_circle, py::arg("circ"), py::arg("color") = std::vector<int>{127, 127, 127}, py::arg("thickness") = 1)
  //     .def("draw_rectangle", &_maix_image::draw_rectangle, py::arg("rect"), py::arg("color") = std::vector<int>{127, 127, 127}, py::arg("thickness") = 1)
  //     .def("draw_line", &_maix_image::draw_line, py::arg("line"), py::arg("color") = std::vector<int>{127, 127, 127}, py::arg("thickness") = 1)
  //     .def("load_freetype", &_maix_image::test);


}