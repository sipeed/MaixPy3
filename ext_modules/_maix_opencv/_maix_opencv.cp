#include "opencv2/core.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/videoio.hpp"
#include "opencv2/imgcodecs.hpp"
#include <iostream>
// #ifdef  __cplusplus
// extern "C" {
// #endif
#define PY_SSIZE_T_CLEAN 
#include <Python.h>
#define _MAIX_OPENCV_VERSION_ "0.0.1"
#define _MAIX_OPENCV_NAME_ "_maix_opencv"

PyDoc_STRVAR(_maix_opencv_doc, "MaixPy Python3 maix_opencv module.\n");

typedef enum image_bpp
{
    IMAGE_BPP_BINARY,       // BPP = 0
    IMAGE_BPP_GRAYSCALE,    // BPP = 1
    IMAGE_BPP_RGB565,       // BPP = 2
    IMAGE_BPP_BAYER,        // BPP = 3
    IMAGE_BPP_RGB888,         // BPP = 4
}
image_bpp_t;
typedef struct image {
    int w;
    int h;
    int bpp;
    uint8_t *data;
}image_t;
typedef struct color_thresholds_list_lnk_data
{
    uint8_t LMin, LMax; // or grayscale
    uint8_t AMin, AMax;
    uint8_t BMin, BMax;
}
color_thresholds_list_lnk_data_t;


