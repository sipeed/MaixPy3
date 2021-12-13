
#include "py_maix_nn.h"
// #include "py_maix_nn_classifier.h"
// #include "py_maix_nn_FaceRecognize.h"

#include "string.h"
#include "libmaix_nn.h"


static PyObject* _maix_nn_help() {
    return PyUnicode_FromString(_maix_nn_doc);
}

PyDoc_STRVAR(_maix_nn_load_doc, "load model, returen a nn.Model object\n");
static PyObject *_maix_nn_load(PyObject *self, PyObject *args, PyObject *kw_args)
{
    /* parse args */
    static char *kwlist[] = {"model_path", "opt", NULL};

    PyObject *model_path = NULL;
    PyObject *opt        = NULL;

    /* Get the buffer's memory */
    if (!PyArg_ParseTupleAndKeywords(args, kw_args, "|OO:load", kwlist,
                                     &model_path, &opt))
    {
        return NULL;
    }
    if(!model_path || !opt)
    {
        PyErr_SetString(PyExc_ValueError, "nee mode_path and opt args");
        return NULL;
    }
    /* new object */
    PyObject* call_args = PyList_New(0);
    PyObject* call_keywords = PyDict_New();
    PyObject* model_obj = PyMaix_NN_Model_Type.tp_new(&PyMaix_NN_Model_Type, call_args, call_keywords);
    Py_DECREF(call_args);Py_DECREF(call_keywords);
    if(!model_obj)
    {
        PyErr_NoMemory();
        return NULL;
    }
    /* init model object */

    call_args = Py_BuildValue("(O)", model_path);
    call_keywords = PyDict_New();
    PyDict_SetItemString(call_keywords, "opt", opt);
    PyObject *o_init_func = PyObject_GetAttrString(model_obj, "__init__");
    PyObject* ret = PyObject_Call(o_init_func, call_args, call_keywords);
    // Py_DECREF(opt);
    Py_DECREF(call_args);
    Py_DECREF(call_keywords);
    Py_DECREF(o_init_func);
    if( ret == NULL)
    {
        return NULL;
    }

    return model_obj;
}

static PyMethodDef _maix_nn_methods[] = {
    {"help", (PyCFunction)_maix_nn_help, METH_NOARGS, _maix_nn_doc},
    {"load", (PyCFunction)_maix_nn_load, METH_VARARGS | METH_KEYWORDS, _maix_nn_load_doc},
    {NULL, NULL, 0, NULL}
};

static struct PyModuleDef _maix_nn_module = {
    PyModuleDef_HEAD_INIT,
    _MAIX_NN_NAME_,         /* Module name */
    _maix_nn_doc,	/* Module _maixMethods */
    -1,			    /* size of per-interpreter state of the module, size of per-interpreter state of the module,*/
    _maix_nn_methods,
};

PyMODINIT_FUNC PyInit__maix_nn(void)
{

    PyObject *module;

    module = PyModule_Create(&_maix_nn_module);
    PyObject *version = PyUnicode_FromString(_MAIX_NN_VERSION_);


    /* Set module version */
    PyObject *dict = PyModule_GetDict(module);
    PyDict_SetItemString(dict, "__version__", version);
    Py_DECREF(version);

    /* Add class*/
    // PyModule_AddObject(module, "Model", SpamError);

    /* init object(default value) */
    if (PyType_Ready(&PyMaix_NN_Model_Type) < 0) {
        return NULL;
    }
    // if (PyType_Ready(&PyMaix_NN_Decoder_Yolo2_Type) < 0) {
    //     return NULL;
    // }
    // if (PyType_Ready(&PyMaix_NN_app_Classifier_Type) < 0) {
    //     return NULL;
    // }
    // if (PyType_Ready(&PyMaix_NN_app_FaceRecognize_Type) < 0) {
    //     return NULL;
    // }

    /* Add maix.nn.F module*/
    PyObject *functional_module = PyModule_Create(&maix_nn_functional_module);
    PyModule_AddObject(module, "F", functional_module);
    Py_INCREF(functional_module);
    PyModule_AddObject(module, "functional", functional_module);

    // /* Add maix.nn.decoder module,
    //    add maix.nn.decoder.Yolo2 class*/
    // PyObject *decoder_module = PyModule_Create(&maix_nn_decoder_module);
    // Py_INCREF(&PyMaix_NN_Decoder_Yolo2_Type);
    // PyModule_AddObject(decoder_module, "Yolo2", (PyObject*)&PyMaix_NN_Decoder_Yolo2_Type);
    // PyModule_AddObject(module, "decoder", decoder_module);

    // /* Add maix.nn._app module,
    //    add maix.nn._app.classifier class*/
    // PyObject *app_module = PyModule_Create(&maix_nn_app_module);
    // Py_INCREF(&PyMaix_NN_app_Classifier_Type);
    // PyModule_AddObject(app_module, "Classifier", (PyObject*)&PyMaix_NN_app_Classifier_Type);
    // Py_INCREF(&PyMaix_NN_app_FaceRecognize_Type);
    // PyModule_AddObject(app_module, "FaceRecognize", (PyObject*)&PyMaix_NN_app_FaceRecognize_Type);
    // PyModule_AddObject(module, "_app", app_module);

    return module;
}
