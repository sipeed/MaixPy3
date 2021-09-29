
#include "_maix_camera.h"

// #define R329Camera

#ifdef R329Camera

#include "libmaix_cam.h"
// #include "libmaix_image.h"

PyDoc_STRVAR(R329CameraObject_type_doc, "R329Camera(width, height) -> R329Camera object.\n");
typedef struct
{
    PyObject_HEAD;
    unsigned int width, height, dev_id;

    libmaix_cam_t *cam;
    uint8_t *img_buff;
    // libmaix_image_t* yuv_img;
    // libmaix_image_t* rgb_img;

} R329CameraObject;

PyDoc_STRVAR(R329Camera_close_doc, "close()\n\nClose R329Camera device.\n");
static PyObject *R329Camera_close(R329CameraObject *self)
{

    if (NULL != self->cam)
        libmaix_cam_destroy(&self->cam);
    if (NULL != self->img_buff)
        free(self->img_buff);

    // self->img_buff = malloc(self->height * self->width * 3);
    // if (NULL != self->rgb_img) {
    //     libmaix_image_destroy(&self->rgb_img);
    // }
    // if (NULL != self->yuv_img) {
    //     libmaix_image_destroy(&self->yuv_img);
    // }
    
    // libmaix_image_module_deinit();
    libmaix_camera_module_deinit();

    Py_RETURN_NONE;
}

static PyObject *R329Camera_new(PyTypeObject *type, PyObject *args, PyObject *kwds)
{
    R329CameraObject *self;

    if ((self = (R329CameraObject *)type->tp_alloc(type, 0)) == NULL)
    {
        return NULL;
    }

    return (PyObject *)self;
}

static void R329Camera_free(R329CameraObject *self)
{
    PyObject *ref = R329Camera_close(self);
    Py_XDECREF(ref);

    Py_TYPE(self)->tp_free((PyObject *)self);
}

static int R329Camera_init(R329CameraObject *self, PyObject *args, PyObject *kwds)
{
    // default init value
    self->width = 640, self->height = 480;

    static char *kwlist[] = {"width", "height", "dev_id", NULL};

    if (!PyArg_ParseTupleAndKeywords(args, kwds, "|iii:__init__", kwlist,
                                     &self->width, &self->height, &self->dev_id))
    {
        return -1;
    }

    libmaix_camera_module_init();
    self->img_buff = malloc(self->height * self->width * 3);
    if (NULL != self->img_buff)
    {


    // libmaix_image_module_init();
    // self->yuv_img = libmaix_image_create(self->width, self->height, LIBMAIX_IMAGE_MODE_YUV420SP_NV21, LIBMAIX_IMAGE_LAYOUT_HWC, NULL, true);
    // if(NULL != self->yuv_img)
    // {
    //   self->rgb_img = libmaix_image_create(self->width, self->height, LIBMAIX_IMAGE_MODE_RGB888, LIBMAIX_IMAGE_LAYOUT_HWC, NULL, true);
    //   if(NULL != self->rgb_img)
    //   {
        self->cam = libmaix_cam_create(self->dev_id, self->width, self->height, 0, 1);
        if (NULL != self->cam)
        {
            int ret = self->cam->start_capture(self->cam);
            if (0 == ret)
            {
                return 0;
            }
        }
    //   }
    // }
    }
    R329Camera_close(self);
    PyErr_SetFromErrno(PyExc_IOError);

    return -1;
}

static PyObject *R329Camera_enter(PyObject *self, PyObject *args)
{
    if (!PyArg_ParseTuple(args, ""))
        return NULL;

    Py_INCREF(self);
    return self;
}

static PyObject *R329Camera_exit(R329CameraObject *self, PyObject *args)
{
    PyObject *exc_type = 0;
    PyObject *exc_value = 0;
    PyObject *traceback = 0;

    if (!PyArg_UnpackTuple(args, "__exit__", 3, 3, &exc_type, &exc_value, &traceback))
    {
        return 0;
    }

    Py_XDECREF(self);
    R329Camera_close(self);
    Py_RETURN_NONE;
}

/* str */
static PyObject *R329Camera_str(PyObject *object)
{
  return PyUnicode_FromString(__FILE__);
}

PyDoc_STRVAR(R329Camera_read_doc, "read()\n\nRead image(rgb888) bytes data from device.\n");
static PyObject *R329Camera_read(R329CameraObject *self, PyObject *args)
{
    PyObject *bytes = NULL;

    char *buf = NULL;
    size_t len = self->width * self->height * 3;
    // self->yuv_img->mode = LIBMAIX_IMAGE_MODE_YUV420SP_NV21;
    libmaix_err_t ret = LIBMAIX_ERR_NONE;
    for (size_t i = 0; i < 5; i++)
    {
      ret = self->cam->capture(self->cam, (unsigned char*)self->img_buff);
      // not ready， sleep to release CPU
      if(ret == LIBMAIX_ERR_NOT_READY)
      {
          usleep(25 * 1000);
          continue;
      }
      if(ret == LIBMAIX_ERR_NONE)
      {
        // libmaix_err_t err = self->yuv_img->convert(self->yuv_img, LIBMAIX_IMAGE_MODE_RGB888, &self->rgb_img);
        // if(err == LIBMAIX_ERR_NONE)
        // {
        //   buf = self->rgb_img->data;
        // }
        // else
        // {
        //   ret = err;
          // printf("convert fail: %s\n", libmaix_get_err_msg(err));
        // }
        buf = self->img_buff;
        break;
      }
    }
    /* Copy data to bytearray and return */
    if (buf != NULL) {
      bytes = PyBytes_FromStringAndSize(buf, len);
    } else {
      bytes = Py_None;
    }
    // Py_INCREF(bytes); // cancel ref Tag
    PyObject* list = PyList_New(2);
    if (!list) Py_RETURN_NONE;
    PyObject* tmp = PyInt_FromLong(!ret);
    PyList_SetItem(list, 0, tmp);
    Py_DECREF(tmp);
    PyList_SET_ITEM(list, 1, bytes);
    return list;
}

