
#include "_maix.h"

#include "jpeglib.h"

#include "string.h"

static PyObject *PyJpegCompress(char *inData, int width, int height, int channels, int color_space, int quality)
{
  struct jpeg_compress_struct cinfo;
  struct jpeg_error_mgr jerr;
  long unsigned int outSize = 0;
  uint8_t *outbuffer = NULL;
  JSAMPROW row_pointer[1];

  cinfo.err = jpeg_std_error(&jerr);
  jpeg_create_compress(&cinfo);
  jpeg_mem_dest(&cinfo, &outbuffer, &outSize);
  cinfo.image_width = width;
  cinfo.image_height = height;
  cinfo.input_components = channels;  // 3 / 1
  cinfo.in_color_space = color_space; // JCS_RGB / JCS_GRAYSCALE
  jpeg_set_defaults(&cinfo);
  jpeg_set_quality(&cinfo, quality, TRUE); // default 75
  jpeg_start_compress(&cinfo, TRUE);
  int row_stride = width;
  while (cinfo.next_scanline < cinfo.image_height)
  {
    row_pointer[0] = (uint8_t *)&inData[cinfo.next_scanline * row_stride];
    (void)jpeg_write_scanlines(&cinfo, row_pointer, 1);
  }
  jpeg_finish_compress(&cinfo);

  PyObject *bytes = PyBytes_FromStringAndSize((const char *)outbuffer, outSize);

  if (NULL != outbuffer)
    free(outbuffer), outbuffer = NULL;

  jpeg_destroy_compress(&cinfo);
  return bytes;
}

/* Macros needed for Python 3 */
#ifndef PyInt_Check
#define PyInt_Check PyLong_Check
#define PyInt_FromLong PyLong_FromLong
#define PyInt_AsLong PyLong_AsLong
#define PyInt_Type PyLong_Type
#endif

PyDoc_STRVAR(CameraObject_type_doc, "Camera(width, height) -> Camera object.\n");
typedef struct
{
  PyObject_HEAD;
  unsigned int width, height;

} CameraObject;

PyDoc_STRVAR(Camera_close_doc, "close()\n\nClose Camera device.\n");
static PyObject *Camera_close(CameraObject *self)
{
  Py_INCREF(Py_None);
  return Py_None;
}

static PyObject *Camera_new(PyTypeObject *type, PyObject *args, PyObject *kwds)
{
  CameraObject *self;

  if ((self = (CameraObject *)type->tp_alloc(type, 0)) == NULL)
  {
    return NULL;
  }

  Py_INCREF(self);
  return (PyObject *)self;
}

static void Camera_free(CameraObject *self)
{
  PyObject *ref = Camera_close(self);
  Py_XDECREF(ref);

  Py_TYPE(self)->tp_free((PyObject *)self);
}

static int Camera_init(CameraObject *self, PyObject *args, PyObject *kwds)
{
  // default init value
  self->width = 640, self->height = 480;

  static char *kwlist[] = {"width", "height", NULL};

  if (!PyArg_ParseTupleAndKeywords(args, kwds, "|ii:__init__", kwlist,
                                   &self->width, &self->height))
  {
    return -1;
  }

  return 0;
}

static PyObject *Camera_enter(PyObject *self, PyObject *args)
{
  if (!PyArg_ParseTuple(args, ""))
    return NULL;

  Py_INCREF(self);
  return self;
}

static PyObject *Camera_exit(CameraObject *self, PyObject *args)
{
  PyObject *exc_type = 0;
  PyObject *exc_value = 0;
  PyObject *traceback = 0;

  if (!PyArg_UnpackTuple(args, "__exit__", 3, 3, &exc_type, &exc_value, &traceback))
  {
    return 0;
  }

  /* Close Camera bus */
  Camera_close(self);
  Py_RETURN_FALSE;
}

/* str */
static PyObject *Camera_str(PyObject *object)
{
  char desc[128];
  PyObject *dev_desc = NULL;
  // CameraObject *self = (CameraObject *)object;
  // Camera_get_device_desc(&self->dev, desc, sizeof(desc));

  dev_desc = PyUnicode_FromString(desc);

  Py_INCREF(dev_desc);
  return dev_desc;
}

