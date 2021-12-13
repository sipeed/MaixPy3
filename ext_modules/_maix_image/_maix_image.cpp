#include "maix_image.h"

any_image::any_image()
{
  this->py_to_pram[0][0] = LIBMAIX_IMAGE_MODE_GRAY;
  this->py_to_pram[0][1] = 1;
  this->py_to_pram[0][2] = CV_8UC1;
  this->py_to_pram[0][3] = "L";
  this->py_to_pram[1][0] = LIBMAIX_IMAGE_MODE_RGB565;
  this->py_to_pram[1][1] = 2;
  this->py_to_pram[1][2] = CV_8UC2;
  this->py_to_pram[1][3] = "RGB16";
  this->py_to_pram[2][0] = LIBMAIX_IMAGE_MODE_RGB888;
  this->py_to_pram[2][1] = 3;
  this->py_to_pram[2][2] = CV_8UC3;
  this->py_to_pram[2][3] = "RGB";
  this->py_to_pram[3][0] = LIBMAIX_IMAGE_MODE_RGBA8888;
  this->py_to_pram[3][1] = 4;
  this->py_to_pram[3][2] = CV_8UC4;
  this->py_to_pram[3][3] = "RGBA";
}

py_input_image_mode_t any_image::get_img_type(py::object &input_img)
{
  auto maix_image_type = py::module::import("_maix_image").attr("Image");
  if (py::isinstance(input_img, maix_image_type))
    return py_input_maix_image;
  if (py::isinstance<py::bytes>(input_img))
    return py_input_bytes;

#ifdef enable_numpy
  auto numpy_image_type = py::module::import("numpy").attr("ndarray");
  if (py::isinstance(input_img, numpy_image_type))
    return py_input_numpy;
#endif
#ifdef enable_PIL
  auto PIL_image_type = py::module::import("PIL.Image").attr("Image");
  if (py::isinstance(input_img, PIL_image_type))
    return py_input_PIL;
#endif
#ifdef enable_opencv
#endif
  return image_none;
}

int any_image::get_to(std::string &mode)
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

size_t maix_image::img_pointer()
{
  size_t pointer = (size_t)this->_img;
  return pointer;
}

void maix_image::v_init()
{
  this->_img = NULL;
  this->_maix_image_height = 0;
  this->_maix_image_size = 0;
  this->_maix_image_width = 0;
}

void maix_image::v_close()
{
  if (this->_img)
  {
    libmaix_image_destroy(&this->_img);
  }
  this->_maix_image_width = 0;
  this->_maix_image_height = 0;
  this->_maix_image_size = 0;
}

maix_image::maix_image()
{
  this->v_init();
}

maix_image::~maix_image()
{
  this->v_close();
}

std::string maix_image::repr__()
{
  return std::string();
  // std::string info_str;
  // std::stringstream ss;
  // ss << "<_maix_image.Image \"width\":" << this->_maix_image_width
  //    << ", \"height\":" << this->_maix_image_height
  //    << ", \"type\"=" << this->_maix_image_type
  //    << ", \"size\":" << this->_maix_image_size
  //    << ">";
  // info_str = ss.str();
  // return info_str;
}

std::string maix_image::str__()
{
  std::string info_str;
  std::stringstream ss;
  ss << "<_maix_image.Image \"width\":" << this->_maix_image_width
     << ", \"height\":" << this->_maix_image_height
     << ", \"type\"=" << this->_maix_image_type
     << ", \"size\":" << this->_maix_image_size
     << ">";
  info_str = ss.str();
  return info_str;
}

int maix_image::len__()
{
  return this->_maix_image_size;
}

static inline libmaix_image_t *libmaix_image_create_patch(libmaix_image_t *self, int w, int h, libmaix_image_mode_t m, libmaix_image_layout_t l, void *d, bool i)
{
  if ((self == NULL) || (self->width != w || self->height != h && self->mode != m))
  {
    libmaix_image_destroy(&self);
    return libmaix_image_create(w, h, m, l, d, i);
  }
  return self;
}

