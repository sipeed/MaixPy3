#include "_maix_camera.h"
#ifdef R329Camera

using namespace std;
namespace py = pybind11;

void v_close(r329_camera *tp)
{
    if (NULL != tp->cam)
        libmaix_cam_destroy(&tp->cam);

    if (NULL != tp->img_buff)
    {
        free(tp->img_buff);
        tp->img_buff = NULL;
    }
    libmaix_camera_module_deinit();
}

void v_init(r329_camera *tp)
{
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
}

r329_camera::r329_camera(int w, int h, int dev_id)
{
    this->width = w;
    this->height = h;
    this->dev_id = dev_id;
    v_init(this);
}

r329_camera::~r329_camera()
{
    v_close(this);
    return;
}
// "read()\n\nRead image(rgb888) bytes data from device.\n"
pybind11::object r329_camera::read()
{
    size_t len = this->width * this->height * 3;
    int ret = LIBMAIX_ERR_NONE;
    for (size_t i = 0; i < 5; i++)
    {
        ret = this->cam->capture(this->cam, (unsigned char *)this->img_buff);
        // not ready， sleep to release CPU
        if (ret == LIBMAIX_ERR_NOT_READY)
        {
            usleep(25 * 1000);
            continue;
        }
        if (ret == LIBMAIX_ERR_NONE)
        {
            break;
        }
    }
    /* Copy data to bytearray and return */
    py::list return_val;
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
    return return_val;
}
void r329_camera::close()
{
    v_close(this);
}

void r329_camera::__enter__()
{
    //do nothing...
}

void r329_camera::__exit__()
{
    v_close(this);
}


#endif