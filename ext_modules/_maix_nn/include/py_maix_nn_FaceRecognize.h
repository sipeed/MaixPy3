#ifndef _CPY_MAIX_NN_FACERECOGNIZE_H_
#define _CPY_MAIX_NN_FACERECOGNIZE_H_

#ifdef  __cplusplus
extern "C" {
#endif

#include <Python.h>
#include "libmaix_nn.h"
#include "py_maix_nn.h"

typedef struct
{
    PyObject_HEAD;
    ModelObject* detect_model;
    ModelObject* fea_model;
    int          fea_len;
    void*        recognize_obj;
    float*       fea_temp;
    float*       fea_temp2;
    int          input_w;
    int          input_h;
    int          input_c;
    bool init;
    PyObject*    pillow_image_module;
} App_FaceRecognize_Object;

extern PyTypeObject PyMaix_NN_app_FaceRecognize_Type;

#ifdef  __cplusplus
}
#endif

#endif

