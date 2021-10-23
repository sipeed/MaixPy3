#ifndef _MAIX_DISPLAY_H
#define _MAIX_DISPLAY_H

#include <pybind11/pybind11.h>
#include <pybind11/embed.h>
#include <pybind11/stl.h>
#ifdef  __cplusplus
extern "C" {
#endif
// #include <stdio.h>
#include "libmaix_disp.h"
#include "libmaix_cam.h"
#define debug_line printf("%s:%d %s %s %s \r\n", __FILE__, __LINE__, __FUNCTION__, __DATE__, __TIME__)
#ifdef  __cplusplus
}
#endif


class _Display
{
private:
public:
    libmaix_disp_t *disp;
    int width ;
    int height ;
    int bpp;
    libmaix_image_t *display_image;
    _Display();
    ~_Display();
    pybind11::object draw(pybind11::object py_img,int img_w,int img_h,int mode);
    void close();
    void __enter__();
    void __exit__();

};

#endif
