
#include "Python.h"
#include "py_maix_nn.h"
#include "py_maix_nn_classifier.h"
#include "libmaix_nn_classifier.h"
#include <math.h>
#include <string.h>


PyDoc_STRVAR(app_classifier_load_doc, "load classifier features.\n");
static PyObject* app_classifier_load(PyObject *self, PyObject *args, PyObject *kw_args)
{
    libmaix_err_t err = LIBMAIX_ERR_NONE;
    static char *kwlist[] = {"model", "path", NULL};
    const char* path = NULL;
    PyObject* model = NULL;

    if (!PyArg_ParseTupleAndKeywords(args, kw_args, "Os:load", kwlist,
                                     &model, &path))
    {
        return NULL;
    }
    if(!path)
    {
        PyErr_Format(PyExc_ValueError, "path error, should be str type");
        return NULL;
    }
    if(!model)
    {
        PyErr_Format(PyExc_ValueError, "model error, should be maix.nn.Model type");
        return NULL;
    }
    void* classifier_obj = NULL;
    int feature_len = 0;
    int input_w = 0, input_h = 0;
    int class_num = 0;
    int sample_num = 0;
    err = libmaix_classifier_load(&classifier_obj, path, ((ModelObject*)model)->nn, &feature_len, &input_w, &input_h, &class_num, &sample_num);
    if(err != LIBMAIX_ERR_NONE)
    {
        PyErr_Format(PyExc_Exception, "app classifier save fail, code: %d", err);
        return NULL;
    }

    /* new object */
    PyObject* call_args = PyList_New(0);
    PyObject* call_keywords = PyDict_New();
    PyObject* o_classifier_obj = PyMaix_NN_app_Classifier_Type.tp_new(&PyMaix_NN_app_Classifier_Type, call_args, call_keywords);
    Py_DECREF(call_args);Py_DECREF(call_keywords);
    if(!o_classifier_obj)
    {
        PyErr_NoMemory();
        return NULL;
    }
    App_Classifier_Object* o = (App_Classifier_Object*)o_classifier_obj;
    o->nn_model = (ModelObject*)model;
    o->class_num = class_num;
    o->sample_num = sample_num;
    o->classifier_obj = classifier_obj;
    o->feature_len = feature_len;
    o->input_w = input_w;
    o->input_h = input_h;
    o->init = true;
    return o_classifier_obj;
}



static PyMethodDef _maix_nn_app_methods[] = {
    {"classifier_load", (PyCFunction)app_classifier_load, METH_VARARGS | METH_KEYWORDS, app_classifier_load_doc},
    {NULL, NULL, 0, NULL}
};


struct PyModuleDef maix_nn_app_module = {
    PyModuleDef_HEAD_INIT,
    "_app",              /* Module name */
    _maix_nn__app_doc,	/* Module _maixMethods */
    -1,			    /* size of per-interpreter state of the module, size of per-interpreter state of the module,*/
    _maix_nn_app_methods,
};


