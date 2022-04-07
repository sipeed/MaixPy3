#include "maix_image.h"
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
//   nanolog::set_log_level(nanolog::LogLevel::INFO);
// }

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

PYBIND11_MODULE(_maix_image, mo)
{
    // mode_init();         //模块的初始化函数
    mo.def("load_freetype", _load_freetype, py::arg("path"), py::arg("fontHeight") = 14)
      .def("free_freetype", _free_freetype)
      .def("get_string_size", _get_string_size, py::arg("str"), py::arg("scale") = 1.0, py::arg("thickness") = 1);

    pybind11::class_<maix_image>(mo, "Image")
        .def(pybind11::init<>())
        //image属性
        .def_readonly("width", &maix_image::_maix_image_width)
        .def_readonly("height", &maix_image::_maix_image_height)
        .def_readonly("mode", &maix_image::_maix_image_type)
        .def_readonly("size", &maix_image::_maix_image_size)
        //python模块重载函数
        .def("__str__", &maix_image::str__)
        .def("__repr__", &maix_image::repr__)
        .def("__len__", &maix_image::len__)
        //maix_image C++类数据传递
        .def("c_img_private", &maix_image::img_pointer)
        //maix_image类的基本方法
        .def("new", &maix_image::_new, py::arg("size") = std::vector<int>{240, 240}, py::arg("color") = std::vector<int>{0, 0, 0}, py::arg("mode") = "RGB")
        .def("load", &maix_image::_load, py::arg("data"), py::arg("size") = std::vector<int>{240, 240}, py::arg("mode") = "RGB")
        .def("open", &maix_image::_open_file)
        .def("copy", &maix_image::_to_py, py::arg("img") = "maix_image")
        .def("clear", &maix_image::_clear)
        .def("delete", &maix_image::_delete)
        .def("save", &maix_image::_save, py::arg("path"), py::arg("format") = "jpeg")
        .def("tobytes", &maix_image::_tobytes)
        .def("resize", &maix_image::_resize, py::arg("w"), py::arg("h"), py::arg("func") = 1)
        .def("draw_line", &maix_image::_draw_line, py::arg("x1"), py::arg("y1"), py::arg("x2"), py::arg("y2"), py::arg("color") = std::vector<int>{127, 127, 127}, py::arg("thickness") = 1)
        .def("draw_rectangle", &maix_image::_draw_rectangle, py::arg("x"), py::arg("y"), py::arg("w"), py::arg("h"), py::arg("color") = std::vector<int>{127, 127, 127}, py::arg("thickness") = 1)
        .def("draw_circle", &maix_image::_draw_circle, py::arg("x"), py::arg("y"), py::arg("radius"), py::arg("color") = std::vector<int>{127, 127, 127}, py::arg("thickness") = 1)
        .def("draw_ellipse", &maix_image::_draw_ellipse, py::arg("cx"), py::arg("xy"), py::arg("rx"), py::arg("ry"), py::arg("angle"), py::arg("startAngle"), py::arg("endAngle"), py::arg("color") = std::vector<int>{127, 127, 127}, py::arg("thickness") = 1)
        .def("draw_string", &maix_image::_draw_string, py::arg("x"), py::arg("y"), py::arg("str"), py::arg("scale") = 1.0, py::arg("color") = std::vector<int>{127, 127, 127}, py::arg("thickness") = 1)
        .def("rotate", &maix_image::_rotate, py::arg("rotate"), py::arg("adjust") = 0)
        .def("convert", &maix_image::_convert, py::arg("mode") = "RGB")
        .def("crop", &maix_image::_draw_crop, py::arg("x"), py::arg("y"), py::arg("w"), py::arg("h"))
        .def("draw_image", &maix_image::_draw_image, py::arg("img"), py::arg("x") = 0, py::arg("y") = 0, py::arg("alpha") = -1.)
        .def("show", &maix_image::_show)
        .def("get_pixel", &maix_image::_get_pixel, py::arg("x"), py::arg("y"))
        .def("set_pixel", &maix_image::_set_pixel, py::arg("x"), py::arg("y"), py::arg("color"))
        //maix_image继承maix_version方法
        .def("get_blob_lab", &maix_image::get_blob_color_max, py::arg("roi") = std::vector<int>{0, 0, 0, 0}, py::arg("critical") = 0, py::arg("co") = 0)
        .def("get_blob_color", &maix_image::get_blob_color_max, py::arg("roi") = std::vector<int>{0, 0, 0, 0}, py::arg("critical") = 0, py::arg("co") = 0)
        .def("find_blob_lab", &maix_image::_maix_vision_find_blob, py::arg("thresholds"), py::arg("roi") = std::vector<int>{0, 0, 0, 0}, py::arg("x_stride") = 2, py::arg("y_stride") = 2, py::arg("invert") = 0, py::arg("area_threshold") = 10, py::arg("pixels_threshold") = 10, py::arg("merge") = 0, py::arg("margin") = 0, py::arg("tilt") = 0, py::arg("co") = 1)
        .def("find_ball_color", &maix_image::_maix_vision_find_ball_blob, py::arg("thresholds"), py::arg("co") = 1)
        .def("find_circles_blob", &maix_image::_maix_vision_find_ball_blob, py::arg("thresholds"), py::arg("co") = 1)
        .def("find_line", &maix_image::find_line, py::arg("func") = 0)
        .def("find_blobs", &maix_image::_maix_vision_find_blob, py::arg("thresholds"), py::arg("roi") = std::vector<int>{0, 0, 0, 0}, py::arg("x_stride") = 2, py::arg("y_stride") = 2, py::arg("invert") = 0, py::arg("area_threshold") = 10, py::arg("pixels_threshold") = 10, py::arg("merge") = 0, py::arg("margin") = 0, py::arg("tilt") = 0, py::arg("co") = 1)
        .def("custom_find_ball_blob", &maix_image::_find_ball_blob, py::arg("thresholds"), py::arg("roi") = std::vector<int>{0, 0, 0, 0}, py::arg("x_stride") = 2, py::arg("y_stride") = 2, py::arg("invert") = 0, py::arg("area_threshold") = 10, py::arg("pixels_threshold") = 10, py::arg("merge") = 0, py::arg("margin") = 0, py::arg("tilt") = 0, py::arg("h_min") = 1, py::arg("w_min") = 1, py::arg("co") = 1)
        // maix_image &_imlib_rotation_corr(float x_rotation, float y_rotation, float z_rotation, float x_translation, float y_translation, float zoom, float fov, std::vector<std::vector<float>> corners);
        .def("imlib_rotation_corr", &maix_image::_imlib_rotation_corr, py::arg("x_rotation") = 0.0, py::arg("y_rotation") = 0.0, py::arg("z_rotation") = 0.0,
          py::arg("x_translation") = 0.0, py::arg("y_translation") = 0.0,
          py::arg("zoom") = 1.0, py::arg("fov") = 60.0, py::arg("corners")=std::vector<std::vector<float>>{std::vector<float>{0, 0}, std::vector<float>{0, 0}})
        .def("gaussian",&maix_image::_gaussian,py::arg("size")=1,py::arg("unsharp")=0,py::arg("mul")=1.0,py::arg("add")=0.0,py::arg("invert")=0)
        .def("histeq",&maix_image::_hist_eq,py::arg("adaptive")=0,py::arg("clip_limit")=-1.0,py::arg("mask") = maix_image())
        .def("gamma_corr",&maix_image::_gamma_corr,py::arg("gamma")=1.0,py::arg("contrast")=1.0,py::arg("brightness")=0.0)
        ; // module end
}
