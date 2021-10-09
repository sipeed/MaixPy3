#ifndef _MAIX_DISPLAY_H
#define _MAIX_DISPLAY_H

#include <iostream>
#include <pybind11/pybind11.h>
#include <pybind11/embed.h>
#include <pybind11/stl.h>
#include <vector>
#include <list>
#include <array>
#include <map>
#include <set>
#ifdef  __cplusplus
extern "C" {
#endif
// #include <stdio.h>
#include "libmaix_disp.h"

#define debug_line printf("%s:%d %s %s %s \r\n", __FILE__, __LINE__, __FUNCTION__, __DATE__, __TIME__)
#ifdef  __cplusplus
}
#endif

typedef struct image {
    int w;
    int h;
    int bpp;
    uint8_t *data;
} image_t;
typedef enum
{
    LIBMAIX_IMAGE_MODE_INVALID = 0,
    LIBMAIX_IMAGE_MODE_BINARY,
    LIBMAIX_IMAGE_MODE_GRAY  ,
    LIBMAIX_IMAGE_MODE_RGB888,          // supported
    LIBMAIX_IMAGE_MODE_RGB565,
    LIBMAIX_IMAGE_MODE_RGBA8888,
    LIBMAIX_IMAGE_MODE_YUV420SP_NV21,   // supported
}image_mode_t;
// #define R329Display
// #define V831Display

#ifdef R329Display
class r329_Display
{
private:
public:
    libmaix_disp_t *disp;
    image_t fb_img;
    int width ;
    int height ;
    r329_Display(int w,int h);
    ~r329_Display();
    pybind11::object draw(pybind11::object py_img,int img_w,int img_h);
    void close();
    void __enter__();
    void __exit__();
};
#endif

#ifdef V831Display
class v831_Display
{
private:
    
public:
    libmaix_disp_t *disp;
    int width ;
    int height ;
    v831_Display(int w,int h);
    ~v831_Display();
    pybind11::object draw(pybind11::object py_img,int img_w,int img_h);
    void close();
    void __enter__();
    void __exit__();
};
#endif

// #ifdef  __cplusplus
// extern "C" {
// #endif

// #include <Python.h>

// /* Macros needed for Python 3 */
// #ifndef PyInt_Check
// #define PyInt_Check PyLong_Check
// #define PyInt_FromLong PyLong_FromLong
// #define PyInt_AsLong PyLong_AsLong
// #define PyInt_Type PyLong_Type
// #endif

// PyDoc_STRVAR(VirtualDisplay_name, "VirtualDisplay");
// extern PyTypeObject VirtualDisplayObjectType;

// // #define V831Display
// #ifdef V831Display
// PyDoc_STRVAR(V831Display_name, "V831Display");
// extern PyTypeObject V831DisplayObjectType;
// #endif

// // #define R329Display
// #ifdef R329Display
// PyDoc_STRVAR(R329Display_name, "R329Display");
// extern PyTypeObject R329DisplayObjectType;
// #endif

// #ifdef  __cplusplus
// }
// #endif

#endif
