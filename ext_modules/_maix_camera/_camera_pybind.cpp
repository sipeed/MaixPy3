
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

    pybind11::class_<_camera>(m, "Camera")
        .def(py::init<int, int, int>(), py::arg("w") = 240, py::arg("h") = 240, py::arg("dev_id") = 0)
        .def_readwrite("width", &_camera::width)
        .def_readwrite("height", &_camera::height)
        .def("read", &_camera::read)
        .def("close", &_camera::close, R"pbdoc(close()\n\nClose Camera device.\n)pbdoc")
        .def("__enter__", &_camera::__enter__)
        .def("__exit__", &_camera::__exit__)
        .doc() = "Camera(width, height,device_id) -> Camera object.\n";
}

void v_close(_camera *tp)
{
#ifdef VirtualCamera
    cout << "virtual_camera close!" << endl;
#else //  VirtualCamera

#ifdef R329Camera
    if (NULL != tp->cam)
        libmaix_cam_destroy(&tp->cam);

    if (NULL != tp->img_buff)
    {
        free(tp->img_buff);
        tp->img_buff = NULL;
    }
    libmaix_camera_module_deinit();

#endif // R329Camera

#ifdef V83xCamera
    if (NULL != tp->cam)
        libmaix_cam_destroy(&tp->cam);
    if (NULL != tp->rgb_img)
    {
        libmaix_image_destroy(&tp->rgb_img);
    }
    if (NULL != tp->yuv_img)
    {
        libmaix_image_destroy(&tp->yuv_img);
    }
    libmaix_image_module_deinit();
    libmaix_camera_module_deinit();
#endif // V83xCamera

#endif //VirtualCamera else
}

void v_init(_camera *tp)
{
#ifdef VirtualCamera
    cout << "virtual_camera init success!" << endl;
#else // VirtualCamera

#ifdef R329Camera
    libmaix_camera_module_init();
    tp->img_buff = NULL;
    tp->img_buff = (uint8_t *)malloc(tp->height * tp->width * 3);
    if (NULL != tp->img_buff)
    {
        tp->cam = libmaix_cam_create(tp->dev_id, tp->width, tp->height, 0, 1);
        if (NULL != tp->cam)
        {
            int ret = tp->cam->start_capture(tp->cam);
            if (0 == ret)
            {
                return;
            }
        }
    }
    v_close(tp);
    PyErr_SetString(PyExc_RuntimeError, "libmaix_cam_create err!");
    throw py::error_already_set();
#endif // R329Camera

#ifdef V83xCamera
    libmaix_camera_module_init();
    libmaix_image_module_init();
    tp->yuv_img = libmaix_image_create(tp->width, tp->height, LIBMAIX_IMAGE_MODE_YUV420SP_NV21, LIBMAIX_IMAGE_LAYOUT_HWC, NULL, true);
    if (NULL != tp->yuv_img)
    {
        tp->rgb_img = libmaix_image_create(tp->width, tp->height, LIBMAIX_IMAGE_MODE_RGB888, LIBMAIX_IMAGE_LAYOUT_HWC, NULL, true);
        if (NULL != tp->rgb_img)
        {
            tp->cam = libmaix_cam_create(tp->dev_id, tp->width, tp->height, 0, 1);
            if (NULL != tp->cam)
            {
                int ret = tp->cam->start_capture(tp->cam);
                if (0 == ret)
                {
                    return;
                }
            }
        }
    }
    v_close(tp);
    PyErr_SetString(PyExc_RuntimeError, "libmaix_cam_create err!");
    throw py::error_already_set();
#endif // V83xCamera

#endif // VirtualCamera
}

_camera::_camera(int w, int h, int dev_id)
{
    this->width = w;
    this->height = h;
    this->dev_id = dev_id;
    v_init(this);
}

_camera::~_camera()
{
    v_close(this);
}
// "read()\n\nRead image(rgb888) bytes data from device.\n"
pybind11::list _camera::read()
{
    py::list return_val;
#ifdef VirtualCamera
    return_val.append(0);
    return_val.append(py::none());
#else

#ifdef R329Camera
    size_t len = this->width * this->height * 3;
    int ret = LIBMAIX_ERR_NONE;
    for (size_t i = 0; i < 5; i++)
    {
        ret = this->cam->capture(this->cam, (unsigned char *)this->img_buff);
        cout << "cam:" << ret << endl;
        // not ready， sleep to release CPU
        if (ret == LIBMAIX_ERR_NOT_READY)
        {
            usleep(25 * 1000);
            debug_line;
            continue;
        }
        if (ret == LIBMAIX_ERR_NONE)
        {
            break;
        }
    }
    /* Copy data to bytearray and return */
    return_val.append(!ret);
    if (this->img_buff != NULL)
    {
        py::bytes tmp((char *)this->img_buff, len);
        return_val.append(tmp);
    }
    else
    {
        return_val.append(py::none());
    }
#endif // R329Camera

#ifdef V83xCamera
    char *buf = NULL;
    size_t len = this->width * this->height * 3;
    this->yuv_img->mode = LIBMAIX_IMAGE_MODE_YUV420SP_NV21;
    int ret = LIBMAIX_ERR_NONE;
    for (size_t i = 0; i < 5; i++)
    {
        ret = this->cam->capture(this->cam, (unsigned char *)this->yuv_img->data);
        // not ready， sleep to release CPU
        if (ret == LIBMAIX_ERR_NOT_READY)
        {
            usleep(25 * 1000);
            continue;
        }
        if (ret == LIBMAIX_ERR_NONE)
        {
            libmaix_err_t err = this->yuv_img->convert(this->yuv_img, LIBMAIX_IMAGE_MODE_RGB888, &this->rgb_img);
            if (err == LIBMAIX_ERR_NONE)
            {
                buf = (char *)this->rgb_img->data;
            }
            else
            {
                ret = err;
                printf("convert fail: %s\n", libmaix_get_err_msg(err));
            }
            break;
        }
    }
    /* Copy data to bytearray and return */
    return_val.append(!ret);
    if (buf != NULL)
    {
        py::bytes tmp((char *)buf, len);
        return_val.append(tmp);
    }
    else
    {
        return_val.append(py::none());
    }

#endif // V83xCamera

#endif // VirtualCamera
    return return_val;
}

void _camera::close()
{
    v_close(this);
}

void _camera::__enter__()
{
    //do nothing...
}

void _camera::__exit__()
{
    v_close(this);
}

// string v831_camera::str()
// {
//     return string(__FILE__);
// }
