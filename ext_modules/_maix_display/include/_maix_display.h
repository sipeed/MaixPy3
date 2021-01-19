#ifndef _MAIX_DISPLAY_H
#define _MAIX_DISPLAY_H

#ifdef  __cplusplus
extern "C" {
#endif

#include <Python.h>

/* Macros needed for Python 3 */
#ifndef PyInt_Check
#define PyInt_Check PyLong_Check
#define PyInt_FromLong PyLong_FromLong
#define PyInt_AsLong PyLong_AsLong
#define PyInt_Type PyLong_Type
#endif

PyDoc_STRVAR(VirtualDisplay_name, "VirtualDisplay");
extern PyTypeObject VirtualDisplayObjectType;

// #define V831Display
#ifdef V831Display
PyDoc_STRVAR(V831Display_name, "V831Display");
extern PyTypeObject V831DisplayObjectType;
#endif

#ifdef  __cplusplus
}
#endif

#endif