maix_image &maix_image::_new(std::vector<int> size, std::vector<int> color, std::string mode)
{
  this->_maix_image_type = mode;
  this->_maix_image_width = size[0];
  this->_maix_image_height = size[1];
  this->_maix_image_size = this->_maix_image_width * this->_maix_image_height * any_cast<int>(this->py_to_pram[this->get_to(this->_maix_image_type)][1]);
  this->_img = libmaix_image_create_patch(this->_img, size[0], size[1], any_cast<libmaix_image_mode_t>(this->py_to_pram[this->get_to(this->_maix_image_type)][0]), LIBMAIX_IMAGE_LAYOUT_HWC, NULL, true);
  if (this->_img)
  {
    libmaix_cv_image_draw_rectangle(this->_img, 0, 0, size[0], size[1], MaixColor(color[0], color[1], color[2]), -1);
  }
  else
  {
    this->v_close();
  }
  return *this;
}

maix_image &maix_image::_load(py::object data, std::vector<int> size, std::string mode)
{
  switch (this->get_img_type(data))
  {
  case py_input_bytes:
  {
    this->_maix_image_type = mode;
    this->_maix_image_width = size[0];
    this->_maix_image_height = size[1];
    this->_img = libmaix_image_create_patch(this->_img, this->_maix_image_width, this->_maix_image_height, any_cast<libmaix_image_mode_t>(py_to_pram[this->get_to(this->_maix_image_type)][0]), LIBMAIX_IMAGE_LAYOUT_HWC, NULL, true);
    this->_maix_image_size = this->_maix_image_width * this->_maix_image_height * any_cast<int>(py_to_pram[this->get_to(this->_maix_image_type)][1]);
    if (this->_img)
    {
      std::string tmp = data.cast<std::string>();
      memcpy(this->_img->data, tmp.c_str(), this->_maix_image_size);
    }
    else
    {
      this->v_close();
    }
    return *this;
  }
  break;
  case py_input_maix_image:
  {
    auto c_img_p = data.attr("c_img_private");
    size_t im_p = c_img_p().cast<size_t>();
    libmaix_image_t *im = (libmaix_image_t *)im_p;
    this->_maix_image_width = data.attr("width").cast<int>();
    this->_maix_image_height = data.attr("height").cast<int>();
    this->_maix_image_size = data.attr("size").cast<int>();
    this->_maix_image_type = data.attr("mode").cast<std::string>();
    this->_img = libmaix_image_create_patch(this->_img, this->_maix_image_width, this->_maix_image_height, any_cast<libmaix_image_mode_t>(py_to_pram[this->get_to(this->_maix_image_type)][0]), LIBMAIX_IMAGE_LAYOUT_HWC, NULL, true);
    if (this->_img)
    {
      memcpy(this->_img->data, im->data, this->_maix_image_size);
    }
    else
    {
      this->v_close();
    }
    return *this;
  }
  break;
  case py_input_PIL:
  {
    auto tobytes = data.attr("tobytes");
    auto img_bytes = tobytes();
    auto img_tmp = img_bytes.cast<std::string>();
    auto img_size = data.attr("size").cast<std::vector<int>>();
    this->_maix_image_type = data.attr("mode").cast<std::string>();
    this->_maix_image_width = img_size[0];
    this->_maix_image_height = img_size[1];
    this->_img = libmaix_image_create_patch(this->_img, this->_maix_image_width, this->_maix_image_height, any_cast<libmaix_image_mode_t>(py_to_pram[this->get_to(this->_maix_image_type)][0]), LIBMAIX_IMAGE_LAYOUT_HWC, NULL, true);
    this->_maix_image_size = this->_maix_image_width * this->_maix_image_height * any_cast<int>(py_to_pram[this->get_to(this->_maix_image_type)][1]);
    if (this->_img)
    {
      memcpy(this->_img->data, img_tmp.c_str(), this->_maix_image_size);
    }
    else
    {
      this->v_close();
    }
    return *this;
  }
  break;
  case py_input_numpy:
  {
    auto tobytes = data.attr("tobytes");
    auto img_bytes = tobytes();
    auto img_tmp = img_bytes.cast<std::string>();
    this->_maix_image_type = mode;
    this->_maix_image_width = size[0];
    this->_maix_image_height = size[1];
    this->_img = libmaix_image_create_patch(this->_img, this->_maix_image_width, this->_maix_image_height, any_cast<libmaix_image_mode_t>(py_to_pram[this->get_to(this->_maix_image_type)][0]), LIBMAIX_IMAGE_LAYOUT_HWC, NULL, true);
    this->_maix_image_size = this->_maix_image_width * this->_maix_image_height * any_cast<int>(py_to_pram[this->get_to(this->_maix_image_type)][1]);
    if (this->_img)
    {
      memcpy(this->_img->data, img_tmp.c_str(), this->_maix_image_size);
    }
    else
    {
      this->v_close();
    }
    return *this;
  }
  break;

  default:
    return *this;
    break;
  }
}