PyDoc_STRVAR(Camera_rgb2jpg_doc, "rgb2jpg()\n\nConvert image(rgb888) bytes data to jpeg image bytes.\n");
static PyObject *Camera_rgb2jpg(CameraObject *self, PyObject *args)
{
  PyObject *bytes = NULL;
  PyObject *inRGB = NULL;

  int width = self->width, height = self->height;
  int channels = 3, color_space = JCS_RGB, quality = 75;

  if (!PyArg_ParseTuple(args, "O|iiiii", &inRGB,
                        &width, &height, &channels, &color_space, &quality))
    return NULL;

  char *rgb_data = PyBytes_AS_STRING(inRGB);

  bytes = PyJpegCompress(rgb_data, width, height, channels, color_space, quality);

  return bytes;
}

static PyMethodDef Camera_methods[] = {

    {"rgb2jpg", (PyCFunction)Camera_rgb2jpg, METH_VARARGS, Camera_rgb2jpg_doc},
    {"close", (PyCFunction)Camera_close, METH_NOARGS, Camera_close_doc},
    {"__enter__", (PyCFunction)Camera_enter, METH_NOARGS, NULL},
    {"__exit__", (PyCFunction)Camera_exit, METH_NOARGS, NULL},
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
PyDoc_STRVAR(Camera_width_doc, "Camera internal operate width.\n\n");
static PyObject *Camera_get_width(CameraObject *self, void *closure)
{
  PyObject *result = Py_BuildValue("i", self->width);
  Py_INCREF(result);
  return result;
}

static int Camera_set_width(CameraObject *self, PyObject *value, void *closeure)
{
  if (check_user_input("width", value, 1, 1000) != 0)
  {
    return -1;
  }
  self->width = PyLong_AsLong(value);
  return 0;
}

/* height */
PyDoc_STRVAR(Camera_height_doc, "Camera internal operate height.\n\n");
static PyObject *Camera_get_height(CameraObject *self, void *closure)
{
  PyObject *result = Py_BuildValue("i", self->height);
  Py_INCREF(result);
  return result;
}

static int Camera_set_height(CameraObject *self, PyObject *value, void *closeure)
{
  if (check_user_input("height", value, 1, 1000) != 0)
  {
    return -1;
  }
  self->height = PyLong_AsLong(value);
  return 0;
}

static PyGetSetDef Camera_getseters[] = {
    {"width", (getter)Camera_get_width, (setter)Camera_set_width, Camera_width_doc},
    {"height", (getter)Camera_get_height, (setter)Camera_set_height, Camera_height_doc},
    {NULL},
};

PyTypeObject CameraObjectType = {
#if PY_MAJOR_VERSION >= 3
    PyVarObject_HEAD_INIT(NULL, 0)
#else
    PyObject_HEAD_INIT(NULL) 0, /* ob_size */
#endif
        Camera_name,                           /* tp_name */
    sizeof(CameraObject),                      /* tp_basicsize */
    0,                                        /* tp_itemsize */
    (destructor)Camera_free,                   /* tp_dealloc */
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
    Camera_str,                                /* tp_str */
    0,                                        /* tp_getattro */
    0,                                        /* tp_setattro */
    0,                                        /* tp_as_buffer */
    Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE, /* tp_flags */
    CameraObject_type_doc,                     /* tp_doc */
    0,                                        /* tp_traverse */
    0,                                        /* tp_clear */
    0,                                        /* tp_richcompare */
    0,                                        /* tp_weaklistoffset */
    0,                                        /* tp_iter */
    0,                                        /* tp_iternext */
    Camera_methods,                            /* tp_methods */
    0,                                        /* tp_members */
    Camera_getseters,                          /* tp_getset */
    0,                                        /* tp_base */
    0,                                        /* tp_dict */
    0,                                        /* tp_descr_get */
    0,                                        /* tp_descr_set */
    0,                                        /* tp_dictoffset */
    (initproc)Camera_init,                     /* tp_init */
    0,                                        /* tp_alloc */
    Camera_new,                                /* tp_new */
};
