
#include "Python.h"
#include "py_maix_nn.h"

#include <math.h>
#include <string.h>

static void maix_nn_functional_softmax_float32(float *data, int n, float *out, int stride)
{
    int i;
    float sum = 0;
    float largest_i = data[0];
    if(!out)
    {
        out = data;
    }

    for (i = 0; i < n; ++i)
    {
        if (data[i * stride] > largest_i)
            largest_i = data[i * stride];
    }
    for (i = 0; i < n; ++i)
    {
        float value = expf(data[i * stride] - largest_i);
        sum += value;
        out[i * stride] = value;
    }
    for (i = 0; i < n; ++i)
	{
        out[i * stride] /= sum;
	}
}

static void maix_nn_functional_softmax_double(double *data, int n, double *out, int stride)
{
    int i;
    double sum = 0;
    double largest_i = data[0];
    if(!out)
    {
        out = data;
    }

    for (i = 0; i < n; ++i)
    {
        if (data[i * stride] > largest_i)
            largest_i = data[i * stride];
    }
    for (i = 0; i < n; ++i)
    {
        double value = expf(data[i * stride] - largest_i);
        sum += value;
        out[i * stride] = value;
    }
    for (i = 0; i < n; ++i)
	{
        out[i * stride] /= sum;
	}
}

PyDoc_STRVAR(functional_softmax_doc, "softmax function.\n");
static PyObject* functional_softmax(PyObject *self, PyObject *args, PyObject *kw_args)
{
    PyObject *o_input = NULL;
    int dim = -1;
    static char *kwlist[] = {"input", "dim", NULL};
    /* Get the buffer's memory */
    if (!PyArg_ParseTupleAndKeywords(args, kw_args, "O|$p:forward", kwlist,
                                     &o_input, &dim))
    {
        return NULL;
    }
    if(dim >=0 ) //TODO:
    {
        PyErr_SetString(PyExc_NotImplementedError, "not support dim arg yet");
        return NULL;
    }
    PyObject* m_numpy = PyImport_ImportModule("numpy");
    if(!m_numpy)
    {
        PyErr_SetString(PyExc_EnvironmentError, "need numpy module");
        return NULL;
    }
    PyObject* o_ndarray = PyObject_GetAttrString(m_numpy, "ndarray");
    if(o_ndarray == NULL)
    {
        return NULL;
    }
    if((void*)o_input->ob_type == (void*)o_ndarray)
    {
        Py_DECREF(o_ndarray);
        int dtype = -1;
        PyObject* o_dtype = PyObject_GetAttrString(o_input, "dtype");
        if(o_dtype == NULL)
        {
            return NULL;
        }
        PyObject* o_dtype_name = PyObject_GetAttrString(o_dtype, "name");
        Py_DECREF(o_dtype);
        if(o_dtype_name == NULL)
        {
            return NULL;
        }
        char* dtype_name = PyUnicode_DATA(o_dtype_name);
        if(strcmp(dtype_name, "float32") == 0)
        {
            dtype = 1;
        }
        else if(strcmp(dtype_name, "float64") == 0)
        {
            dtype = 2;
        }
        else
        {
            Py_DECREF(o_dtype_name);
            PyErr_SetString(PyExc_ValueError, "only support float32 and float64 dtype");
            return NULL;
        }
        PyObject* o_bytes = PyObject_CallMethod(o_input, "tobytes", NULL);
        if(o_bytes == NULL)
        {
            Py_DECREF(o_dtype_name);
            return NULL;
        }
        Py_ssize_t bytes_len = PyBytes_Size(o_bytes);
        char* bytes = PyBytes_AsString(o_bytes);
        switch(dtype)
        {
            case 1: //float32
                maix_nn_functional_softmax_float32((float*)bytes, bytes_len/4, NULL, 1);
                break;
            case 2: //float64
                maix_nn_functional_softmax_double((double*)bytes, bytes_len/4, NULL, 1);
                break;
            default:
                assert(false); // should never here
                return NULL;
        }
        PyObject *call_args = Py_BuildValue("(O)", o_bytes);
        PyObject *call_keywords = PyDict_New();
        PyDict_SetItemString(call_keywords, "dtype", o_dtype_name);
        PyObject* o_result_numpy = PyObject_Call(PyObject_GetAttrString(m_numpy, "frombuffer"), call_args, call_keywords);
        Py_DECREF(call_args);
        Py_DECREF(call_keywords);
        Py_DECREF(o_dtype_name);
        Py_DECREF(o_bytes);
        return o_result_numpy;
    }
    else
    {
        Py_DECREF(o_ndarray);
        PyErr_SetString(PyExc_NotImplementedError, "only support numpy object");
        return NULL;
    }
}

static PyMethodDef _maix_nn_functional_methods[] = {
    {"softmax", (PyCFunction)functional_softmax, METH_VARARGS | METH_KEYWORDS, functional_softmax_doc},
    {NULL, NULL, 0, NULL}
};


struct PyModuleDef maix_nn_functional_module = {
    PyModuleDef_HEAD_INIT,
    "functional",              /* Module name */
    _maix_nn_functional_doc,	/* Module _maixMethods */
    -1,			    /* size of per-interpreter state of the module, size of per-interpreter state of the module,*/
    _maix_nn_functional_methods,
};