maix_image &maix_image::_open_file(std::string path)
{
  this->v_close();
  libmaix_image_t *tmp_img = NULL;
  if (libmaix_cv_image_open_file(&tmp_img, path.c_str()) != 0)
  {
    this->v_close();
    return *this;
  }
  this->_img = tmp_img;
  this->_maix_image_width = this->_img->width;
  this->_maix_image_height = this->_img->height;
  this->_maix_image_type = "RGB";
  this->_maix_image_size = this->_maix_image_width * this->_maix_image_height * 3;
  return *this;
}

py::object maix_image::_to_py(std::string im)
{
  if (NULL == this->_img)
  {
    py::print("no img");
    return py::none();
  }
  if (im == "maix_image")
  {
    auto _maix_Image_ = py::module::import("_maix_image");
    auto py_Image = _maix_Image_.attr("Image");
    auto py_img_obj = py_Image();
    auto py_fun_load = py_img_obj.attr("load");
    py_fun_load(this);
    return py_img_obj;
  }
#ifdef enable_opencv
  else if (im == "opencv")
  {
  }
#endif
#ifdef enable_PIL
  else if (im == "PIL")
  {
    if (this->_maix_image_type != "RGB16")
    {
      auto _PIL_ = py::module::import("PIL.Image");
      auto frombytes = _PIL_.attr("frombytes");
      py::bytes tmp((const char *)this->_img->data, this->_maix_image_size);
      py::tuple sizek = py::make_tuple(this->_maix_image_width, this->_maix_image_height);
      auto PIL_img = frombytes(this->_maix_image_type, sizek, tmp);
      return PIL_img;
    }
    else
    {
      return py::none();
    }
  }
#endif
#ifdef enable_numpy
  else if (im == "numpy")
  {
    if (this->_maix_image_type != "RGB16")
    {
      auto _numpy_ = py::module::import("numpy");
      auto from_buf = _numpy_.attr("frombuffer");
      auto _numpy_dtype = _numpy_.attr("uint8");
      py::bytes tmp((const char *)this->_img->data, this->_maix_image_size);
      auto numpy_obj = from_buf(tmp, _numpy_dtype);
      // auto numpy_obj_shape = numpy_obj.attr("shape");
      if (this->_maix_image_type == "RGB")
      {
        numpy_obj.attr("shape") = py::make_tuple(this->_maix_image_width, this->_maix_image_height, 3);
      }
      else if (this->_maix_image_type == "RGBA")
      {
        numpy_obj.attr("shape") = py::make_tuple(this->_maix_image_width, this->_maix_image_height, 4);
      }
      else if (this->_maix_image_type == "L")
      {
        numpy_obj.attr("shape") = py::make_tuple(this->_maix_image_width, this->_maix_image_height);
      }
      return numpy_obj;
    }
    else
    {
      return py::none();
    }
  }
#endif
}

void maix_image::_clear()
{
  this->v_close();
}

void maix_image::_load_freetype(std::string path)
{
  libmaix_cv_image_load_freetype(path.c_str());
}

py::tuple maix_image::_get_string_size(std::string str, double scale, int thickness)
{
  int w = 0, h = 0;
  libmaix_cv_image_get_string_size(&w, &h, str.c_str(), scale, thickness);
  return py::make_tuple(w, h);
}

