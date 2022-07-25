#ifndef _CPY_MAIX_NN_CLASSIFIER_H_
#define _CPY_MAIX_NN_CLASSIFIER_H_

#ifdef  __cplusplus
extern "C" {
#endif

#include <Python.h>
#include "libmaix_nn.h"
#include "py_maix_nn.h"

typedef struct
{
    PyObject_HEAD;
    ModelObject* nn_model;
    int class_num;
    int sample_num;
    int feature_len;
    int input_w;
    int input_h;
    void* classifier_obj;
    bool init;
} App_Classifier_Object;

extern PyTypeObject PyMaix_NN_app_Classifier_Type;

#ifdef  __cplusplus
}
#endif

#endif

