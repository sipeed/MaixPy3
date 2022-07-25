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
#include <opencv2/opencv.hpp>
#include "libmaix_cv_image.h"
#include "libmaix_disp.h"

#define enable_PIL
#define enable_numpy

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

#include "imlib.h"

class maix_vision : virtual public any_image
{
private:
public:
  static struct imlib_env
  {
    imlib_env()
    {
      // init
      // printf("imlib_env init\r\n");
      imlib_init_all();
    }
    ~imlib_env()
    {
      // exit
      // printf("imlib_env exit\r\n");
      imlib_deinit_all();
    }
  } imlib;
  maix_vision();
  py::list get_blob_color_max(std::vector<int> &roi, int critical, int co);
  py::list _maix_vision_find_blob(std::vector<std::vector<int>> &thresholds, std::vector<int> roi, int x_stride, int y_stride, bool invert, int area_threshold, int pixels_threshold, bool merge, int margin, int tilt, int co);
  py::list _maix_vision_find_ball_blob(std::vector<int> &thresholds, int co);
  py::dict find_line(int func);
  void version_test();
  py::list _imlib_find_rects(std::vector<int> &roi, uint32_t threshold, int is_xywh);
  py::list _imlib_find_lines(std::vector<int> &roi, unsigned int x_stride, unsigned int y_stride, uint32_t threshold, unsigned int theta_margin, unsigned int rho_margin);
  py::list _imlib_find_circles(std::vector<int> &roi, unsigned int x_stride, unsigned int y_stride, uint32_t threshold, unsigned int x_margin, unsigned int y_margin, unsigned int r_margin, unsigned int r_min, unsigned int r_max, unsigned int r_step);
  py::list _imlib_find_line_segments(std::vector<int> &roi, unsigned int merge_distance, unsigned int max_theta_diff);
  py::list _imlib_find_apriltags(std::vector<int> &roi, int families, float fx, float fy, float cx, float cy);
  py::list _imlib_find_qrcodes(std::vector<int> &roi);
  py::list _imlib_find_barcodes(std::vector<int> &roi);
  py::list _imlib_find_blobs(std::vector<std::vector<int>> &thresholds_src, std::vector<int> roi_src, int x_stride, int y_stride, bool invert, int area_threshold, int pixels_threshold, bool merge, int margin, int x_hist_bins_max, int y_hist_bins_max);
};

class maix_image : virtual public any_image, public maix_vision, public maix_custom
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
  py::bytes _tobytes(std::string format, std::vector<int> params);
  size_t img_pointer();
  maix_image &_new(std::vector<int> size, std::vector<int> color, std::string mode);
  maix_image &_load(py::object data, std::vector<int> size, std::string mode);
  maix_image &_open_file(std::string path);
  py::object _to_py(std::string im);
  void _delete();
  maix_image &_clear();
  void _load_freetype(std::string path);
  py::tuple _get_string_size(std::string str, double scale, int thickness);
  maix_image &_resize(int w, int h, int func, int padding, std::vector<int> size);
  maix_image &_draw_line(int x1, int y1, int x2, int y2, std::vector<int> color, int thickness);
  maix_image &_draw_cross(int x, int y, int c, int size, int thickness);
  maix_image &_draw_rectangle(int x1_x, int y1_y, int x2_w, int y2_h, std::vector<int> color, int thickness, int is_xywh);
  maix_image &_draw_circle(int x, int y, int r, std::vector<int> color, int thickness);
  maix_image &_draw_string(int x, int y, std::string str, double scale, std::vector<int> color, int thickness);
  maix_image &_draw_ellipse(int cx, int cy, int rx, int ry, double angle, double startAngle, double endAngle, std::vector<int> color, int thickness);
  maix_image &_rotate(double rotate, int adjust);
  maix_image &_flip(int flip);
  maix_image &_convert(std::string mode);
  maix_image *_draw_crop(int x, int y, int w, int h);
  maix_image *_warp_affine(std::vector<int> points, int w, int h);
  maix_image &_draw_image(py::object data, int x, int y, double alpha);
  std::vector<int> _get_pixel(int x, int y);
  maix_image &_set_pixel(int x, int y, std::vector<int> color);

  // https://docs.openmv.io/library/omv.image.html#image.image.histogram
  struct maix_histogram
  {
    std::vector<float> lab_bins[3];
    PIXFORMAT_STRUCT;
    maix_histogram() { ; }
    maix_histogram(histogram_t &hist)
    {
      lab_bins[0].resize(hist.LBinCount);
      for (int i = 0; i < hist.LBinCount; i++)
        lab_bins[0][i] = hist.LBins[i];
      lab_bins[1].resize(hist.ABinCount);
      for (int i = 0; i < hist.ABinCount; i++)
        lab_bins[1][i] = hist.ABins[i];
      lab_bins[2].resize(hist.BBinCount);
      for (int i = 0; i < hist.BBinCount; i++)
        lab_bins[2][i] = hist.BBins[i];
    }

    // std::vector<float> &operator[](size_t pos) {
    //   return lab_bins[pos];
    // };

    std::vector<float> bins()
    {
      return lab_bins[0];
    }

    std::vector<float> l_bins()
    {
      return lab_bins[0];
    }

    std::vector<float> a_bins()
    {
      return lab_bins[1];
    }

    std::vector<float> b_bins()
    {
      return lab_bins[2];
    }

    void get_percentile()
    {
    }

    void get_threshold()
    {
    }

    void get_statistics()
    {
    }
  };

  maix_histogram _imlib_get_histogram(std::vector<int> roi_src, std::vector<std::vector<int>> &thresholds_src, bool invert, maix_image &other_src, int bins, int l_bins, int a_bins, int b_bins);

  py::list _imlib_get_statistics(std::vector<int> roi_src, std::vector<std::vector<int>> &thresholds_src, bool invert, maix_image &other_src, int bins, int l_bins, int a_bins, int b_bins);

  py::dict _imlib_find_template(maix_image &template_src, float arg_thresh, std::vector<int> roi_src, int step, int search);

  // void imlib_rotation_corr(image_t *img, float x_rotation, float y_rotation,
  //                  float z_rotation, float x_translation, float y_translation,
  //                  float zoom, float fov, float *corners);
  // https://docs.openmv.io/library/omv.image.html?highlight=rotation_corr#image.image.Image.img.rotation_corr
  maix_image &_imlib_rotation_corr(float x_rotation, float y_rotation, float z_rotation, float x_translation, float y_translation, float zoom, float fov, std::vector<std::vector<float>> corners);

  maix_image &_hist_eq(bool adaptive, float clip_limit, maix_image &mask);

  maix_image &_gamma_corr(float gamma, float contrast, float brightness);

  maix_image &_lens_corr(float strength, float zoom, float x_corr, float y_corr);

  maix_image &_mean(const int ksize, bool threshold, int offset, bool invert, maix_image &mask);

  maix_image &_opencv_Canny(double threshold1, double threshold2, int apertureSize, bool L2gradient);
};
#endif