//这里的thresholds准备采用兼容操作,主要为了向前兼容
static PyObject *
find_blob_lab(PyObject *self, PyObject *args, PyObject *keywds)
{
    PyObject *img = NULL;
    PyObject *thresholds = NULL;
    bool invert = 0;
    PyObject *roi = NULL;
    int x_stride = 2;
    int y_stride = 1;
    int area_threshold = 10;
    int pixels_threshold = 10;
    bool merge = 0;
    int margin = 0;
    static char *kwlist[] = {"img","thresholds", "invert", "roi", "x_stride","y_stride","area_threshold","pixels_threshold","merge","margin", NULL};
    if (!PyArg_ParseTupleAndKeywords(args, keywds, "OO|iOiiiiii", kwlist,
                                     &img, &thresholds, &invert, &roi, &x_stride, &y_stride, &area_threshold, &pixels_threshold, &merge, &margin))
        return NULL;
    return Py_BuildValue("i", 0);
/*

    PyObject *input_img_o = NULL;
    char* input_img_bytes = NULL;

    image_t img_data;
    img_data.data = NULL;


    if(PyBytes_Check(img))
    {                                           //默认240 * 240
        img_data.data = PyBytes_AsString(img);
        if(img_data.data == NULL)
        {
            return NULL;
        }
        img_data.bpp = IMAGE_BPP_RGB888;
        img_data.w = 240;
        img_data.h = 240;
    }
    else if(strstr(img->ob_type->tp_name, "Image") >= 0)
    {                                           //检查是否是pillow类型
        input_img_o = PyObject_CallMethod(img, "tobytes", NULL);
        // Py_INCREF(input_img_o);
        if(input_img_o == NULL)
        {
            PyErr_SetString(PyExc_Exception, "tobytes error");
            return NULL;
        }
        img_data.data = PyBytes_AsString(input_img_o);
        Py_DECREF(input_img_o);
        if(img_data.data == NULL)
        {
            return NULL;
        }
        img_data.bpp = IMAGE_BPP_RGB888;
        img_data.w = 240;
        img_data.h = 240;

    }
    else if(PyTuple_Check(img))
    {                                           //检查是否元组
        return NULL;

    }
    else if(PyList_Check(img))
    {                                           //检查是否列表
        return NULL;
    }
    vector<color_thresholds_list_lnk_data_t> thrms;
    if (PyList_Check(thresholds))
    {
        color_thresholds_list_lnk_data_t thrm;
        int thr_len = PyList_Size(thresholds);
        if (thr_len == 0)
        {
            return NULL;
        }
        for (int i = 0; i < thr_len; i++)
        {
            PyObject *items = PyList_GetItem(thresholds, i);        //借用

            int thrs = PyTuple_Size(items);
            switch (thrs)
            {
            case 6:
                thrm.LMin = uint8_t(PyLong_AsLong(PyTuple_GetItem(items, 0)) * 255 / 100) ;   //借用
                thrm.LMax = uint8_t(PyLong_AsLong(PyTuple_GetItem(items, 3)) * 255 / 100) ;
                thrm.AMin = uint8_t(PyLong_AsLong(PyTuple_GetItem(items, 1)) + 128) ;
                thrm.AMax = uint8_t(PyLong_AsLong(PyTuple_GetItem(items, 4)) + 128) ;
                thrm.BMin = uint8_t(PyLong_AsLong(PyTuple_GetItem(items, 2)) + 128) ;
                thrm.BMax = uint8_t(PyLong_AsLong(PyTuple_GetItem(items, 5)) + 128) ;
                break;
            case 2:
                thrm.LMin = PyLong_AsLong(PyTuple_GetItem(items, 0));
                thrm.LMax = PyLong_AsLong(PyTuple_GetItem(items, 1));
                break;
            default:
                break;
            }
            thrms.push_back(thrm);
        }
    }
    else
    {
        return NULL;
    }
        //    Py_DECREF(thr0);
        //     Py_DECREF(thrs);


    // if((Py_ssize_t)(self->input_w * self->input_h * 3) != PyBytes_Size(o_input_bytes))
    // {
    //     PyErr_Format(PyExc_ValueError, "input shape error, need: %d (%d, %d, %d), but: %d", self->input_w * self->input_h * 3, self->input_h, self->input_w, 3, PyBytes_Size(o_input_bytes));
    //     goto err0;
    // }
    // char* input_bytes = PyBytes_AsString(o_input_bytes);
    // if(input_bytes == NULL)
    // {
    //     PyErr_SetString(PyExc_ValueError, "get bytes data error");
    //     goto err0;
    // }


    Mat opencv_img(img_data.w, img_data.h, CV_8UC3, img_data.data);
    cvtColor(opencv_img, opencv_img, COLOR_RGB2Lab);



    Mat mask(img_data.w, img_data.h, CV_8UC1，Scalar(0) );//单通道
    Mat mask1;

    for (int i=0;i<thrms.size();i++)
    {
      inRange(lab, Scalar(thrms[i].LMin, thrms[i].AMin, thrms[i].BMin), Scalar(thrms[i].LMax, thrms[i].AMax, thrms[i].BMax), mask1);
      mask = mask + mask1;
    }


*/
    
    // // Mat se = getStructuringElement(MORPH_RECT, Size(5, 5), Point(-1, -1));
    // // morphologyEx(mask, mask, MORPH_OPEN, se);                                    //开运算


    // vector<vector<Point>> contours;
    // vector<Vec4i> hiearchy;
    // findContours(mask, contours, hiearchy, RETR_EXTERNAL, CHAIN_APPROX_NONE);
    // if (contours.size() == 0)
    // {
    //   return return_val;
    // }

    // for (int i = 0; i < contours.size(); i++)
    // {
    //   py::dict val;
    //   Rect rects = boundingRect(contours[i]);
    //   val["x"] = int(rects.x);
    //   val["y"] = int(rects.y);
    //   val["w"] = int(rects.width);
    //   val["h"] = int(rects.height);

    //   val["pixels"] = int(contourArea(contours[i]));
    //   val["cx"] = int(rects.x + rects.width / 2);
    //   val["cy"] = int(rects.y + rects.height / 2);

    //   if (tilt)
    //   {
    //     RotatedRect minRect = minAreaRect(contours[i]);
    //     Point2f rect_points[4];
    //     minRect.points(rect_points);
    //     py::tuple tmp3 = py::make_tuple(rect_points[0].x, rect_points[0].y, rect_points[1].x, rect_points[1].y, rect_points[2].x, rect_points[2].y, rect_points[3].x, rect_points[3].y);
    //     val["tilt_Rect"] = tmp3;
    //     int tmp1 = Distance(int(rect_points[0].x), int(rect_points[0].y), int(rect_points[1].x), int(rect_points[1].y));
    //     int tmp2 = Distance(int(rect_points[0].x), int(rect_points[0].y), int(rect_points[3].x), int(rect_points[3].y));
    //     float x1, y1, k;
    //     if (tmp1 > tmp2)
    //     {
    //       x1 = rect_points[1].x - rect_points[0].x;
    //       y1 = rect_points[1].y - rect_points[0].y;
    //       k = atan(y1 / x1);
    //     }
    //     else
    //     {
    //       x1 = rect_points[3].x - rect_points[0].x;
    //       y1 = rect_points[3].y - rect_points[0].y;
    //       k = atan(y1 / x1);
    //     }

    //     val["rotation"] = k;
    //   }
    //   return_val.append(val);
    // }
    // return return_val;
}



















static PyMethodDef _maix_opencv_methods[] = {
    // {"help", (PyCFunction)_maix_nn_help, METH_NOARGS, _maix_opencv_doc},
    {"find_blob_lab", (PyCFunction)find_blob_lab, METH_VARARGS | METH_KEYWORDS, _maix_opencv_doc},
    {NULL, NULL, 0, NULL}
};

