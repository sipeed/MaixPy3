#ifndef __MAIX_IMAGE_H__
#define __MAIX_IMAGE_H__



#include <iostream>
#include "any.hpp"
#include <sstream>

#include <pybind11/pybind11.h>
#include <pybind11/embed.h>
#include <pybind11/stl.h>
#include <vector>
#include <list>
#include <array>
#include <map>
#include <set>
#include <typeinfo>

#include "opencv2/core.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/videoio.hpp"
#include "opencv2/imgcodecs.hpp"

#include <opencv2/opencv.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/imgcodecs/legacy/constants_c.h>
#include "opencv2/core/types_c.h"

#include "libmaix_cv_image.h"
#include "libmaix_disp.h"

namespace py = pybind11;



class _maix_image
{
private:
    any py_to_pram[4][3];
    int get_to(string &mode);


public:
    libmaix_image_t *img;
    int _maix_image_width;
    int _maix_image_height;
    std::string _maix_image_type;
    int _maix_image_size;
    _maix_image();
    ~_maix_image();
    void v_init();
    void v_close();

    std::string str__();

    void _show();
    int _save(string file_path, string format);
    _maix_image &_new(std::vector<int> size, std::vector<int> color, std::string mode);
    _maix_image &_load(py::bytes &data, std::vector<int> size, std::string mode);
    // _maix_image &_load_freetype();
    // _maix_image &_tobytes();
    // _maix_image &_resize();
    // _maix_image &_rotate();
    // _maix_image &_crop();
    // _maix_image &_convert();
    // _maix_image &_draw_ellipse();
    // _maix_image &_draw_string();
    // _maix_image &_draw_circle();
    // _maix_image &_draw_rectangle();
    // _maix_image &_draw_line();
};















#endif