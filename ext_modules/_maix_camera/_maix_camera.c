
#include "_maix_camera.h"

#define _VERSION_ "0.2"
#define _NAME_ "_maix_camera"

#include "jpeglib.h"
#include "string.h"

PyDoc_STRVAR(_maix_camera_doc, "MaixPy3 Camera\n");

static PyObject *_maix_camera_help() {
    return PyUnicode_FromString(_maix_camera_doc);
}

static PyMethodDef _maix_camera_methods[] = {
    {"help", (PyCFunction)_maix_camera_help, METH_NOARGS, _maix_camera_doc},
    {NULL}
};

void define_constants(PyObject *module) {
    PyModule_AddObject(module, "_VERSION_", Py_BuildValue("s", _VERSION_));
}

static struct PyModuleDef _maix_cameramodule = {
    PyModuleDef_HEAD_INIT,
    _NAME_,         /* Module name */
    _maix_camera_doc,	/* Module _maix_cameraMethods */
    -1,			    /* size of per-interpreter state of the module, size of per-interpreter state of the module,*/
    _maix_camera_methods,
};

PyMODINIT_FUNC PyInit__maix_camera(void)
{
    PyObject *module;

    module = PyModule_Create(&_maix_cameramodule);
    PyObject *version = PyUnicode_FromString(_VERSION_);

    /* Constants */
    define_constants(module);

    /* Set module version */
    PyObject *dict = PyModule_GetDict(module);
    PyDict_SetItemString(dict, "__version__", version);
    Py_DECREF(version);

    if (PyType_Ready(&VirtualCameraObjectType) < 0) {
        return NULL;
    }

    /* Register VirtualCameraObjectType */
    Py_INCREF(&VirtualCameraObjectType);
    PyModule_AddObject(module, VirtualCamera_name, (PyObject *)&VirtualCameraObjectType);

#ifdef V831Camera
    if (PyType_Ready(&V831CameraObjectType) < 0) {
        return NULL;
    }

    /* Register V831CameraObjectType */
    Py_INCREF(&V831CameraObjectType);
    PyModule_AddObject(module, V831Camera_name, (PyObject *)&V831CameraObjectType);
#endif

#ifdef R329Camera
    if (PyType_Ready(&R329CameraObjectType) < 0) {
        return NULL;
    }

    /* Register R329CameraObjectType */
    Py_INCREF(&R329CameraObjectType);
    PyModule_AddObject(module, R329Camera_name, (PyObject *)&R329CameraObjectType);
#endif

    return module;
}