static struct PyModuleDef _maix_opencv_module = {
    PyModuleDef_HEAD_INIT,
    "_maix_opencv",         /* Module name */
    _maix_opencv_doc,	/* Module _maixMethods */
    -1,			    /* size of per-interpreter state of the module, size of per-interpreter state of the module,*/
    _maix_opencv_methods,
};

PyMODINIT_FUNC PyInit__maix_opencv(void)
{

    PyObject *module;

    module = PyModule_Create(&_maix_opencv_module);


    PyObject *version = PyUnicode_FromString(_MAIX_OPENCV_VERSION_);


    // /* Set module version */
    PyObject *dict = PyModule_GetDict(module);
    PyDict_SetItemString(dict, "__version__", version);
    Py_DECREF(version);

    // /* Add class*/
    // // PyModule_AddObject(module, "Model", SpamError);

    // /* init object(default value) */
    // if (PyType_Ready(&PyMaix_NN_Model_Type) < 0) {
    //     return NULL;
    // }
    // if (PyType_Ready(&PyMaix_NN_Decoder_Yolo2_Type) < 0) {
    //     return NULL;
    // }
    // if (PyType_Ready(&PyMaix_NN_app_Classifier_Type) < 0) {
    //     return NULL;
    // }

    // /* Add maix.nn.F module*/
    // PyObject *functional_module = PyModule_Create(&maix_nn_functional_module);    
    // PyModule_AddObject(module, "F", functional_module);
    // Py_INCREF(functional_module);
    // PyModule_AddObject(module, "functional", functional_module);

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
    // PyModule_AddObject(module, "_app", app_module);

    return module;
}






















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
    if (PyType_Ready(&PyMaix_NN_Decoder_Yolo2_Type) < 0) {
        return NULL;
    }
    if (PyType_Ready(&PyMaix_NN_app_Classifier_Type) < 0) {
        return NULL;
    }

    /* Add maix.nn.F module*/
    PyObject *functional_module = PyModule_Create(&maix_nn_functional_module);    
    PyModule_AddObject(module, "F", functional_module);
    Py_INCREF(functional_module);
    PyModule_AddObject(module, "functional", functional_module);

    /* Add maix.nn.decoder module,
       add maix.nn.decoder.Yolo2 class*/
    PyObject *decoder_module = PyModule_Create(&maix_nn_decoder_module);
    Py_INCREF(&PyMaix_NN_Decoder_Yolo2_Type);
    PyModule_AddObject(decoder_module, "Yolo2", (PyObject*)&PyMaix_NN_Decoder_Yolo2_Type);
    PyModule_AddObject(module, "decoder", decoder_module);

    /* Add maix.nn._app module,
       add maix.nn._app.classifier class*/
    PyObject *app_module = PyModule_Create(&maix_nn_app_module);
    Py_INCREF(&PyMaix_NN_app_Classifier_Type);
    PyModule_AddObject(app_module, "Classifier", (PyObject*)&PyMaix_NN_app_Classifier_Type);
    PyModule_AddObject(module, "_app", app_module);

    return module;
}



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

PyMODINIT_FUNC PyInit__maix_opencv(void)
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
    if (PyType_Ready(&PyMaix_NN_Decoder_Yolo2_Type) < 0) {
        return NULL;
    }
    if (PyType_Ready(&PyMaix_NN_app_Classifier_Type) < 0) {
        return NULL;
    }

    /* Add maix.nn.F module*/
    PyObject *functional_module = PyModule_Create(&maix_nn_functional_module);    
    PyModule_AddObject(module, "F", functional_module);
    Py_INCREF(functional_module);
    PyModule_AddObject(module, "functional", functional_module);

    /* Add maix.nn.decoder module,
       add maix.nn.decoder.Yolo2 class*/
    PyObject *decoder_module = PyModule_Create(&maix_nn_decoder_module);
    Py_INCREF(&PyMaix_NN_Decoder_Yolo2_Type);
    PyModule_AddObject(decoder_module, "Yolo2", (PyObject*)&PyMaix_NN_Decoder_Yolo2_Type);
    PyModule_AddObject(module, "decoder", decoder_module);

    /* Add maix.nn._app module,
       add maix.nn._app.classifier class*/
    PyObject *app_module = PyModule_Create(&maix_nn_app_module);
    Py_INCREF(&PyMaix_NN_app_Classifier_Type);
    PyModule_AddObject(app_module, "Classifier", (PyObject*)&PyMaix_NN_app_Classifier_Type);
    PyModule_AddObject(module, "_app", app_module);

    return module;
}









// #ifdef  __cplusplus
// }
// #endif
