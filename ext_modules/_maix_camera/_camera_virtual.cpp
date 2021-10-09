#include "_maix_camera.h"

#ifdef VirtualCamera


using namespace std;
namespace py = pybind11;

void v_close(virtual_camera *tp)
{
  cout <<  "virtual_camera close!" << endl;
}

void v_init(virtual_camera *tp)
{

    cout <<  "virtual_camera init success!" << endl;

    // PyErr_SetString(PyExc_RuntimeError, "libmaix_cam_create err!");
    // throw py::error_already_set();
}

virtual_camera::virtual_camera(int w, int h, int dev_id)
{
    this->width = w;
    this->height = h;
    this->dev_id = dev_id;
    v_init(this);
}

virtual_camera::~virtual_camera()
{
    v_close(this);
}
// "read()\n\nRead image(rgb888) bytes data from device.\n"
pybind11::object virtual_camera::read()
{
    int i = 1;
    /* Copy data to bytearray and return */
    py::list return_val;
    return_val.append(!i);
    return_val.append(py::none());
    return return_val;
}
void virtual_camera::close()
{
    v_close(this);
}

void virtual_camera::__enter__()
{
    //do nothing...
}

void virtual_camera::__exit__()
{
    v_close(this);
}


#endif