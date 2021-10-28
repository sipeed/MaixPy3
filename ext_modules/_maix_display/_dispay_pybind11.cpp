
#include "_maix_display.h"

using namespace std;
namespace py = pybind11;

class _dispay_pybind11
{
private:
public:
    string version = "0.2";
    string name = "_maix_display";
    _dispay_pybind11()
    {
    }
    ~_dispay_pybind11()
    {
    }
    string _maix_display_help()
    {
        return "MaixPy3 Display\n";
    }
};

PYBIND11_MODULE(_maix_display, m)
{
    m.doc() = "This is MaixPy3 Display\n";
    pybind11::class_<_dispay_pybind11>(m, "_maix_display")
        .def(pybind11::init())
        .def_readonly("_VERSION_", &_dispay_pybind11::version)
        .def_readonly("_NAME_", &_dispay_pybind11::name)
        .def("help", &_dispay_pybind11::_maix_display_help);

    pybind11::class_<_Display>(m, "Display")
        .def(py::init())
        .def_readonly("width", &_Display::width)
        .def_readonly("height", &_Display::height)
        .def_readonly("bpp", &_Display::bpp)
        .def("draw", &_Display::draw, py::arg("py_img"), py::arg("img_w") = 240, py::arg("img_w") = 240, py::arg("mode") = 3)
        .def("__enter__", &_Display::__enter__)
        .def("__exit__", &_Display::__exit__);
}

void v_close(_Display *tp)
{
    if (NULL != tp->disp)
        libmaix_disp_destroy(&tp->disp);

    if (NULL != tp->display_image)
        libmaix_image_destroy(&tp->display_image);
}

void v_init(_Display *tp)
{
    tp->disp = libmaix_disp_create(0);
    if (tp->disp == NULL)
    {
        v_close(tp);
        PyErr_SetString(PyExc_RuntimeError, "libmaix_disp_create err!");
        throw py::error_already_set();
    }
    tp->bpp = tp->disp->bpp;
    tp->width = tp->disp->width;
    tp->height = tp->disp->height;
    tp->display_image = libmaix_image_create(tp->width, tp->height, LIBMAIX_IMAGE_MODE_RGB888, LIBMAIX_IMAGE_LAYOUT_HWC, NULL, false);

    if (NULL == tp->display_image)
    {
        v_close(tp);
        PyErr_SetString(PyExc_RuntimeError, "libmaix_image_create err!");
        throw py::error_already_set();
    }
}


_Display::_Display()
{
    v_init(this);
}

_Display::~_Display()
{
    v_close(this);
}

pybind11::object _Display::draw(pybind11::object py_img, int img_w, int img_h, int mode)
{
    string tmp;
    if (py::isinstance<py::bytes>(py_img))
    {
        tmp = py_img.cast<string>();
        this->display_image->data = (void *)(uint8_t *)tmp.c_str();
        this->display_image->mode = libmaix_image_mode_t(mode);
    }
    else
    {
        auto PIL_ = py::module::import("PIL.Image").attr("Image");
        if (py::isinstance(py_img, PIL_))
        {
            auto tobytes = PIL_.attr("tobytes");
            auto img_bytes = tobytes(py_img);
            tmp = img_bytes.cast<string>();
            this->display_image->data = (void *)(uint8_t *)tmp.c_str();
            auto img_size = py_img.attr("size").cast<vector<int>>();
            string tmp_mode = py_img.attr("mode").cast<string>();
            if (tmp_mode == "RGB")
                this->display_image->mode = LIBMAIX_IMAGE_MODE_RGB888;
            else // fit other format
                return py::none();
            //     this->display_image->mode = LIBMAIX_IMAGE_MODE_RGB888;
            img_w = img_size[0];
            img_h = img_size[1];
        }
    }

    this->display_image->width = img_w;
    this->display_image->height = img_h;

    if (NULL != this->disp)
    {
        if (this->disp->width >= img_w && this->disp->height >= img_h)
        {
            if (this->display_image->data != NULL)
            {
                this->disp->draw_image(this->disp, this->display_image);
            }
        }
    }
    return py::none();
}

void _Display::close()
{
    v_close(this);
}

void _Display::__enter__()
{
    //do nothing...
}

void _Display::__exit__()
{
    v_close(this);
}
