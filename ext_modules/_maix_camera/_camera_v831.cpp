#include "_maix_camera.h"
#ifdef V831Camera

using namespace std;
namespace py = pybind11;

void v_close(v831_camera *tp)
{
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
}

void v_init(v831_camera *tp)
{
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
}

v831_camera::v831_camera(int w, int h, int dev_id)
{
    this->width = w;
    this->height = h;
    this->dev_id = dev_id;
    v_init(this);
}

v831_camera::~v831_camera()
{
    v_close(this);
    return;
}
// "read()\n\nRead image(rgb888) bytes data from device.\n"
pybind11::list v831_camera::read()
{
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
    py::list return_val;
    return_val.append(ret);
    if (buf != NULL)
    {
        py::bytes tmp((char *)buf, len);
        return_val.append(tmp);
    }
    else
    {
        return_val.append(py::none());
    }
    return return_val;
}

void v831_camera::close()
{
    v_close(this);
}

void v831_camera::__enter__()
{
    //do nothing...
}

void v831_camera::__exit__()
{
    v_close(this);
}

// string v831_camera::str()
// {
//     return string(__FILE__);
// }

#endif
