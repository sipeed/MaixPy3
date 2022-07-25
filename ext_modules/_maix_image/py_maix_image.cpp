#include "maix_image.h"

#include "cv_orb.hpp"

void _load_freetype(std::string path, int fontHeight)
{
  libmaix_cv_image_load_freetype(path.c_str(), fontHeight);
}

void _free_freetype()
{
  libmaix_cv_image_free_freetype();
}

py::tuple _get_string_size(std::string str, double scale, int thickness)
{
  int w = 0, h = 0;
  libmaix_cv_image_get_string_size(&w, &h, str.c_str(), scale, thickness);
  return py::make_tuple(w, h);
}

maix_image *_image_new(std::vector<int> size, std::vector<int> color, std::string mode)
{
  auto tmp = new maix_image();
  tmp->_new(size, color, mode);
  return tmp;
}

maix_image *_image_load(py::object data, std::vector<int> size, std::string mode)
{
  auto tmp = new maix_image();
  tmp->_load(data, size, mode);
  return tmp;
}

maix_image *_image_open(std::string path)
{
  auto tmp = new maix_image();
  tmp->_open_file(path);
  return tmp;
}

PYBIND11_MODULE(_maix_image, mo)
{
  // mode_init();         //模块的初始化函数
  mo.def("load_freetype", _load_freetype, py::arg("path"), py::arg("fontHeight") = 14)
      .def("free_freetype", _free_freetype)
      .def("get_string_size", _get_string_size, py::arg("str"), py::arg("scale") = 1.0, py::arg("thickness") = 1)
      .def("new", _image_new, py::arg("size") = std::vector<int>{240, 240}, py::arg("color") = std::vector<int>{0, 0, 0}, py::arg("mode") = "RGB")
      .def("load", _image_load, py::arg("data"), py::arg("size") = std::vector<int>{240, 240}, py::arg("mode") = "RGB")
      .def("open", _image_open, py::arg("str"));

  pybind11::class_<maix_image::maix_histogram>(mo, "histogram")
      .def(pybind11::init<>())
      /// Bare bones interface
      .def("__getitem__",
           [](const maix_image::maix_histogram &s, size_t i)
           {
             if (i > 3)
             {
               throw py::index_error();
             }
             return s.lab_bins[i];
           })
      .def("__setitem__",
           [](maix_image::maix_histogram &s, size_t i, std::vector<float> v)
           {
             if (i > 3)
             {
               throw py::index_error();
             }
             s.lab_bins[i] = v;
           })
      .def("bins", &maix_image::maix_histogram::bins)
      .def("l_bins", &maix_image::maix_histogram::l_bins)
      .def("a_bins", &maix_image::maix_histogram::a_bins)
      .def("b_bins", &maix_image::maix_histogram::b_bins)
      .def("get_threshold", &maix_image::maix_histogram::get_threshold)
      .def("get_statistics", &maix_image::maix_histogram::get_statistics)
      .def("get_percentile", &maix_image::maix_histogram::get_percentile);

  pybind11::class_<maix_image>(mo, "Image")
      .def(pybind11::init<>())
      // image属性
      .def_readonly("width", &maix_image::_maix_image_width)
      .def_readonly("height", &maix_image::_maix_image_height)
      .def_readonly("mode", &maix_image::_maix_image_type)
      .def_readonly("size", &maix_image::_maix_image_size)
      // python模块重载函数
      .def("__str__", &maix_image::str__)
      .def("__repr__", &maix_image::repr__)
      .def("__len__", &maix_image::len__)
      // maix_image C++类数据传递
      .def("c_img_private", &maix_image::img_pointer)
      // maix_image类的基本方法
      .def("new", &maix_image::_new, py::arg("size") = std::vector<int>{240, 240}, py::arg("color") = std::vector<int>{0, 0, 0}, py::arg("mode") = "RGB")
      .def("load", &maix_image::_load, py::arg("data"), py::arg("size") = std::vector<int>{240, 240}, py::arg("mode") = "RGB")
      .def("open", &maix_image::_open_file)
      .def("copy", &maix_image::_to_py, py::arg("img") = "maix_image")
      .def("clear", &maix_image::_clear)
      .def("delete", &maix_image::_delete)
      .def("save", &maix_image::_save, py::arg("path"), py::arg("format") = "jpeg")
      .def("tobytes", &maix_image::_tobytes, py::arg("format") = "rgb", py::arg("params") = std::vector<int>{})
      .def("resize", &maix_image::_resize, py::arg("w") = 0, py::arg("h") = 0, py::arg("func") = 1, py::arg("padding") = 1, py::arg("size") = std::vector<int>{0, 0})
      .def("draw_line", &maix_image::_draw_line, py::arg("x1"), py::arg("y1"), py::arg("x2"), py::arg("y2"), py::arg("color") = std::vector<int>{127, 127, 127}, py::arg("thickness") = 1)
      .def("draw_cross", &maix_image::_draw_cross, py::arg("x"), py::arg("y"), py::arg("c"), py::arg("size") = 5, py::arg("thickness") = 1)
      .def("draw_rectangle", &maix_image::_draw_rectangle, py::arg("x1_x"), py::arg("y1_y"), py::arg("x2_w"), py::arg("y2_h"), py::arg("color") = std::vector<int>{127, 127, 127}, py::arg("thickness") = 1, py::arg("is_xywh") = 0)
      .def("draw_circle", &maix_image::_draw_circle, py::arg("x"), py::arg("y"), py::arg("radius"), py::arg("color") = std::vector<int>{127, 127, 127}, py::arg("thickness") = 1)
      .def("draw_ellipse", &maix_image::_draw_ellipse, py::arg("cx"), py::arg("xy"), py::arg("rx"), py::arg("ry"), py::arg("angle"), py::arg("startAngle"), py::arg("endAngle"), py::arg("color") = std::vector<int>{127, 127, 127}, py::arg("thickness") = 1)
      .def("draw_string", &maix_image::_draw_string, py::arg("x"), py::arg("y"), py::arg("str"), py::arg("scale") = 1.0, py::arg("color") = std::vector<int>{127, 127, 127}, py::arg("thickness") = 1)
      .def("rotate", &maix_image::_rotate, py::arg("angle"), py::arg("adjust") = 0)
      .def("flip", &maix_image::_flip, py::arg("flip") = 1)
      .def("convert", &maix_image::_convert, py::arg("mode") = "RGB")
      .def("cut", &maix_image::_draw_crop, py::arg("x"), py::arg("y"), py::arg("w"), py::arg("h"))
      .def("crop", &maix_image::_draw_crop, py::arg("x"), py::arg("y"), py::arg("w"), py::arg("h"))
      .def("draw_image", &maix_image::_draw_image, py::arg("img"), py::arg("x") = 0, py::arg("y") = 0, py::arg("alpha") = -1.)
      .def("paste", &maix_image::_draw_image, py::arg("img"), py::arg("x") = 0, py::arg("y") = 0, py::arg("alpha") = -1.)
      .def("show", &maix_image::_show)
      .def("get_pixel", &maix_image::_get_pixel, py::arg("x"), py::arg("y"))
      .def("set_pixel", &maix_image::_set_pixel, py::arg("x"), py::arg("y"), py::arg("color"))
      .def("Canny", &maix_image::_opencv_Canny, py::arg("threshold1") = 10, py::arg("threshold2") = 100, py::arg("apertureSize") = 3, py::arg("L2gradient") = false)
      .def("crop_affine",&maix_image::_warp_affine, py::arg("points"), py::arg("w"), py::arg("h"))

      // maix_image继承maix_vision方法
      .def("get_blob_lab", &maix_image::get_blob_color_max, py::arg("roi") = std::vector<int>{0, 0, 0, 0}, py::arg("critical") = 0, py::arg("co") = 0)
      .def("get_blob_color", &maix_image::get_blob_color_max, py::arg("roi") = std::vector<int>{0, 0, 0, 0}, py::arg("critical") = 0, py::arg("co") = 0)
      .def("find_ball_color", &maix_image::_maix_vision_find_ball_blob, py::arg("thresholds"), py::arg("co") = 1)
      .def("find_circles_blob", &maix_image::_maix_vision_find_ball_blob, py::arg("thresholds"), py::arg("co") = 1)
      .def("find_line", &maix_image::find_line, py::arg("func") = 0)
      .def("find_blobs_cv", &maix_image::_maix_vision_find_blob, py::arg("thresholds"), py::arg("roi") = std::vector<int>{0, 0, 0, 0}, py::arg("x_stride") = 2, py::arg("y_stride") = 2, py::arg("invert") = 0, py::arg("area_threshold") = 10, py::arg("pixels_threshold") = 10, py::arg("merge") = 0, py::arg("margin") = 0, py::arg("tilt") = 0, py::arg("co") = 1)
      .def("custom_find_ball_blob", &maix_image::_find_ball_blob, py::arg("thresholds"), py::arg("roi") = std::vector<int>{0, 0, 0, 0}, py::arg("x_stride") = 2, py::arg("y_stride") = 2, py::arg("invert") = 0, py::arg("area_threshold") = 10, py::arg("pixels_threshold") = 10, py::arg("merge") = 0, py::arg("margin") = 0, py::arg("tilt") = 0, py::arg("h_min") = 1, py::arg("w_min") = 1, py::arg("co") = 1)
      // void imlib_get_histogram(histogram_t *out, image_t *ptr, rectangle_t *roi, list_t *thresholds, bool invert, image_t *other);
      .def("get_histogram", &maix_image::_imlib_get_histogram, py::arg("roi") = std::vector<int>{0, 0, 0, 0},
           py::arg("thresholds") = std::vector<std::vector<int>>{}, py::arg("invert") = false, py::arg("difference") = maix_image(),
           py::arg("bins") = -1, py::arg("l_bins") = -1, py::arg("a_bins") = -1, py::arg("b_bins") = -1)
      .def("get_statistics", &maix_image::_imlib_get_statistics, py::arg("roi") = std::vector<int>{0, 0, 0, 0},
           py::arg("thresholds") = std::vector<std::vector<int>>{}, py::arg("invert") = false, py::arg("difference") = maix_image(),
           py::arg("bins") = -1, py::arg("l_bins") = -1, py::arg("a_bins") = -1, py::arg("b_bins") = -1)
      // maix_image &_imlib_rotation_corr(float x_rotation, float y_rotation, float z_rotation, float x_translation, float y_translation, float zoom, float fov, std::vector<std::vector<float>> corners);
      .def("rotation_corr", &maix_image::_imlib_rotation_corr, py::arg("x_rotation") = 0.0, py::arg("y_rotation") = 0.0, py::arg("z_rotation") = 0.0,
           py::arg("x_translation") = 0.0, py::arg("y_translation") = 0.0,
           py::arg("zoom") = 1.0, py::arg("fov") = 60.0, py::arg("corners") = std::vector<std::vector<float>>{std::vector<float>{0, 0}, std::vector<float>{0, 0}})
      .def("histeq", &maix_image::_hist_eq, py::arg("adaptive") = 0, py::arg("clip_limit") = -1.0, py::arg("mask") = maix_image())
      // .def("gamma_corr",&maix_image::_gamma_corr,py::arg("gamma")=1.0,py::arg("contrast")=1.0,py::arg("brightness")=0.0)
      .def("lens_corr", &maix_image::_lens_corr, py::arg("strength") = 1.8, py::arg("zoom") = 1.0, py::arg("x_corr") = 0.0, py::arg("y_corr") = 0.0)
      .def("mean", &maix_image::_mean, py::arg("ksize") = 1, py::arg("threshold") = 0, py::arg("offset") = 0, py::arg("invert") = 0, py::arg("mask") = maix_image())
      .def("find_rects", &maix_image::_imlib_find_rects, py::arg("roi") = std::vector<int>{0, 0, 0, 0}, py::arg("threshold"), py::arg("is_xywh") = 0)
      .def("find_lines", &maix_image::_imlib_find_lines, py::arg("roi") = std::vector<int>{0, 0, 0, 0}, py::arg("x_stride") = 2, py::arg("y_stride") = 1, py::arg("threshold") = 1000, py::arg("theta_margin") = 25, py::arg("rho_margin") = 25)
      .def("find_circles", &maix_image::_imlib_find_circles, py::arg("roi") = std::vector<int>{0, 0, 0, 0}, py::arg("x_stride") = 2, py::arg("y_stride") = 1, py::arg("threshold") = 2000, py::arg("x_margin") = 10, py::arg("y_margin") = 10, py::arg("r_margin") = 10, py::arg("r_min") = 2, py::arg("r_max") = 0, py::arg("r_step") = 2)
      .def("find_line_segments", &maix_image::_imlib_find_line_segments, py::arg("roi") = std::vector<int>{0, 0, 0, 0}, py::arg("merge_distance") = 0, py::arg("max_theta_diff") = 15)
      .def("find_apriltags", &maix_image::_imlib_find_apriltags, py::arg("roi") = std::vector<int>{0, 0, 0, 0}, py::arg("families") = 16, py::arg("fx") = 0, py::arg("fy") = 0, py::arg("cx") = 0, py::arg("cy") = 0)
      .def("find_qrcodes", &maix_image::_imlib_find_qrcodes, py::arg("roi") = std::vector<int>{0, 0, 0, 0})
      .def("find_blobs", &maix_image::_imlib_find_blobs, py::arg("thresholds"), py::arg("roi") = std::vector<int>{0, 0, 0, 0}, py::arg("x_stride") = 2, py::arg("y_stride") = 2, py::arg("invert") = 0, py::arg("area_threshold") = 10, py::arg("pixels_threshold") = 10, py::arg("merge") = 0, py::arg("margin") = 0, py::arg("x_hist_bins_max") = 0, py::arg("y_hist_bins_max") = 0)
      .def("find_template", &maix_image::_imlib_find_template, py::arg("template") = maix_image(), py::arg("thresh") = 0.5, py::arg("roi") = std::vector<int>{0, 0, 0, 0}, py::arg("step") = 2, py::arg("search") = 1)
      .def("find_barcodes", &maix_image::_imlib_find_barcodes, py::arg("roi") = std::vector<int>{0, 0, 0, 0}); // module end

  pybind11::class_<cv_orb>(mo, "orb")
      .def(pybind11::init<int, float, int, int, int, int, int, int, int>(),
           py::arg("nfeatures") = 500, py::arg("scaleFactor") = 1.2f, py::arg("nlevels") = 8, py::arg("edgeThreshold") = 31, py::arg("firstLevel") = 0, py::arg("WTA_K") = 2, py::arg("scoreType") = 0, py::arg("patchSize") = 31, py::arg("fastThreshold") = 20)
      .def("match", &cv_orb::match, pybind11::arg("src"), pybind11::arg("dst"), pybind11::arg("limit") = 30, pybind11::arg("min") = 0.0f, pybind11::arg("max") = 100.f, pybind11::arg("crossCheck") = false, pybind11::arg("dump") = 0);
}
