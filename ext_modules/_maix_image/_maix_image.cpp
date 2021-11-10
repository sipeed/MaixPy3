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

void maix_image::v_init()
{
  this->_img = NULL;
  // // LOG_INFO << "init base img!";
}
void maix_image::v_close()
{
  // // LOG_INFO << "start!";
  if (this->_img)
  {
    libmaix_image_destroy(&this->_img);
    // // LOG_INFO << "libmaix_image_destroy success";
  }
  this->_maix_image_width = 0;
  this->_maix_image_height = 0;
  this->_maix_image_size = 0;
}

maix_image::maix_image()
{
  // // LOG_INFO << "maix_image creat!";
  this->v_init();
}
maix_image::~maix_image()
{
  // // LOG_INFO << "~maix_image del";
  this->v_close();
}

std::string maix_image::repr__()
{
  return std::string("");
}

std::string maix_image::str__()
{
  std::string info_str;
  std::stringstream ss;
  ss << "<_maix_image.Image\"width\":" << this->_maix_image_width << ", \"height\":" << this->_maix_image_height << ", \"type\"=" << this->_maix_image_type << ", \"size\":" << this->_maix_image_size << ">";
  info_str = ss.str();
  return info_str;
}
int maix_image::len__()
{
  return this->_maix_image_size;
}

static inline libmaix_image_t * libmaix_image_create_patch(libmaix_image_t *self, int w, int h, libmaix_image_mode_t m, libmaix_image_layout_t l, void *d, bool i)
{
  // printf("libmaix_image_create_patch %p\r\n", self);
  // libmaix_image_destroy(&self);
  // return libmaix_image_create(w, h, m, l, d, i);
  if ((self == NULL) || (self->width != w && self->width != w && self->mode != m)) {
    libmaix_image_destroy(&self);
    return libmaix_image_create(w, h, m, l, d, i);
  }
  return self;
}

maix_image &maix_image::_new(std::vector<int> size, std::vector<int> color, std::string mode)
{
  // // LOG_INFO << "maix_image::_new start!";
  this->_maix_image_type = mode;
  this->_maix_image_width = size[0];
  this->_maix_image_height = size[1];
  this->_maix_image_size = this->_maix_image_width * this->_maix_image_height * any_cast<int>(this->py_to_pram[this->get_to(this->_maix_image_type)][1]);
  this->_img = libmaix_image_create_patch(this->_img, size[0], size[1], any_cast<libmaix_image_mode_t>(this->py_to_pram[this->get_to(this->_maix_image_type)][0]), LIBMAIX_IMAGE_LAYOUT_HWC, NULL, true);
  if (this->_img)
  {
    libmaix_cv_image_draw_rectangle(this->_img, 0, 0, size[0], size[1], MaixColor(color[0], color[1], color[2]), -1);
    // // LOG_INFO << "this->_img init success!";
  }
  else
  {
    this->v_close();
    this->_maix_image_size = 0;
    // LOG_INFO << "this->_img creat fail!";
  }
  return *this;
}

maix_image &maix_image::_load(py::object data, std::vector<int> size, std::string mode)
{
  if (py::isinstance<py::bytes>(data))
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
      // LOG_INFO << "img load fail!";
      this->v_close();
      this->_maix_image_size = 0;
    }
    return *this;
  }
  else
  {
    auto maix_image_type = py::module::import("maix_image").attr("Image");
    if (py::isinstance(data, maix_image_type))
    {
      auto tobytes = data.attr("tobytes"); //获取tobytes函数
      auto tmp_img_bytes = tobytes().cast<std::string>();
      this->_maix_image_width = data.attr("width").cast<int>();
      this->_maix_image_height = data.attr("height").cast<int>();
      this->_maix_image_size = data.attr("size").cast<int>();
      this->_maix_image_type = data.attr("mode").cast<std::string>();
      this->_img = libmaix_image_create(this->_maix_image_width, this->_maix_image_height, any_cast<libmaix_image_mode_t>(py_to_pram[this->get_to(this->_maix_image_type)][0]), LIBMAIX_IMAGE_LAYOUT_HWC, NULL, true);
      if (this->_img)
      {
        memcpy(this->_img->data, tmp_img_bytes.c_str(), this->_maix_image_size);
      }
      else
      {
        // LOG_INFO << "img load fail!";
        this->v_close();
        this->_maix_image_size = 0;
      }
      return *this;
    }
#ifdef enable_PIL
    auto PIL_ = py::module::import("PIL.Image").attr("Image");
    if (py::isinstance(data, PIL_))
    {
      auto tobytes = PIL_.attr("tobytes");          //获取tobytes函数
      auto img_bytes = tobytes(data);               //转化成python对象的img_bytes
      auto img_tmp = img_bytes.cast<std::string>(); //转换成c++的string
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
        // LOG_INFO << "img load fail!";
        this->v_close();
        this->_maix_image_size = 0;
      }
      return *this;
    }
