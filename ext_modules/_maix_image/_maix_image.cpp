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

std::string maix_image::str__()
{
  std::string info_str;
  std::stringstream ss;
  ss << "<_maix_image.Image " << this->_img << " \" width\":" << this->_maix_image_width
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
  libmaix_err_t err = libmaix_cv_image_open_file(&tmp_img, path.c_str());
  if (LIBMAIX_ERR_NOT_EXEC == err) // maybe is bytes jpg.
  {
    try
    {
      // maybe is jpg or bmp bytes
      cv::Mat tmp(1, path.size(), CV_8U, (char *)path.data());
      cv::Mat image = cv::imdecode(tmp, cv::IMREAD_COLOR);
      if (image.empty()) {
        printf("[image.open] %s file does not exist\r\n", path.c_str());
        this->v_close();
        return *this;
      }
      cv::cvtColor(image, image, cv::ColorConversionCodes::COLOR_BGR2RGB);
      tmp_img = libmaix_image_create(image.cols, image.rows, LIBMAIX_IMAGE_MODE_RGB888, LIBMAIX_IMAGE_LAYOUT_HWC, NULL, true);
      memcpy(tmp_img->data, image.data, tmp_img->width * tmp_img->height * 3);
    }
    catch (const std::exception &e)
    {
      std::cerr << e.what() << '\n';
      this->v_close();
      return *this;
    }
  }
  else if (LIBMAIX_ERR_NONE != err)
  {
    printf("[image.open] %s file does not exist\r\n", path.c_str());
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
    py::print("[image] is empty !");
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
  return py::none();
}

void maix_image::_delete()
{
  this->v_close();
}
maix_image &maix_image::_clear()
{
  uint8_t *img_p_t = (uint8_t *)this->_img->data;
  memset(img_p_t, 0, this->_maix_image_size);
  return *this;
}
void maix_image::_show()
{
  if (NULL == this->_img)
  {
    py::print("[image] is empty !");
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
  libmaix_cv_image_save_file(this->_img, file_path.c_str());
  return 0;
}

py::bytes maix_image::_tobytes(std::string format, std::vector<int> params)
{
  if (NULL == this->_img)
    return py::bytes();

  if (format == "rgb")
    return py::bytes((const char *)this->_img->data, this->_maix_image_size);

  std::vector<uchar> encoded_buffer;
  cv::Mat cv_is_bgr, rgb(this->_img->height, this->_img->width, CV_8UC3, this->_img->data);
  cv::cvtColor(rgb, cv_is_bgr, cv::COLOR_RGB2BGR);

  if (format == "jpg")
  {
    if (params.size() == 0)
      params.push_back(CV_IMWRITE_JPEG_QUALITY), params.push_back(95);
    cv::imencode(".jpeg", cv_is_bgr, encoded_buffer, params);
    return py::bytes((const char *)encoded_buffer.data(), encoded_buffer.size());
  }
  if (format == "png")
  {
    if (params.size() == 0)
      params.push_back(CV_IMWRITE_PNG_COMPRESSION), params.push_back(3);
    cv::imencode(".png", cv_is_bgr, encoded_buffer, params);
    return py::bytes((const char *)encoded_buffer.data(), encoded_buffer.size());
  }
  if (format == "bmp")
  {
    cv::imencode(".bmp", cv_is_bgr, encoded_buffer, params);
    return py::bytes((const char *)encoded_buffer.data(), encoded_buffer.size());
  }
  return py::bytes();
}

maix_image &maix_image::_resize(int dst_w, int dst_h, int func, int padding, std::vector<int> size)
{
  if (NULL == this->_img)
  {
    py::print("[image] is empty !");
    return *this;
  }
  if (dst_w == 0 && dst_h == 0)
    dst_w = size[0], dst_h = size[1];
  int src_w = this->_img->width, src_h = this->_img->height;
  if (src_w == dst_w && src_h == dst_h)
    return *this;
  libmaix_image_t *tmp = libmaix_image_create(dst_w, dst_h, this->_img->mode, LIBMAIX_IMAGE_LAYOUT_HWC, NULL, true);
  if (tmp)
  {
    cv::Mat src(src_h, src_w, any_cast<int>(py_to_pram[this->get_to(this->_maix_image_type)][2]), this->_img->data);
    cv::Mat dst(dst_h, dst_w, any_cast<int>(py_to_pram[this->get_to(this->_maix_image_type)][2]), tmp->data);

    if (padding)
    {
      float scale_src = ((float)src_w) / ((float)src_h), scale_dst = ((float)dst_w) / ((float)dst_h);
      if (scale_dst == scale_src)
      {
        // func == InterpolationFlags default 1 == cv::INTER_LINEAR
        cv::resize(src, dst, cv::Size(dst_w, dst_h), func);
      }
      else
      {
        // Scale to original image
        int new_w = 0, new_h = 0, top = 0, bottom = 0, left = 0, right = 0;
        if (scale_src > scale_dst)
        {
          new_w = dst_w, new_h = new_w * src_h / src_w; // new_h / src_h = new_w / src_w => new_h = new_w * src_h / src_w
          top = (dst_h - new_h) / 2, bottom = top;
        }
        else
        { // Division loses precision
          new_h = dst_h, new_w = new_h * src_w / src_h;
          left = (dst_w - new_w) / 2, right = left;
        }
        // printf("_resize %d %d > %d %d > %d %d : %d %d %d %d \r\n", src_w, src_h, new_w, new_h, dst_w, dst_h, top, bottom, left, right);
        cv::Mat tmp;
        cv::resize(src, tmp, cv::Size(new_w, new_h), func);
        cv::copyMakeBorder(tmp, dst, top, bottom, left, right, IPL_BORDER_CONSTANT);
      }
    }
    else
    {
      // func == InterpolationFlags default 1 == cv::INTER_LINEAR
      cv::resize(src, dst, cv::Size(dst_w, dst_h), func);
    }
    if (dst.data != tmp->data)
    {
      memcpy(tmp->data, dst.data, dst_h * dst_w * any_cast<int>(py_to_pram[this->get_to(this->_maix_image_type)][1]));
    }
    libmaix_image_destroy(&this->_img), this->_img = tmp;
    this->_maix_image_height = dst_h;
    this->_maix_image_width = dst_w;
    this->_maix_image_size = dst_w * dst_h * any_cast<int>(py_to_pram[this->get_to(this->_maix_image_type)][1]);
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
    py::print("[image] is empty !");
    return *this;
  }
  libmaix_cv_image_draw_line(this->_img, x1, y1, x2, y2, MaixColor(color[0], color[1], color[2]), thickness);
  return *this;
}

maix_image &maix_image::_draw_cross(int x, int y, int c, int size, int thickness)
{
  if (NULL == this->_img)
  {
    py::print("[image] is empty !");
    return *this;
  }
  image_t img = {};
  img.w = this->_img->width;
  img.h = this->_img->height;
  img.pixels = (uint8_t *)this->_img->data;
  img.pixfmt = PIXFORMAT_RGB888;
  imlib_draw_cross(&img, x, y, c, size, thickness);
  return *this;
}

maix_image &maix_image::_draw_rectangle(int x1_x, int y1_y, int x2_w, int y2_h, std::vector<int> color, int thickness, int is_xywh)
{
  if (NULL == this->_img)
  {
    py::print("[image] is empty !");
    return *this;
  }
  if (is_xywh)
    x2_w = x1_x + x2_w, y2_h = y1_y + y2_h;
  libmaix_cv_image_draw_rectangle(this->_img, x1_x, y1_y, x2_w, y2_h, MaixColor(color[0], color[1], color[2]), thickness);
  return *this;
}

maix_image &maix_image::_draw_circle(int x, int y, int r, std::vector<int> color, int thickness)
{
  if (NULL == this->_img)
  {
    py::print("[image] is empty !");
    return *this;
  }
  libmaix_cv_image_draw_circle(this->_img, x, y, r, MaixColor(color[0], color[1], color[2]), thickness);
  return *this;
}

maix_image &maix_image::_draw_ellipse(int cx, int cy, int rx, int ry, double angle, double startAngle, double endAngle, std::vector<int> color, int thickness)
{
  if (NULL == this->_img)
  {
    py::print("[image] is empty !");
    return *this;
  }
  libmaix_cv_image_draw_ellipse(this->_img, cx, cy, rx, ry, angle, startAngle, endAngle, MaixColor(color[0], color[1], color[2]), thickness);
  return *this;
}

maix_image &maix_image::_draw_string(int x, int y, std::string str, double scale, std::vector<int> color, int thickness)
{
  if (NULL == this->_img)
  {
    py::print("[image] is empty !");
    return *this;
  }
  libmaix_cv_image_draw_string(this->_img, x, y, str.c_str(), scale, MaixColor(color[0], color[1], color[2]), thickness);
  return *this;
}

maix_image &maix_image::_rotate(double angle, int adjust)
{
  if (NULL == this->_img)
  {
    py::print("[image] is empty !");
    return *this;
  }
  libmaix_image_t *tmp = NULL;
  if (libmaix_cv_image_rotate(this->_img, angle, adjust, &tmp) != 0)
  {
    return *this;
  }
  libmaix_image_destroy(&this->_img), this->_img = tmp;
  return *this;
}

maix_image &maix_image::_flip(int flip)
{
  if (NULL == this->_img)
  {
    py::print("[image] is empty !");
    return *this;
  }
  libmaix_cv_image_flip(this->_img, flip);
  return *this;
}

maix_image &maix_image::_convert(std::string mode)
{
  if (NULL == this->_img)
  {
    py::print("[image] is empty !");
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
    py::print("[image] is empty !");
    maix_image *tmp_img = new maix_image();
    return tmp_img;
  }
  x = std::max(x, 0);
  y = std::max(y, 0);
  w = std::min(w, this->_img->width - x);
  h = std::min(h, this->_img->height - y);
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
    py::print("[image] is empty !");
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
    py::print("[image] is empty !");
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

maix_image &maix_image::_hist_eq(bool adaptive, float clip_limit, maix_image &mask)
{
  if (NULL == this->_img)
  {
    py::print("[image] is empty !");
    return *this;
  }

  image_t img = {};
  img.w = this->_img->width;
  img.h = this->_img->height;
  img.pixels = (uint8_t *)this->_img->data;
  img.pixfmt = PIXFORMAT_RGB888;

  image_t *mask_img = NULL;
  if (NULL != mask._img && this->_img->width == mask._img->width && this->_img->height == mask._img->height)
  {
    mask_img = (image_t *)malloc(sizeof(image_t));
    if (mask_img)
    {
      mask_img->w = mask._img->width;
      mask_img->h = mask._img->height;
      mask_img->pixels = (uint8_t *)mask._img->data;
      mask_img->pixfmt = PIXFORMAT_RGB888;
    }
  }

  fb_alloc_mark();
  if (adaptive)
    imlib_clahe_histeq(&img, clip_limit, mask_img);
  else
    imlib_histeq(&img, mask_img);
  fb_alloc_free_till_mark();

  if (NULL != mask._img && this->_img->width == mask._img->width && this->_img->height == mask._img->height)
    free(mask_img), mask_img = NULL;

  return *this;
}

maix_image &maix_image::_gamma_corr(float gamma, float contrast, float brightness)
{
  if (NULL == this->_img)
  {
    py::print("[image] is empty !");
    return *this;
  }
  image_t img = {};
  img.w = this->_img->width;
  img.h = this->_img->height;
  img.pixels = (uint8_t *)this->_img->data;
  img.pixfmt = PIXFORMAT_RGB888;
  fb_alloc_mark();
  imlib_gamma_corr(&img, gamma, contrast, contrast);
  fb_alloc_free_till_mark();
  return *this;
}

maix_image &maix_image::_lens_corr(float strength, float zoom, float x_corr, float y_corr)
{
  if (NULL == this->_img)
  {
    py::print("[image] is empty !");
    return *this;
  }

  image_t img = {};
  img.w = this->_img->width;
  img.h = this->_img->height;
  img.pixels = (uint8_t *)this->_img->data;
  img.pixfmt = PIXFORMAT_RGB888;

  fb_alloc_mark();
  imlib_lens_corr(&img, strength, zoom, x_corr, y_corr);
  fb_alloc_free_till_mark();

  return *this;
}

maix_image &maix_image::_mean(const int ksize, bool threshold, int offset, bool invert, maix_image &mask)
{
  if (NULL == this->_img)
  {
    py::print("[image] is empty !");
    return *this;
  }

  image_t img = {};
  img.w = this->_img->width;
  img.h = this->_img->height;
  img.pixels = (uint8_t *)this->_img->data;
  img.pixfmt = PIXFORMAT_RGB888;

  image_t *mask_img = NULL;
  if (NULL != mask._img) // check if mask is NULL
  {
    if (this->_img->width == mask._img->width && this->_img->height == mask._img->height) // check if mask has same size with image
    {
      mask_img = (image_t *)malloc(sizeof(image_t));
      if (mask_img)
      {
        mask_img->w = mask._img->width;
        mask_img->h = mask._img->height;
        mask_img->pixels = (uint8_t *)mask._img->data;
        mask_img->pixfmt = PIXFORMAT_RGB888;
      }
    }
    else
      printf("The size of mask is different with input image,use default mask!");
  }

  fb_alloc_mark();
  imlib_mean_filter(&img, ksize, offset, threshold, invert, mask_img);
  fb_alloc_free_till_mark();

  if (mask_img != NULL)
    free(mask_img), mask_img = NULL;

  return *this;
}

py::list maix_image::_imlib_get_statistics(std::vector<int> roi_src, std::vector<std::vector<int>> &thresholds_src, bool invert, maix_image &other_src, int bins, int l_bins, int a_bins, int b_bins)
{

  if (NULL == this->_img)
  {
    py::print("[image] is empty !");
    return py::none();
  }

  image_t img_tmp = {}, *arg_img = &img_tmp;
  arg_img->w = this->_img->width;
  arg_img->h = this->_img->height;
  arg_img->pixels = (uint8_t *)this->_img->data;
  arg_img->pixfmt = PIXFORMAT_RGB888;

  image_t other_img = {}, *other = NULL;
  if (NULL != other_src._img)
  {
    other->w = other_src._img->width;
    other->h = other_src._img->height;
    other->pixels = (uint8_t *)other_src._img->data;
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

  if (roi_src[2] == 0)
    roi_src[2] = arg_img->w;
  if (roi_src[3] == 0)
    roi_src[3] = arg_img->h;

  rectangle_t roi = {roi_src[0], roi_src[1], roi_src[2], roi_src[3]};

  histogram_t hist;

  switch (arg_img->pixfmt)
  {
  case PIXFORMAT_GRAYSCALE:
  {
    if (bins >= 2 && bins <= 255)
    {
      hist.LBinCount = bins;
    }
    else
    {
      hist.LBinCount = bins = 255;
    }
    if (hist.LBinCount >= 2)
    {
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
  case PIXFORMAT_RGB565:
  case PIXFORMAT_RGB888:
  {
    if (bins >= 2 && bins <= 255)
    {
      hist.LBinCount = bins;
    }
    else
    {
      hist.LBinCount = bins = 255;
    }
    if (l_bins < 2)
      l_bins = bins;
    hist.LBinCount = l_bins;
    if (a_bins < 2)
      a_bins = bins;
    hist.ABinCount = a_bins;
    if (b_bins < 2)
      b_bins = bins;
    hist.BBinCount = b_bins;

    if (hist.LBinCount >= 2 && hist.ABinCount >= 2 && hist.BBinCount >= 2)
    {
      hist.LBins = (float *)fb_alloc(hist.LBinCount * sizeof(float), FB_ALLOC_NO_HINT);
      hist.ABins = (float *)fb_alloc(hist.ABinCount * sizeof(float), FB_ALLOC_NO_HINT);
      hist.BBins = (float *)fb_alloc(hist.BBinCount * sizeof(float), FB_ALLOC_NO_HINT);
      imlib_get_histogram(&hist, arg_img, &roi, &thresholds, invert, other);
      list_free(&thresholds);
    }
    break;
  }
  }

  statistics_t stats;
  imlib_get_statistics(&stats, (pixformat_t)arg_img->pixfmt, &hist);

  py::list tmps;

  tmps.append(stats.LMean);
  tmps.append(stats.LMedian);
  tmps.append(stats.LMode);
  tmps.append(stats.LSTDev);
  tmps.append(stats.LMin);
  tmps.append(stats.LMax);
  tmps.append(stats.LLQ);
  tmps.append(stats.LUQ);

  tmps.append(stats.AMean);
  tmps.append(stats.AMedian);
  tmps.append(stats.AMode);
  tmps.append(stats.ASTDev);
  tmps.append(stats.AMin);
  tmps.append(stats.AMax);
  tmps.append(stats.ALQ);
  tmps.append(stats.AUQ);

  tmps.append(stats.BMean);
  tmps.append(stats.BMedian);
  tmps.append(stats.BMode);
  tmps.append(stats.BSTDev);
  tmps.append(stats.BMin);
  tmps.append(stats.BMax);
  tmps.append(stats.BLQ);
  tmps.append(stats.BUQ);

  fb_alloc_free_till_mark();

  return tmps;
}

maix_image &maix_image::_imlib_rotation_corr(float x_rotation, float y_rotation, float z_rotation, float x_translation, float y_translation, float zoom, float fov, std::vector<std::vector<float>> corners)
{
  if (NULL == this->_img)
  {
    py::print("[image] is empty !");
    return *this;
  }

  // image_t *imlib_img = imlib_image_create(img->width, img->height, PIXFORMAT_RGB888, img->width * img->height * PIXFORMAT_BPP_RGB888, img->data, false);

  image_t img_tmp = {}, *img = &img_tmp;
  img->w = this->_img->width;
  img->h = this->_img->height;
  img->pixels = (uint8_t *)this->_img->data;
  img->pixfmt = PIXFORMAT_RGB888;

  fb_alloc_mark();

  imlib_rotation_corr(img, x_rotation, y_rotation, z_rotation, x_translation, y_translation, zoom, fov, NULL);

  fb_alloc_free_till_mark();

  // imlib_image_destroy(&resize_img);

  return *this;
}

maix_image &maix_image::_opencv_Canny(double threshold1, double threshold2, int apertureSize, bool L2gradient)
{
  if (NULL == this->_img)
  {
    py::print("[image] is empty !");
    return *this;
  }
  if (this->_maix_image_type == "RGB")
  {
    cv::Mat gray, rgb(this->_img->height, this->_img->width, CV_8UC3, this->_img->data);
    cv::cvtColor(rgb, gray, cv::COLOR_RGB2GRAY);
    cv::Canny(gray, gray, threshold1, threshold2, apertureSize, L2gradient);
    cv::cvtColor(gray, rgb, cv::COLOR_GRAY2RGB);
  }
  return *this;
}

py::dict maix_image::_imlib_find_template(maix_image &template_src, float arg_thresh, std::vector<int> roi_src, int step = 2, int search = SEARCH_EX)
{
  py::dict return_val;
  if (NULL == this->_img || NULL == template_src._img)
  {
    return return_val;
  }

  cv::Mat gray, rgb(this->_img->height, this->_img->width, CV_8UC3, this->_img->data);
  cv::cvtColor(rgb, gray, cv::COLOR_RGB2GRAY);

  image_t _arg_img = {
              .w = gray.cols,
              .h = gray.rows,
          },
          *arg_img = &_arg_img;
  arg_img->size = gray.cols * gray.rows;
  arg_img->data = gray.data;
  arg_img->pixfmt = PIXFORMAT_GRAYSCALE;

  cv::Mat template_gray, template_rgb(template_src._img->height, template_src._img->width, CV_8UC3, template_src._img->data);
  cv::cvtColor(template_rgb, template_gray, cv::COLOR_RGB2GRAY);

  image_t _arg_template = {
              .w = template_gray.cols,
              .h = template_gray.rows,
          },
          *arg_template = &_arg_template;
  arg_template->size = template_gray.cols * template_gray.rows;
  arg_template->data = template_gray.data;
  arg_template->pixfmt = PIXFORMAT_GRAYSCALE;

  if (roi_src[2] == 0)
    roi_src[2] = arg_img->w;
  if (roi_src[3] == 0)
    roi_src[3] = arg_img->h;

  rectangle_t roi = {roi_src[0], roi_src[1], roi_src[2], roi_src[3]};

  if (roi.w >= arg_template->w && roi.h >= arg_template->h)
  {
    // printf("roi.w:%d, roi.h:%d, arg_template->w:%d, arg_template->h:%d\n", roi.w, roi.h, arg_template->w, arg_template->h);
    if ((roi.x + roi.w) <= arg_img->w && (roi.y + roi.h) <= arg_img->h)
    {
      // printf("roi.x:%d, roi.y:%d, roi.w:%d, roi.h:%d\n", roi.x, roi.y, roi.w, roi.h);
      rectangle_t r;
      float corr;
      fb_alloc_mark();
      if (search == SEARCH_DS)
      {
        corr = imlib_template_match_ds(arg_img, arg_template, &r);
      }
      else
      {
        corr = imlib_template_match_ex(arg_img, arg_template, &roi, step, &r);
      }
      fb_alloc_free_till_mark();
      if (corr > arg_thresh)
      {
        return_val["x"] = r.x;
        return_val["y"] = r.y;
        return_val["w"] = r.w;
        return_val["h"] = r.h;
        return_val["thresh"] = corr;
      }
    }
  }
  return return_val;
}

maix_image *maix_image::_warp_affine(std::vector<int> points, int w, int h)
{
  if (NULL == this->_img || points.size() < 3)
  {
    py::print("[image] is empty !");
    maix_image *tmp_img = new maix_image();
    return tmp_img;
  }
  w = std::min(w, this->_img->width);
  h = std::min(h, this->_img->height);
  libmaix_image_t *tmp = libmaix_image_create(w, h, LIBMAIX_IMAGE_MODE_RGB888, LIBMAIX_IMAGE_LAYOUT_HWC, NULL, true);
  if (tmp)
  {
    maix_image *tmp_img = new maix_image();
    int affine_dst_pts[6] = {w, h, 0, h, 0, 0}, *affine_src_pts = points.data();
    if (libmaix_cv_image_affine(this->_img, affine_src_pts, affine_dst_pts, h, w, &tmp) != LIBMAIX_ERR_NONE)
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