void maix_image::_show()
{
  if (NULL == this->_img)
  {
    py::print("no img");
    return;
  }
  auto _maix_display = py::module::import("_maix_display");
  auto Display = _maix_display.attr("Display");
  auto show = Display();
  auto draw__ = show.attr("draw");
  if ((this->_maix_image_type == "RGB") && (this->_maix_image_width == 240) && (this->_maix_image_height == 240))
  {
    py::bytes tmp((const char *)this->_img->data, 240 * 240 * 3);
    draw__(tmp);
    return;
  }
  libmaix_image_t *tmp = libmaix_image_create(240, 240, any_cast<libmaix_image_mode_t>(this->py_to_pram[2][0]), LIBMAIX_IMAGE_LAYOUT_HWC, NULL, true);
  if (this->_maix_image_type != "RGB")
  {
    if (libmaix_cv_image_convert(this->_img, any_cast<libmaix_image_mode_t>(py_to_pram[2][0]), &tmp) != 0)
    {
      libmaix_image_destroy(&tmp);
      return;
    }
    if (this->_maix_image_width == 240 && this->_maix_image_height == 240)
    {
      py::bytes tmp_bytes((const char *)tmp->data, 240 * 240 * 3);
      draw__(tmp_bytes);
      libmaix_image_destroy(&tmp);
      return;
    }
    else
    {
      libmaix_image_t *tmp_two = libmaix_image_create(240, 240, any_cast<libmaix_image_mode_t>(py_to_pram[2][0]), LIBMAIX_IMAGE_LAYOUT_HWC, NULL, true);
      cv::Mat src(tmp->height, tmp->width, any_cast<int>(py_to_pram[2][2]), tmp->data);
      cv::Mat dst(240, 240, any_cast<int>(py_to_pram[2][2]), tmp_two->data);
      cv::resize(src, dst, cv::Size(240, 240));
      py::bytes tmp_bytes((const char *)tmp_two->data, 240 * 240 * 3);
      draw__(tmp_bytes);
      libmaix_image_destroy(&tmp);
      libmaix_image_destroy(&tmp_two);
      return;
    }
  }
  else
  {
    cv::Mat src(this->_img->height, this->_img->width, any_cast<int>(py_to_pram[2][2]), this->_img->data);
    cv::Mat dst(240, 240, any_cast<int>(py_to_pram[2][2]), tmp->data);
    cv::resize(src, dst, cv::Size(240, 240));
    py::bytes tmp_bytes((const char *)tmp->data, 240 * 240 * 3);
    draw__(tmp_bytes);
    libmaix_image_destroy(&tmp);
    return;
  }
}

int maix_image::_save(std::string file_path, std::string format)
{
  if (NULL == this->_img)
    return -1;
  libmaix_cv_image_draw_image_save(this->_img, file_path.c_str());
  return 0;
}

py::bytes maix_image::_tobytes()
{
  if (NULL == this->_img)
    return py::bytes();
  return py::bytes((const char *)this->_img->data, this->_maix_image_size);
}

maix_image &maix_image::_resize(int w, int h, int func)
{
  if (NULL == this->_img)
  {
    py::print("no img");
    return *this;
  }
  if (this->_img->width == w && this->_img->height == h)
    return *this;
  libmaix_image_t *tmp = libmaix_image_create(w, h, this->_img->mode, LIBMAIX_IMAGE_LAYOUT_HWC, NULL, true);
  if (tmp)
  {
    cv::Mat src(this->_img->height, this->_img->width, any_cast<int>(py_to_pram[this->get_to(this->_maix_image_type)][2]), this->_img->data);
    cv::Mat dst(h, w, any_cast<int>(py_to_pram[this->get_to(this->_maix_image_type)][2]), tmp->data);
    switch (func)
    {
    case 0: //INTER_NEAREST
    {
      cv::resize(src, dst, cv::Size(w, h), cv::INTER_NEAREST);
    }
    break;
    case 1: //INTER_LINEAR
    {
      cv::resize(src, dst, cv::Size(w, h), cv::INTER_LINEAR);
    }
    break;
    case 3: //INTER_AREA
    {
      cv::resize(src, dst, cv::Size(w, h), cv::INTER_AREA);
    }
    break;
    case 2: //INTER_CUBIC
    {
      cv::resize(src, dst, cv::Size(w, h), cv::INTER_CUBIC);
    }
    break;
    case 4: //INTER_LANCZOS4
    {
      cv::resize(src, dst, cv::Size(w, h), cv::INTER_LANCZOS4);
    }
    break;
    default: //INTER_LINEAR
      cv::resize(src, dst, cv::Size(w, h), cv::INTER_LINEAR);
      break;
    }
    if (dst.data != tmp->data)
    {
      memcpy(tmp->data, dst.data, h * w * any_cast<int>(py_to_pram[this->get_to(this->_maix_image_type)][1]));
    }
    libmaix_image_destroy(&this->_img), this->_img = tmp;
    this->_maix_image_height = h;
    this->_maix_image_width = w;
    this->_maix_image_size = w * h * any_cast<int>(py_to_pram[this->get_to(this->_maix_image_type)][1]);
  }
  else
  {
    libmaix_image_destroy(&tmp);
  }
  return *this;
}

