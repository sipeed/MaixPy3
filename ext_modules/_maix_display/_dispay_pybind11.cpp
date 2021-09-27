
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
    
#ifdef R329Display
    pybind11::class_<r329_Display>(m, "Display")
        .def(py::init<int,int>(),py::arg("w") = 240, py::arg("h") = 240)
        .def_readwrite("width",&r329_Display::width)
        .def_readwrite("height",&r329_Display::height)
        .def("draw", &r329_Display::draw,py::arg("py_img"),py::arg("img_w") = 240,py::arg("img_w") = 240)
        .def("__enter__", &r329_Display::__enter__)
        .def("__exit__", &r329_Display::__exit__);
#endif

#ifdef V831Display
    pybind11::class_<v831_Display>(m, "Display")
        .def(py::init<int,int>(),py::arg("w") = 240, py::arg("h") = 240)
        .def_readwrite("width",&v831_Display::width)
        .def_readwrite("height",&v831_Display::height)
        .def("draw", &v831_Display::draw,py::arg("py_img"),py::arg("img_w") = 240,py::arg("img_w") = 240)
        .def("__enter__", &v831_Display::__enter__)
        .def("__exit__", &v831_Display::__exit__);
#endif
}