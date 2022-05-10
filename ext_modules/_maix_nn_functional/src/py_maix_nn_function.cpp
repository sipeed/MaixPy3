#include "py_maix_nn_function.h"
#include <iostream>
#include <sstream>
#include <pybind11/pybind11.h>
#include <pybind11/embed.h>
#include <pybind11/stl.h>
#include <vector>
#include <list>
#include <array>
#include <map>
#include <set>
#include <typeinfo>
#include "libmaix_nn.h"
#include "string.h"
#include "mud.h"
#include <math.h>

#define debug_line printf("%s:%d %s %s %s \r\n", __FILE__, __LINE__, __FUNCTION__, __DATE__, __TIME__)

namespace py = pybind11;

class _Function
{
    private:
    public:
    _Function();
    ~_Function();
    py::object softmax (py::object o_input , int dim);
};

py::object _Function::softmax (py::object o_input , int dim)
{
    if(dim >= 0)
    {
        std::cout <<"not support dim arg yet" << std::endl;
        return py::none();
    }
    auto numpy_object_num =py::module::import("numpy").attr("ndarray");
    if(py::isinstance<py::bytes>(o_input))
    {

    }
    else
    {
        std::cout << "only support numpy object" << std::endl;
        return py::none();
    }
}
