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

#define debug_line // printf("%s:%d %s %s %s \r\n", __FILE__, __LINE__, __FUNCTION__, __DATE__, __TIME__)

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

class maix_version : virtual public any_image
{
private:
public:
  static struct imlib_env
  {
      imlib_env()
      {
          // init
          // printf("imlib_env init\r\n");
          // imlib_init_all();
          fb_alloc_init(2 * 1024 * 1024);
          fmath_init();
      }
      ~imlib_env()
      {
          // exit
          // printf("imlib_env exit\r\n");
          imlib_deinit_all();
      }
  } imlib;
  maix_version();
  py::list get_blob_color_max(std::vector<int> &roi, int critical, int co);
  py::list _maix_vision_find_blob(std::vector<std::vector<int>> &thresholds, std::vector<int> roi, int x_stride, int y_stride, bool invert, int area_threshold, int pixels_threshold, bool merge, int margin, int tilt, int co);
  py::list _maix_vision_find_ball_blob(std::vector<int> &thresholds, int co);
  py::dict find_line(int func);
  void version_test();
  py::list _imlib_find_rects(std::vector<int> &roi,uint32_t threshold,int is_xywh);
  py::list _imlib_find_lines(std::vector<int> &roi,unsigned int x_stride, unsigned int y_stride,uint32_t threshold, unsigned int theta_margin, unsigned int rho_margin);
  py::list _imlib_find_circles(std::vector<int> &roi,unsigned int x_stride, unsigned int y_stride,uint32_t threshold, unsigned int x_margin, unsigned int y_margin, unsigned int r_margin, unsigned int r_min, unsigned int r_max, unsigned int r_step);
  py::list _imlib_find_line_segments(std::vector<int> &roi, unsigned int merge_distance, unsigned int max_theta_diff);
  py::list _imlib_find_apriltags(std::vector<int> &roi, int families, float fx, float fy, float cx, float cy);
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
  void _delete();
  maix_image &_clear();
  void _load_freetype(std::string path);
  py::tuple _get_string_size(std::string str, double scale, int thickness);
  maix_image &_resize(int w, int h, int func);
  maix_image &_draw_line(int x1, int y1, int x2, int y2, std::vector<int> color, int thickness);
  maix_image &_draw_cross(int x, int y, int c, int size, int thickness);
  maix_image &_draw_rectangle(int x1_x, int y1_y, int x2_w, int y2_h, std::vector<int> color, int thickness, int is_xywh);
  maix_image &_draw_circle(int x, int y, int r, std::vector<int> color, int thickness);
  maix_image &_draw_string(int x, int y, std::string str, double scale, std::vector<int> color, int thickness);
  maix_image &_draw_ellipse(int cx, int cy, int rx, int ry, double angle, double startAngle, double endAngle, std::vector<int> color, int thickness);
  maix_image &_rotate(double rotate, int adjust);
  maix_image &_convert(std::string mode);
  maix_image *_draw_crop(int x, int y, int w, int h);
  maix_image &_draw_image(py::object data, int x, int y, double alpha);
  std::vector<int> _get_pixel(int x, int y);
  maix_image &_set_pixel(int x, int y, std::vector<int> color);

  // https://docs.openmv.io/library/omv.image.html#image.image.histogram
  struct maix_histogram : public py::object
  {
    std::vector<float> lab_bins[3];
    maix_histogram() { ; }
    maix_histogram(histogram_t &src) {

      // o->LBins = mp_obj_new_list(hist.LBinCount, NULL);
      // o->ABins = mp_obj_new_list(hist.ABinCount, NULL);
      // o->BBins = mp_obj_new_list(hist.BBinCount, NULL);

      // for (int i = 0; i < hist.LBinCount; i++) {
      //     ((mp_obj_list_t *) o->LBins)->items[i] = mp_obj_new_float(hist.LBins[i]);
      // }

      // for (int i = 0; i < hist.ABinCount; i++) {
      //     ((mp_obj_list_t *) o->ABins)->items[i] = mp_obj_new_float(hist.ABins[i]);
      // }

      // for (int i = 0; i < hist.BBinCount; i++) {
      //     ((mp_obj_list_t *) o->BBins)->items[i] = mp_obj_new_float(hist.BBins[i]);
      // }

    }

    std::vector<float> &operator[](size_t pos) {
      return lab_bins[pos];
    };

    std::vector<float> bins() {
      return lab_bins[0];
    }

    std::vector<float> l_bins() {
      return lab_bins[0];
    }

    std::vector<float> a_bins() {
      return lab_bins[1];
    }

    std::vector<float> b_bins() {
      return lab_bins[2];
    }

    void get_threshold() {

    }

    void get_statistics() {

    }

  };