static PyMethodDef R329Camera_methods[] = {

    {"read", (PyCFunction)R329Camera_read, METH_VARARGS, R329Camera_read_doc},
    {"close", (PyCFunction)R329Camera_close, METH_NOARGS, R329Camera_close_doc},
    {"__enter__", (PyCFunction)R329Camera_enter, METH_NOARGS, NULL},
    {"__exit__", (PyCFunction)R329Camera_exit, METH_NOARGS, NULL},
    {NULL},
};

/* Check */
static int check_user_input(const char *name, PyObject *input, int min, int max)
{

    int value;

    if (input == NULL)
    {
        PyErr_SetString(PyExc_TypeError, "Cannot delete the last attribute");
        return -1;
    }

    /* Other attribute, integer type */
    if (!PyInt_Check(input))
    {
        PyErr_SetString(PyExc_TypeError, "The last attribute value must be an integer");
        return -1;
    }

    /* Convert value to int */
    value = PyLong_AsLong(input);

    /* Check input value range */
    if (value < min || value > max)
    {
        PyErr_Format(PyExc_ValueError, "invalid ' %s'(%d - %d)", name, min, max);
        return -1;
    }

    return 0;
}

/* width */
PyDoc_STRVAR(R329Camera_width_doc, "R329Camera internal operate width.\n\n");
static PyObject *R329Camera_get_width(R329CameraObject *self, void *closure)
{
    PyObject *result = Py_BuildValue("i", self->width);
    return result;
}

static int R329Camera_set_width(R329CameraObject *self, PyObject *value, void *closeure)
{
    if (check_user_input("width", value, 640, 480) != 0)
    {
        return -1;
    }
    self->width = PyLong_AsLong(value);
    return 0;
}

/* height */
PyDoc_STRVAR(R329Camera_height_doc, "R329Camera internal operate height.\n\n");
static PyObject *R329Camera_get_height(R329CameraObject *self, void *closure)
{
    PyObject *result = Py_BuildValue("i", self->height);
    return result;
}

static int R329Camera_set_height(R329CameraObject *self, PyObject *value, void *closeure)
{
    if (check_user_input("height", value, 640, 480) != 0)
    {
        return -1;
    }
    self->height = PyLong_AsLong(value);
    return 0;
}

static PyGetSetDef R329Camera_getseters[] = {
    {"width", (getter)R329Camera_get_width, (setter)R329Camera_set_width, R329Camera_width_doc},
    {"height", (getter)R329Camera_get_height, (setter)R329Camera_set_height, R329Camera_height_doc},
    {NULL},
};

PyTypeObject R329CameraObjectType = {
    PyVarObject_HEAD_INIT(NULL, 0)
    R329Camera_name,                           /* tp_name */
    sizeof(R329CameraObject),                      /* tp_basicsize */
    0,                                        /* tp_itemsize */
    (destructor)R329Camera_free,                   /* tp_dealloc */
    0,                                        /* tp_print */
    0,                                        /* tp_getattr */
    0,                                        /* tp_setattr */
    0,                                        /* tp_compare */
    0,                                        /* tp_repr */
    0,                                        /* tp_as_number */
    0,                                        /* tp_as_sequence */
    0,                                        /* tp_as_mapping */
    0,                                        /* tp_hash */
    0,                                        /* tp_call */
    R329Camera_str,                                /* tp_str */
    0,                                        /* tp_getattro */
    0,                                        /* tp_setattro */
    0,                                        /* tp_as_buffer */
    Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE, /* tp_flags */
    R329CameraObject_type_doc,                     /* tp_doc */
    0,                                        /* tp_traverse */
    0,                                        /* tp_clear */
    0,                                        /* tp_richcompare */
    0,                                        /* tp_weaklistoffset */
    0,                                        /* tp_iter */
    0,                                        /* tp_iternext */
    R329Camera_methods,                            /* tp_methods */
    0,                                        /* tp_members */
    R329Camera_getseters,                          /* tp_getset */
    0,                                        /* tp_base */
    0,                                        /* tp_dict */
    0,                                        /* tp_descr_get */
    0,                                        /* tp_descr_set */
    0,                                        /* tp_dictoffset */
    (initproc)R329Camera_init,                     /* tp_init */
    0,                                        /* tp_alloc */
    R329Camera_new,                                /* tp_new */
};

#endif