#endif
  }
  return *this;
}

maix_image &maix_image::_open_file(std::string path)
{
  // // LOG_INFO << "clear img" ;
  this->v_close();
  // // LOG_INFO << "call libmaix_cv_image_open_file img" ;
  libmaix_image_t *tmp_img = NULL;
  if (libmaix_cv_image_open_file(&tmp_img, path.c_str()) != 0)
  {
    this->v_close();
    // LOG_INFO << "open file err! " << path << "exec or don't read !";
    // std::cout << "open file err! " << path << "exec or don't read !" << std::endl;
    return *this;
  }
  this->_img = tmp_img;
  // // LOG_INFO << "call libmaix_cv_image_open_file success!";
  this->_maix_image_width = this->_img->width;
  this->_maix_image_height = this->_img->height;
  this->_maix_image_type = "RGB";
  this->_maix_image_size = this->_maix_image_width * this->_maix_image_height * 3;
  return *this;
}

py::object maix_image::_get_to(std::string im)
{
  if (im == "maix_image")
  {
    auto _maix_Image_ = py::module::import("maix_image");
    auto py_Image = _maix_Image_.attr("Image");
    auto py_img_obj = py_Image();
    auto py_fun_load = py_img_obj.attr("load");
    std::string tmp((const char *)this->_img->data, this->_maix_image_size);
    py_fun_load(tmp, std::vector<int>{this->_maix_image_width, this->_maix_image_height}, this->_maix_image_type);
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
      std::string tmp((const char *)this->_img->data, this->_maix_image_size);
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
}

maix_image &maix_image::_clear()
{
  // // LOG_INFO << "img clear!";
  v_close();
}

void maix_image::_show()
{
  if (this->_img == NULL)
  {
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
  libmaix_image_t *tmp = libmaix_image_create(this->_img->width, this->_img->height, any_cast<libmaix_image_mode_t>(py_to_pram[2][0]), LIBMAIX_IMAGE_LAYOUT_HWC, NULL, true);
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
      cv::Mat src(tmp->width, tmp->height, any_cast<int>(py_to_pram[2][2]), tmp->data);
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
    cv::Mat src(this->_img->width, this->_img->height, any_cast<int>(py_to_pram[2][2]), this->_img->data);
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
  if (this->_img)
  {
    libmaix_cv_image_draw_image_save(this->_img, file_path.c_str());
  }
  return 0;
}

py::bytes maix_image::_tobytes()
{
  if (this->_img)
  {
    return py::bytes((const char *)this->_img->data, this->_maix_image_size);
  }
  py::bytes return_val;
  return return_val;
}

maix_image &maix_image::_resize(int w, int h)
{
  if (this->_img && this->_img->width != w && this->_img->height != h)
  {
    libmaix_image_t *tmp = libmaix_image_create(w, h, this->_img->mode, LIBMAIX_IMAGE_LAYOUT_HWC, NULL, true);
    if (tmp)
    {
      cv::Mat src(this->_img->width, this->_img->height, any_cast<int>(py_to_pram[this->get_to(this->_maix_image_type)][2]), this->_img->data);
      cv::Mat dst(w, h, any_cast<int>(py_to_pram[this->get_to(this->_maix_image_type)][2]), tmp->data);
      cv::resize(src, dst, cv::Size(w, h));
      libmaix_image_destroy(&this->_img), this->_img = tmp;
      this->_maix_image_height = h;
      this->_maix_image_width = w;
      this->_maix_image_size = w * h * any_cast<int>(py_to_pram[this->get_to(this->_maix_image_type)][1]);
    }
    else
    {
      libmaix_image_destroy(&tmp);
    }
  }
  return *this;
}

maix_image &maix_image::_draw_line(int x1, int y1, int x2, int y2, std::vector<int> color, int thickness)
{
  if (this->_img)
  {
    libmaix_cv_image_draw_line(this->_img, x1, y1, x2, y2, MaixColor(color[0], color[1], color[2]), thickness);
  }
  return *this;
}

maix_image &maix_image::_draw_rectangle(int x, int y, int w, int h, std::vector<int> color, int thickness)
{
  // // LOG_INFO << "start";
  if (this->_img)
  {
    libmaix_cv_image_draw_rectangle(this->_img, x, y, w, h, MaixColor(color[0], color[1], color[2]), thickness);
    // // LOG_INFO << "success";
  }
  return *this;
}

maix_image &maix_image::_draw_circle(int x, int y, int r, std::vector<int> color, int thickness)
{
  if (this->_img)
  {
    libmaix_cv_image_draw_circle(this->_img, x, y, r, MaixColor(color[0], color[1], color[2]), thickness);
  }
  return *this;
}
maix_image &maix_image::_draw_ellipse(int cx, int cy, int rx, int ry, double angle, double startAngle, double endAngle, std::vector<int> color, int thickness)
{
  if (this->_img)
  {
    libmaix_cv_image_draw_ellipse(this->_img, cx, cy, rx, ry, angle, startAngle, endAngle, MaixColor(color[0], color[1], color[2]), thickness);
  }
  return *this;
}

maix_image &maix_image::_draw_string(int x, int y, std::string str, double scale, std::vector<int> color, int thickness)
{
  if (this->_img)
  {
    libmaix_cv_image_draw_string(this->_img, x, y, str.c_str(), scale, MaixColor(color[0], color[1], color[2]), thickness);
  }
  return *this;
}

maix_image &maix_image::_rotate(double rotate)
{
  if (this->_img)
  {
    libmaix_image_t *tmp = libmaix_image_create(this->_img->width, this->_img->height, this->_img->mode, LIBMAIX_IMAGE_LAYOUT_HWC, NULL, true);
    if (tmp)
    {
      libmaix_cv_image_rotate(this->_img, int(rotate), &tmp);
      libmaix_image_destroy(&this->_img), this->_img = tmp;
    }
    else
    {
      libmaix_image_destroy(&tmp);
    }
  }
  return *this;
}

maix_image &maix_image::_convert(std::string mode)
{
  if (this->_img)
  {
    libmaix_image_t *tmp = libmaix_image_create(this->_img->width, this->_img->height, any_cast<libmaix_image_mode_t>(py_to_pram[this->get_to(mode)][0]), LIBMAIX_IMAGE_LAYOUT_HWC, NULL, true);
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
  }
  else
  {
    // std::cout << "please load image!" << std::endl;
  }
  return *this;
}

maix_image &maix_image::_draw_crop(int x, int y, int w, int h)
{
  if (this->_img)
  {
    libmaix_image_t *tmp = libmaix_image_create(w, h, this->_img->mode, LIBMAIX_IMAGE_LAYOUT_HWC, NULL, true);
    if (tmp)
    {
      libmaix_cv_image_crop(this->_img, x, y, w, h, &tmp);
      libmaix_image_destroy(&this->_img), this->_img = tmp;
      this->_maix_image_width = this->_img->width;
      this->_maix_image_height = this->_img->height;
    }
    else
    {
      libmaix_image_destroy(&tmp);
    }
  }
  return *this;
}
std::vector<int> maix_image::_get_pixe(int x, int y)
{
  std::vector<int> color_val;
  if (this->_img)
  {
    libmaix_image_color_t colot_tmp = libmaix_cv_image_get_pixe(this->_img, x, y);
    color_val.push_back(colot_tmp.rgb888.r);
    color_val.push_back(colot_tmp.rgb888.g);
    color_val.push_back(colot_tmp.rgb888.b);
    color_val.push_back(colot_tmp.rgb888.a);
    return color_val;
  }
  return color_val;
}

// py::object img_open(std::string path, std::string format)
// {
//   auto _maix_Image_ = py::module::import("maix_image");
//   auto py_Image = _maix_Image_.attr("Image");
//   auto py_img_obj = py_Image();
//   auto py_fun_open = py_img_obj.attr("open_file");
//   py_fun_open(path);
//   return py_img_obj;
// }

// py::object img_new(std::vector<int> size, std::vector<int> color, std::string mode)
// {
//   // // LOG_INFO << "new img";
//   //pybind11 python端的实现方法
//   auto _maix_Image_ = py::module::import("_maix_image");
//   auto py_Image = _maix_Image_.attr("Image");
//   auto py_img_obj = py_Image();
//   auto py_fun_new = py_img_obj.attr("new");
//   py_fun_new(size, color, mode);
//   return py_img_obj;
//   // pybind11 C++端的实现方法
//   // maix_image *img_val = NULL;
//   // img_val = new maix_image();
//   // img_nu.push_back(img_val);
//   // img_val->_new(size, color, mode);
//   // return img_val;
// }

// py::object img_load(py::bytes data, std::vector<int> size, std::string mode)
// {
//   auto _maix_Image_ = py::module::import("maix_image");
//   auto py_Image = _maix_Image_.attr("Image");
//   auto py_img_obj = py_Image();
//   auto py_fun_load = py_img_obj.attr("load");
//   py_fun_load(data, size, mode);
//   return py_img_obj;
// }

// static void s_log(int nu)
// {
//   switch (nu)
//   {
//   case 1:
//     nanolog::set_log_level(nanolog::LogLevel::INFO);
//     break;
//   case 2:
//     nanolog::set_log_level(nanolog::LogLevel::WARN);
//     break;
//   case 3:
//     nanolog::set_log_level(nanolog::LogLevel::CRIT);
//     break;
//   default:
//     break;
//   }
// }

// static void mode_init()
// {
//   nanolog::initialize(nanolog::GuaranteedLogger(), "/var/maixpy3/", "_maix_image", 1);
//   nanolog::set_log_level(nanolog::LogLevel::CRIT);
// }

PYBIND11_MODULE(_maix_image, mo)
{
  // mode_init();
  // mo.def("set_log", &s_log, py::arg("level") = 3);
  // mo.def("open", &img_open, py::arg("path"), py::arg("format") = "RGB");
  // mo.def("new", &img_new, py::arg("size") = std::vector<int>{240, 240}, py::arg("color") = std::vector<int>{0, 0, 0}, py::arg("mode") = "RGB");
  // mo.def("load", &img_load, py::arg("data"), py::arg("size") = std::vector<int>{240, 240}, py::arg("mode") = "RGB");

  pybind11::class_<maix_image>(mo, "Image")
      .def(pybind11::init<>())

      //Image 的一些属性
      .def_readonly("width", &maix_image::_maix_image_width)
      .def_readonly("height", &maix_image::_maix_image_height)
      .def_readonly("mode", &maix_image::_maix_image_type)
      .def_readonly("size", &maix_image::_maix_image_size)

      //Image重载自己python类的方法
      .def("__str__", &maix_image::str__)
      .def("__repr__", &maix_image::repr__)
      .def("__len__", &maix_image::len__)

      //Image自己的方法
      .def("new", &maix_image::_new, py::arg("size") = std::vector<int>{240, 240}, py::arg("color") = std::vector<int>{0, 0, 0}, py::arg("mode") = "RGB")
      .def("load", &maix_image::_load, py::arg("data"), py::arg("size") = std::vector<int>{240, 240}, py::arg("mode") = "RGB")
      .def("open", &maix_image::_open_file)
      .def("copy", &maix_image::_get_to, py::arg("img") = "maix_image")
      .def("clear", &maix_image::_clear)
      .def("save", &maix_image::_save, py::arg("path"), py::arg("format") = "jpeg")
      .def("tobytes", &maix_image::_tobytes)
      .def("resize", &maix_image::_resize, py::arg("w"), py::arg("h"))
      .def("draw_line", &maix_image::_draw_line, py::arg("x1"), py::arg("y1"), py::arg("x2"), py::arg("y2"), py::arg("color") = std::vector<int>{127, 127, 127}, py::arg("thickness") = 1)
      .def("draw_rectangle", &maix_image::_draw_rectangle, py::arg("x"), py::arg("y"), py::arg("w"), py::arg("h"), py::arg("color") = std::vector<int>{127, 127, 127}, py::arg("thickness") = 1)
      .def("draw_circle", &maix_image::_draw_circle, py::arg("x"), py::arg("y"), py::arg("radius"), py::arg("color") = std::vector<int>{127, 127, 127}, py::arg("thickness") = 1)
      .def("draw_ellipse", &maix_image::_draw_ellipse, py::arg("cx"), py::arg("xy"), py::arg("rx"), py::arg("ry"), py::arg("angle"), py::arg("startAngle"), py::arg("endAngle"), py::arg("color") = std::vector<int>{127, 127, 127}, py::arg("thickness") = 1)
      .def("draw_string", &maix_image::_draw_string, py::arg("x"), py::arg("y"), py::arg("str"), py::arg("scale") = 1.0, py::arg("color") = std::vector<int>{127, 127, 127}, py::arg("thickness") = 1)
      .def("rotate", &maix_image::_rotate, py::arg("rotate"))
      .def("convert", &maix_image::_convert, py::arg("mode") = "RGB")
      .def("crop", &maix_image::_draw_crop, py::arg("x"), py::arg("y"), py::arg("w"), py::arg("h"))
      .def("show", &maix_image::_show)
      .def("get_pixel", &maix_image::_get_pixe, py::arg("x"), py::arg("y"))
      // .def("load_freetype", &maix_image::test)

      //Image继承 version 的方法
      //基于opencv编写MaixPy3特有函数
      .def("get_blob_lab", &maix_image::get_blob_color_max, py::arg("roi") = std::vector<int>{0, 0, 0, 0}, py::arg("critical") = 0, py::arg("co") = 0)
      .def("get_blob_color", &maix_image::get_blob_color_max, py::arg("roi") = std::vector<int>{0, 0, 0, 0}, py::arg("critical") = 0, py::arg("co") = 0)
      .def("find_blob_lab", &maix_image::_maix_vision_find_blob, py::arg("thresholds"), py::arg("roi") = std::vector<int>{0, 0, 0, 0}, py::arg("x_stride") = 2, py::arg("y_stride") = 2, py::arg("invert") = 0, py::arg("area_threshold") = 10, py::arg("pixels_threshold") = 10, py::arg("merge") = 0, py::arg("margin") = 0, py::arg("tilt") = 0, py::arg("co") = 1)
      .def("find_ball_lab", &maix_image::_maix_vision_find_ball_blob, py::arg("thresholds"), py::arg("co") = 1)
      .def("find_circles_blob", &maix_image::_maix_vision_find_ball_blob, py::arg("thresholds"), py::arg("co") = 1)
      .def("find_line", &maix_image::find_line)

      //基于opencv编写兼容openmv图像处理函数
      // .def("test", &maix_image::version_test)
      .def("Canny", &maix_image::_maix_vision_Canny, py::arg("thr_h"), py::arg("thr_l")) // find_edges
      .def("find_blobs", &maix_image::_maix_vision_find_blob, py::arg("thresholds"), py::arg("roi") = std::vector<int>{0, 0, 0, 0}, py::arg("x_stride") = 2, py::arg("y_stride") = 2, py::arg("invert") = 0, py::arg("area_threshold") = 10, py::arg("pixels_threshold") = 10, py::arg("merge") = 0, py::arg("margin") = 0, py::arg("tilt") = 0, py::arg("co") = 1)

      //Image继承 image_dsp 的方法

      //Image继承 nn的一些方法(待定)
      ;
}