  // void imlib_get_histogram(histogram_t *out, image_t *ptr, rectangle_t *roi, list_t *thresholds, bool invert, image_t *other);
  py::object _imlib_get_histogram(std::vector<int> roi_src, std::vector<std::vector<int>> &thresholds_src, bool invert, maix_image & other_src)
  {
    if (NULL == this->_img)
    {
      py::print("no img");
      return py::none();
    }

    // image_t *imlib_img = imlib_image_create(img->width, img->height, PIXFORMAT_RGB888, img->width * img->height * PIXFORMAT_BPP_RGB888, img->data, false);

    image_t img_tmp = {}, *arg_img = &img_tmp;
    arg_img->w = this->_img->width;
    arg_img->h = this->_img->height;
    arg_img->pixels = (uint8_t*)this->_img->data;
    arg_img->pixfmt = PIXFORMAT_RGB888;

    image_t other_img = {}, *other = NULL;
    if (NULL == other_src._img)
    {
        other->w = other_src._img->width;
        other->h = other_src._img->height;
        other->pixels = (uint8_t*)other_src._img->data;
        other->pixfmt = PIXFORMAT_RGB888;
        other = &other_img;
    }

    fb_alloc_mark();

    list_t thresholds;
    list_init(&thresholds, sizeof(color_thresholds_list_lnk_data_t));
    for (auto src : thresholds_src)
    {
      color_thresholds_list_lnk_data_t tmp_ct;
      tmp_ct.LMin = src[0];
      tmp_ct.LMax = src[1];
      tmp_ct.AMin = src[2];
      tmp_ct.AMax = src[3];
      tmp_ct.BMin = src[4];
      tmp_ct.BMax = src[5];
      list_push_back(&thresholds, &tmp_ct);
    }

    rectangle_t roi = { roi_src[0], roi_src[1], roi_src[2], roi_src[3] };
    histogram_t hist;
    switch (arg_img->pixfmt) {
        case PIXFORMAT_GRAYSCALE: {
          if (hist.LBinCount >= 2) {
            hist.ABinCount = 0;
            hist.BBinCount = 0;
            hist.LBins = (float *)fb_alloc(hist.LBinCount * sizeof(float), FB_ALLOC_NO_HINT);
            hist.ABins = NULL;
            hist.BBins = NULL;
            imlib_get_histogram(&hist, arg_img, &roi, &thresholds, invert, other);
            list_free(&thresholds);
          }
          break;
        }
        case PIXFORMAT_RGB888:
        case PIXFORMAT_RGB565: {
          if (hist.LBinCount >= 2 && hist.ABinCount >= 2 && hist.BBinCount >= 2) {
            hist.LBins = (float *)fb_alloc(hist.LBinCount * sizeof(float), FB_ALLOC_NO_HINT);
            hist.ABins = (float *)fb_alloc(hist.ABinCount * sizeof(float), FB_ALLOC_NO_HINT);
            hist.BBins = (float *)fb_alloc(hist.BBinCount * sizeof(float), FB_ALLOC_NO_HINT);
            imlib_get_histogram(&hist, arg_img, &roi, &thresholds, invert, other);
            list_free(&thresholds);
          }
          break;
        }
    }
    auto result = maix_histogram(hist);
    fb_alloc_free_till_mark();
    return result;
  }

  // void imlib_rotation_corr(image_t *img, float x_rotation, float y_rotation,
  //                  float z_rotation, float x_translation, float y_translation,
  //                  float zoom, float fov, float *corners);
  // https://docs.openmv.io/library/omv.image.html?highlight=rotation_corr#image.image.Image.img.rotation_corr
  maix_image &_imlib_rotation_corr(float x_rotation, float y_rotation, float z_rotation, float x_translation, float y_translation, float zoom, float fov, std::vector<std::vector<float>> corners)
  {
    if (NULL == this->_img)
    {
      py::print("no img");
      return *this;
    }

    // image_t *imlib_img = imlib_image_create(img->width, img->height, PIXFORMAT_RGB888, img->width * img->height * PIXFORMAT_BPP_RGB888, img->data, false);

    image_t img_tmp = {}, *img = &img_tmp;
    img->w = this->_img->width;
    img->h = this->_img->height;
    img->pixels = (uint8_t*)this->_img->data;
    img->pixfmt = PIXFORMAT_RGB888;

    fb_alloc_mark();

    imlib_rotation_corr(img, x_rotation, y_rotation, z_rotation, x_translation, y_translation, zoom, fov, NULL);

	  fb_alloc_free_till_mark();

    // imlib_image_destroy(&resize_img);

    return *this;
  }

  maix_image &_hist_eq(bool adaptive,float clip_limit,maix_image & mask);

  maix_image &_gamma_corr(float gamma,float contrast,float brightness);

  maix_image &_lens_corr(float strength, float zoom,float x_corr, float y_corr);

  maix_image &_mean(const int ksize, bool threshold, int offset,bool invert,maix_image & mask);
};
#endif