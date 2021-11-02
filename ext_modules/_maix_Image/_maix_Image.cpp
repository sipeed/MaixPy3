#include "maix_Image.h"

int _maix_image::get_to(string &mode)
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

void _maix_image::v_init()
{
    this->img = NULL;
}
void _maix_image::v_close()
{
    if (img)
    {
        libmaix_image_destroy(&img);
    }
}

_maix_image::_maix_image()
{

    this->py_to_pram[0][0] = LIBMAIX_IMAGE_MODE_GRAY;
    this->py_to_pram[0][1] = 1;
    this->py_to_pram[0][2] = CV_8UC1;

    this->py_to_pram[1][0] = LIBMAIX_IMAGE_MODE_RGB565;
    this->py_to_pram[1][1] = 2;
    this->py_to_pram[1][2] = CV_8UC2;

    this->py_to_pram[2][0] = LIBMAIX_IMAGE_MODE_RGB888;
    this->py_to_pram[2][1] = 3;
    this->py_to_pram[2][2] = CV_8UC3;

    this->py_to_pram[3][0] = LIBMAIX_IMAGE_MODE_RGBA8888;
    this->py_to_pram[3][1] = 4;
    this->py_to_pram[3][2] = CV_8UC4;

    this->v_init();
}

_maix_image::~_maix_image()
{
    this->v_close();
}
std::string _maix_image::str__()
{
    std::string info_str;
    std::stringstream ss;
    ss << "{\"w\":" << this->_maix_image_width << ", \"h\":" << this->_maix_image_height << ", \"type\"=" << this->_maix_image_type << ", \"size\":" << this->_maix_image_size << "}";
    ss >> info_str;
    return info_str;
}

_maix_image &_maix_image::_new(std::vector<int> size, std::vector<int> color, std::string mode)
{
    this->v_close();
    this->_maix_image_type = mode;
    this->_maix_image_width = size[0];
    this->_maix_image_height = size[1];
    this->_maix_image_size = this->_maix_image_width * this->_maix_image_height * any_cast<int>(py_to_pram[this->get_to(this->_maix_image_type)][1]);
    this->img = libmaix_image_create(size[0], size[1], any_cast<libmaix_image_mode_t>(py_to_pram[this->get_to(this->_maix_image_type)][0]), LIBMAIX_IMAGE_LAYOUT_HWC, NULL, true);
    if (this->img)
    {
        libmaix_cv_image_draw_rectangle(this->img, 0, 0, size[0], size[1], MaixColor(color[0], color[1], color[2]), -1);
    }
    else
    {
        this->v_close();
        this->_maix_image_size = 0;
    }
    return *this;
}

_maix_image &_maix_image::_load(py::object &data, std::vector<int> size, std::string mode)
{

    this->v_close();
    this->_maix_image_type = mode;
    this->_maix_image_width = size[0];
    this->_maix_image_height = size[1];
    this->img = libmaix_image_create(size[0], size[1], any_cast<libmaix_image_mode_t>(py_to_pram[this->get_to(this->_maix_image_type)][0]), LIBMAIX_IMAGE_LAYOUT_HWC, NULL, true);
    this->_maix_image_size = this->_maix_image_width * this->_maix_image_height * any_cast<int>(py_to_pram[this->get_to(this->_maix_image_type)][1]);
    if (py::isinstance<py::bytes>(data))
    {
        if (this->img)
        {
            string tmp = data.cast<string>();
            memcpy(this->img->data, tmp.c_str(), this->_maix_image_size);
        }
        else
        {
            this->v_close();
            this->_maix_image_size = 0;
        }

        return *this;
    }
}

_maix_image img_new(std::vector<int> size, std::vector<int> color, std::string mode)
{
    _maix_image img_val();
    img_val._new(size, color, mode);
    return img_val;
}
_maix_image img_load(py::bytes data, std::vector<int> size, std::string mode)
{
    _maix_image img_val();
    img_val._load(size, color, mode);
    return img_val;
}

PYBIND11_MODULE(maix_image, mo)
{
    mo.def("new", &img_new, py::arg("size") = std::vector<int>{240, 240}, py::arg("color") = std::vector<int>{0, 0, 0}, py::arg("mode") = "RGB");
    mo.def("load", &img_load, py::arg("data"), py::arg("size") = std::vector<int>{240, 240}, py::arg("mode") = "RGB");

    pybind11::class_<_maix_image>(mo, "Image")
        .def(pybind11::init<>())
        .def_readonly("width", &_maix_image::_maix_image_width)
        .def_readonly("height", &_maix_image::_maix_image_height)
        .def_readonly("mode", &_maix_image::_maix_image_type)
        .def_readonly("size", &_maix_image::_maix_image_size)
        .def("new", &_maix_image::_new, py::arg("size") = std::vector<int>{240, 240}, py::arg("color") = std::vector<int>{0, 0, 0}, py::arg("mode") = "RGB")
        .def("load", &_maix_image::load, py::arg("data"), py::arg("size") = std::vector<int>{240, 240}, py::arg("mode") = "RGB")

        // .def_readonly("format", &_maix_image::_maix_image_format)
        // .def_readonly("size", &_maix_image::_maix_image_size)
        // .def_readonly("mode", &_maix_image::_maix_image_mode)
        // // .def("new", &_maix_image::_new, py::arg("size") = std::vector<int>{240, 240}, py::arg("color") = std::vector<int>{0, 0, 0}, py::arg("mode") = "RGB")
        // .def("show", &_maix_image::_show)
        // // .def("load", &_maix_image::load, py::arg("data"), py::arg("size") = std::vector<int>{240, 240}, py::arg("mode") = "RGB")
        // .def("save", &_maix_image::save, py::arg("path"))
        // .def("tobytes", &_maix_image::tobytes)
        // .def("resize", &_maix_image::resize, py::arg("w"), py::arg("h"))
        // .def("rotate", &_maix_image::rotate, py::arg("rotate"))
        // .def("crop", &_maix_image::draw_crop, py::arg("thr"))
        // .def("convert", &_maix_image::draw_convert, py::arg("mode") = "RGB")
        // .def("draw_ellipse", &_maix_image::draw_ellipse, py::arg("rect"), py::arg("angle"), py::arg("startAngle"), py::arg("endAngle"), py::arg("color"), py::arg("thickness"))
        // .def("draw_string", &_maix_image::draw_string, py::arg("x"), py::arg("y"), py::arg("str"), py::arg("scale") = 1.0, py::arg("color") = std::vector<int>{127, 127, 127}, py::arg("thickness") = 1)
        // .def("draw_circle", &_maix_image::draw_circle, py::arg("circ"), py::arg("color") = std::vector<int>{127, 127, 127}, py::arg("thickness") = 1)
        // .def("draw_rectangle", &_maix_image::draw_rectangle, py::arg("rect"), py::arg("color") = std::vector<int>{127, 127, 127}, py::arg("thickness") = 1)
        // .def("draw_line", &_maix_image::draw_line, py::arg("line"), py::arg("color") = std::vector<int>{127, 127, 127}, py::arg("thickness") = 1)

        // .def("load_freetype", &_maix_image::test);
        ;
}
