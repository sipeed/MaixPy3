
#include "_maix_camera.h"

using namespace std;
namespace py = pybind11;

class _Camera_pybind11
{
private:
public:
    string version = "0.2";
    string name = "_maix_Camera";
    _Camera_pybind11()
    {
    }
    ~_Camera_pybind11()
    {
        cout << "_maix_Camera exit!" << endl;
    }
    string _maix_Camera_help()
    {
        return "MaixPy3 Camera\n";
    }
};

PYBIND11_MODULE(_maix_camera, m)
{
    m.doc() = "This is MaixPy3 Camera\n";
    pybind11::class_<_Camera_pybind11>(m, "_maix_camera")
        .def(pybind11::init())
        .def_readonly("_VERSION_", &_Camera_pybind11::version)
        .def_readonly("_NAME_", &_Camera_pybind11::name)
        .def("help", &_Camera_pybind11::_maix_Camera_help, R"pbdoc(read()\n\nRead image(rgb888) bytes data from device.\n)pbdoc")
        .doc() = "_maix_Camera";

#ifdef R329Camera
    pybind11::class_<r329_camera>(m, "Camera")
        .def(py::init<int, int, int>(), py::arg("w") = 240, py::arg("h") = 240, py::arg("dev_id") = 0)
        .def_readwrite("width", &r329_camera::width)
        .def_readwrite("height", &r329_camera::height)
        .def("read", &r329_camera::read, R"pbdoc(read()\n\nRead image(rgb888) bytes data from device.\n)pbdoc")
        .def("close", &r329_camera::close, R"pbdoc(close()\n\nClose R329Camera device.\n)pbdoc")
        .def("__enter__", &r329_camera::__enter__)
        .def("__exit__", &r329_camera::__exit__)
        .doc() = "Camera(width, height,device_id) -> R329Camera object.\n";
#endif

#ifdef V831Camera
    pybind11::class_<v831_Camera>(m, "Camera")
        .def(py::init<int, int, int>(), py::arg("w") = 240, py::arg("h") = 240, py::arg("dev_id") = 0)
        .def_readwrite("width", &v831_Camera::width)
        .def_readwrite("height", &v831_Camera::height)
        .def("read", &v831_Camera::read)
        .def("close", &v831_Camera::close, R"pbdoc(close()\n\nClose V831Camera device.\n)pbdoc")
        .def("__enter__", &v831_Camera::__enter__)
        .def("__exit__", &v831_Camera::__exit__)
        .doc() = "Camera(width, height,device_id) -> V831Camera object.\n";
    // .def("str", &v831_Camera::str)
#endif

#ifdef VirtualCamera
    pybind11::class_<virtual_camera>(m, "Camera")
        .def(py::init<int, int, int>(), py::arg("w") = 240, py::arg("h") = 240, py::arg("dev_id") = 0)
        .def_readwrite("width", &virtual_camera::width)
        .def_readwrite("height", &virtual_camera::height)
        .def("read", &virtual_camera::read)
        .def("close", &virtual_camera::close, R"pbdoc(close()\n\nClose VirtualCamera device.\n)pbdoc")
        .def("__enter__", &virtual_camera::__enter__)
        .def("__exit__", &virtual_camera::__exit__)
        .doc() = "Camera(width, height,device_id) -> VirtualCamera object.\n";
#endif

}