maix_image &maix_image::_draw_line(int x1, int y1, int x2, int y2, std::vector<int> color, int thickness)
{
  if (NULL == this->_img)
  {
    py::print("no img");
    return *this;
  }
  libmaix_cv_image_draw_line(this->_img, x1, y1, x2, y2, MaixColor(color[0], color[1], color[2]), thickness);
  return *this;
}

maix_image &maix_image::_draw_rectangle(int x, int y, int w, int h, std::vector<int> color, int thickness)
{
  if (NULL == this->_img)
  {
    py::print("no img");
    return *this;
  }
  libmaix_cv_image_draw_rectangle(this->_img, x, y, w, h, MaixColor(color[0], color[1], color[2]), thickness);
  return *this;
}

maix_image &maix_image::_draw_circle(int x, int y, int r, std::vector<int> color, int thickness)
{
  if (NULL == this->_img)
  {
    py::print("no img");
    return *this;
  }
  libmaix_cv_image_draw_circle(this->_img, x, y, r, MaixColor(color[0], color[1], color[2]), thickness);
  return *this;
}

maix_image &maix_image::_draw_ellipse(int cx, int cy, int rx, int ry, double angle, double startAngle, double endAngle, std::vector<int> color, int thickness)
{
  if (NULL == this->_img)
  {
    py::print("no img");
    return *this;
  }
  libmaix_cv_image_draw_ellipse(this->_img, cx, cy, rx, ry, angle, startAngle, endAngle, MaixColor(color[0], color[1], color[2]), thickness);
  return *this;
}

maix_image &maix_image::_draw_string(int x, int y, std::string str, double scale, std::vector<int> color, int thickness)
{
  if (NULL == this->_img)
  {
    py::print("no img");
    return *this;
  }
  libmaix_cv_image_draw_string(this->_img, x, y, str.c_str(), scale, MaixColor(color[0], color[1], color[2]), thickness);
  return *this;
}

maix_image &maix_image::_rotate(double rotate, int adjust)
{
  if (NULL == this->_img)
  {
    py::print("no img");
    return *this;
  }
  libmaix_image_t *tmp = NULL;
  if (libmaix_cv_image_rotate(this->_img, rotate, adjust, &tmp) != 0)
  {
    return *this;
  }
  libmaix_image_destroy(&this->_img), this->_img = tmp;
  return *this;
}

maix_image &maix_image::_convert(std::string mode)
{
  if (NULL == this->_img)
  {
    py::print("no img");
    return *this;
  }
  libmaix_image_t *tmp = libmaix_image_create(this->_img->width, this->_img->height, any_cast<libmaix_image_mode_t>(this->py_to_pram[this->get_to(mode)][0]), LIBMAIX_IMAGE_LAYOUT_HWC, NULL, true);
  if (tmp)
  {
    if (libmaix_cv_image_convert(this->_img, any_cast<libmaix_image_mode_t>(py_to_pram[this->get_to(mode)][0]), &tmp) == 0)
    {
      libmaix_image_destroy(&this->_img), this->_img = tmp;
      this->_maix_image_type = mode;
      this->_maix_image_size = this->_img->width * this->_img->height * any_cast<int>(py_to_pram[this->get_to(mode)][1]);
    }
    else
    {
      libmaix_image_destroy(&tmp);
    }
  }
  else
  {
    libmaix_image_destroy(&tmp);
  }
  return *this;
}

