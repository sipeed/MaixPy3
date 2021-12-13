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
#define enable_PIL
#define enable_numpy

#define debug_line printf("%s:%d %s %s %s \r\n", __FILE__, __LINE__, __FUNCTION__, __DATE__, __TIME__)

namespace py = pybind11;
typedef enum
{
  image_none = -1,
  py_input_maix_image = 0,
  py_input_bytes,
  py_input_PIL,
  py_input_opencv,
  py_input_numpy,
} py_input_image_mode_t;
class any_image
{
private:
public:
  Any py_to_pram[4][4];
  libmaix_image_t *_img;
  int _maix_image_width;
  int _maix_image_height;
  std::string _maix_image_type;
  int _maix_image_size;
  any_image();
  py_input_image_mode_t get_img_type(py::object &input_img);
  int get_to(std::string &mode);
};

int Distance(int x1, int y1, int x2, int y2);

class maix_custom : virtual public any_image
{
private:
public:
  maix_custom();
  py::list _find_ball_blob(std::vector<std::vector<int>> &thresholds, std::vector<int> roi, int x_stride, int y_stride, bool invert, int area_threshold, int pixels_threshold, bool merge, int margin, int tilt, int h_min, int w_min, int co);
};

class maix_version : virtual public any_image
{
private:
public:
  maix_version();
  py::list get_blob_color_max(std::vector<int> &roi, int critical, int co);
  py::list _maix_vision_find_blob(std::vector<std::vector<int>> &thresholds, std::vector<int> roi, int x_stride, int y_stride, bool invert, int area_threshold, int pixels_threshold, bool merge, int margin, int tilt, int co);
  py::list _maix_vision_find_ball_blob(std::vector<int> &thresholds, int co);
  py::dict find_line(int func);
  void version_test();
};

class maix_image : virtual public any_image, public maix_version, public maix_custom
{
private:
public:
  maix_image();
  ~maix_image();
  void v_init();
  void v_close();
  void local_load(libmaix_image_t *img);
  std::string repr__();
  std::string str__();
  int len__();
  void _show();
  int _save(std::string file_path, std::string format);
  py::bytes _tobytes();
  size_t img_pointer();
  maix_image &_new(std::vector<int> size, std::vector<int> color, std::string mode);
  maix_image &_load(py::object data, std::vector<int> size, std::string mode);
  maix_image &_open_file(std::string path);
  py::object _to_py(std::string im);
  void _clear();
  void _load_freetype(std::string path);
  py::tuple _get_string_size(std::string str, double scale, int thickness);
  maix_image &_resize(int w, int h, int func);
  maix_image &_draw_line(int x1, int y1, int x2, int y2, std::vector<int> color, int thickness);
  maix_image &_draw_rectangle(int x, int y, int w, int h, std::vector<int> color, int thickness);
  maix_image &_draw_circle(int x, int y, int r, std::vector<int> color, int thickness);
  maix_image &_draw_string(int x, int y, std::string str, double scale, std::vector<int> color, int thickness);
  maix_image &_draw_ellipse(int cx, int cy, int rx, int ry, double angle, double startAngle, double endAngle, std::vector<int> color, int thickness);
  maix_image &_rotate(double rotate, int adjust);
  maix_image &_convert(std::string mode);
  maix_image *_draw_crop(int x, int y, int w, int h);
  maix_image &_draw_image(py::object data, int x, int y, double alpha);
  std::vector<int> _get_pixel(int x, int y);
  maix_image &_set_pixel(int x, int y, std::vector<int> color);
};
#endif