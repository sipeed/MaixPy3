
#include "_maix.h"

#define _VERSION_ "0.1"
#define _NAME_ "_maix"

#include "jpeglib.h"
#include "string.h"

PyDoc_STRVAR(_maix_doc, "MaixPy Python3 library.\n");

static PyObject *_maix_help() {
    return PyUnicode_FromString(_maix_doc);
}

PyDoc_STRVAR(_maix_rgb2jpg_doc, "rgb2jpg()\n\nConvert image(rgb888) bytes data to jpeg image bytes.\n");
static PyObject *PyJpegCompress(char *inData, int width, int height, int channels, int color_space, int quality);
static PyObject *_maix_rgb2jpg(PyObject *self, PyObject *args)
{
  PyObject *bytes = NULL;
  PyObject *inRGB = NULL;

  int width = 1, height = 1;
  int channels = 3, color_space = JCS_RGB, quality = 75;

  if (!PyArg_ParseTuple(args, "Oii|iii", &inRGB,
                        &width, &height, &channels, &color_space, &quality))
    return NULL;
  
  char *rgb_data = PyBytes_AS_STRING(inRGB);

  bytes = PyJpegCompress(rgb_data, width, height, channels, color_space, quality);

  return bytes;
}

static PyMethodDef _maix_methods[] = {
    {"help", (PyCFunction)_maix_help, METH_NOARGS, _maix_doc},
    {"rgb2jpg", (PyCFunction)_maix_rgb2jpg, METH_VARARGS, _maix_rgb2jpg_doc},
    {NULL}
};

void define_constants(PyObject *module) {
    PyModule_AddObject(module, "_VERSION_", Py_BuildValue("H", _VERSION_));
}

static struct PyModuleDef _maixmodule = {
    PyModuleDef_HEAD_INIT,
    _NAME_,         /* Module name */
    _maix_doc,	/* Module _maixMethods */
    -1,			    /* size of per-interpreter state of the module, size of per-interpreter state of the module,*/
    _maix_methods,
};

PyMODINIT_FUNC PyInit__maix(void)
{

    PyObject *module;

    if (PyType_Ready(&CameraObjectType) < 0) {
        return NULL;
    }

    if (PyType_Ready(&DisplayObjectType) < 0) {
        return NULL;
    }

    module = PyModule_Create(&_maixmodule);
    PyObject *version = PyUnicode_FromString(_VERSION_);

    /* Constants */
    define_constants(module);

    /* Set module version */
    PyObject *dict = PyModule_GetDict(module);
    PyDict_SetItemString(dict, "__version__", version);
    Py_DECREF(version);

    /* Register CameraObjectType */
    Py_INCREF(&CameraObjectType);
    PyModule_AddObject(module, Camera_name, (PyObject *)&CameraObjectType);

    /* Register DisplayObjectType */
    Py_INCREF(&DisplayObjectType);
    PyModule_AddObject(module, Display_name, (PyObject *)&DisplayObjectType);

    return module;
}

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
  int row_stride = width * 3;
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