void maix_image::local_load(libmaix_image_t *img)
{
  this->_img = img;
  this->_maix_image_width = img->width;
  this->_maix_image_height = img->height;
  switch (img->mode)
  {
  case LIBMAIX_IMAGE_MODE_GRAY:
    this->_maix_image_type = "L";
    this->_maix_image_size = img->width * img->height;
    break;
  case LIBMAIX_IMAGE_MODE_RGB565:
    this->_maix_image_type = "RGB16";
    this->_maix_image_size = img->width * img->height * 2;
    break;
  case LIBMAIX_IMAGE_MODE_RGB888:
    this->_maix_image_type = "RGB";
    this->_maix_image_size = img->width * img->height * 3;
    break;
  case LIBMAIX_IMAGE_MODE_RGBA8888:
    this->_maix_image_type = "RGBA";
    this->_maix_image_size = img->width * img->height * 4;
    break;
  default:
    break;
  }
}

maix_image *maix_image::_draw_crop(int x, int y, int w, int h)
{
  if (NULL == this->_img)
  {
    py::print("no img");
    maix_image *tmp_img = new maix_image();
    return tmp_img;
  }
  libmaix_image_t *tmp = libmaix_image_create(w, h, this->_img->mode, LIBMAIX_IMAGE_LAYOUT_HWC, NULL, true);
  if (tmp)
  {
    maix_image *tmp_img = new maix_image();
    if (libmaix_cv_image_crop(this->_img, x, y, w, h, &tmp) != 0)
    {
      return tmp_img;
    }
    tmp_img->local_load(tmp);
    return tmp_img;
  }
  else
  {
    libmaix_image_destroy(&tmp);
  }
  maix_image *tmp_img = new maix_image();
  return tmp_img;
}

maix_image &maix_image::_draw_image(py::object data, int x, int y, double alpha)
{
  if (NULL == this->_img)
  {
    py::print("no img");
    return *this;
  }
  if (py_input_maix_image != this->get_img_type(data))
    return *this;
  if ("RGB" != data.attr("mode").cast<std::string>())
    return *this;
  auto c_img_p = data.attr("c_img_private");
  size_t im_p = c_img_p().cast<size_t>();
  libmaix_image_t *im = (libmaix_image_t *)im_p;
  libmaix_cv_image_draw_image(this->_img, x, y, im, alpha);
  return *this;
}

std::vector<int> maix_image::_get_pixel(int x, int y)
{
  std::vector<int> color_val;
  if (NULL == this->_img)
    return color_val;
  libmaix_image_color_t colot_tmp = libmaix_cv_image_get_pixel(this->_img, x, y);
  color_val.push_back(colot_tmp.rgb888.r);
  color_val.push_back(colot_tmp.rgb888.g);
  color_val.push_back(colot_tmp.rgb888.b);
  color_val.push_back(colot_tmp.rgb888.a);
  return color_val;
}

maix_image &maix_image::_set_pixel(int x, int y, std::vector<int> color)
{
  if (NULL == this->_img)
  {
    py::print("no img");
    return *this;
  }
  libmaix_image_color_t colot_tmp;
  switch (this->get_to(this->_maix_image_type))
  {
  case 0:
    colot_tmp.rgb888.r = color[0];
    colot_tmp.rgb888.g = 0;
    colot_tmp.rgb888.b = 0;
    colot_tmp.rgb888.a = 0;
    break;
  case 2:
    colot_tmp.rgb888.r = color[0];
    colot_tmp.rgb888.g = color[1];
    colot_tmp.rgb888.b = color[2];
    colot_tmp.rgb888.a = 0;
    break;
  case 3:
    colot_tmp.rgb888.r = color[0];
    colot_tmp.rgb888.g = color[1];
    colot_tmp.rgb888.b = color[2];
    colot_tmp.rgb888.a = color[3];
    break;
  default:
    return *this;
    break;
  }
  libmaix_cv_image_set_pixel(this->_img, x, y, colot_tmp);
  return *this;
}