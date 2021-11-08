#include "maix_image.h"

base_img::base_img()
{
    this->py_to_pram[0][0] = LIBMAIX_IMAGE_MODE_GRAY;
    this->py_to_pram[0][1] = 1;
    this->py_to_pram[0][2] = CV_8UC1;
    this->py_to_pram[0][3] = "L";

    this->py_to_pram[1][0] = LIBMAIX_IMAGE_MODE_RGB565;
    this->py_to_pram[1][1] = 2;
    this->py_to_pram[1][2] = CV_8UC2;
    this->py_to_pram[1][3] = "RGB16";

    this->py_to_pram[2][0] = LIBMAIX_IMAGE_MODE_RGB888;
    this->py_to_pram[2][1] = 3;
    this->py_to_pram[2][2] = CV_8UC3;
    this->py_to_pram[2][3] = "RGB";

    this->py_to_pram[3][0] = LIBMAIX_IMAGE_MODE_RGBA8888;
    this->py_to_pram[3][1] = 4;
    this->py_to_pram[3][2] = CV_8UC4;
    this->py_to_pram[3][3] = "RGBA";
}
int base_img::get_to(std::string &mode)
{
    if (mode == "L")
    {
        return 0;
    }
    else if (mode == "RGB16")
    {
        return 1;
    }
    else if (mode == "RGB")
    {
        return 2;
    }
    else if (mode == "RGBA")
    {
        return 3;
    }
    else
    {
        PyErr_SetString(PyExc_RuntimeError, "libmaix_disp_create err!");
        throw py::error_already_set();
        return -1;
    }
}

void _maix_image::v_init()
{
    this->_img = NULL;
    LOG_INFO << "init base img!";
}
void _maix_image::v_close()
{
    LOG_INFO << "start!";
    if (this->_img)
    {
        libmaix_image_destroy(&this->_img);
        LOG_INFO << "libmaix_image_destroy success";
    }
    this->_maix_image_width = 0;
    this->_maix_image_height = 0;
    this->_maix_image_size = 0;
}

_maix_image::_maix_image()
{
    LOG_INFO << "_maix_image creat!";
    this->v_init();
}
_maix_image::~_maix_image()
{
    LOG_INFO << "~_maix_image del";
    this->v_close();
}
std::string _maix_image::str__()
{
    std::string info_str;
    std::stringstream ss;
    ss << "{\"w\":" << this->_maix_image_width << ", \"h\":" << this->_maix_image_height << ", \"type\"=" << this->_maix_image_type << ", \"size\":" << this->_maix_image_size << "}";
    info_str = ss.str();
    return info_str;
}
int _maix_image::len__()
{
    return this->_maix_image_size;
}

_maix_image &_maix_image::_new(std::vector<int> size, std::vector<int> color, std::string mode)
{
    LOG_INFO << "_maix_image::_new start!";
    this->v_close();
    this->_maix_image_type = mode;
    this->_maix_image_width = size[0];
    this->_maix_image_height = size[1];
    this->_maix_image_size = this->_maix_image_width * this->_maix_image_height * any_cast<int>(this->py_to_pram[this->get_to(this->_maix_image_type)][1]);
    this->_img = libmaix_image_create(size[0], size[1], any_cast<libmaix_image_mode_t>(this->py_to_pram[this->get_to(this->_maix_image_type)][0]), LIBMAIX_IMAGE_LAYOUT_HWC, NULL, true);
    if (this->_img)
    {
        libmaix_cv_image_draw_rectangle(this->_img, 0, 0, size[0], size[1], MaixColor(color[0], color[1], color[2]), -1);
        LOG_INFO << "this->_img init success!";
    }
    else
    {
        this->v_close();
        this->_maix_image_size = 0;
        LOG_WARN << "this->_img creat fail!";
    }
    return *this;
}

_maix_image &_maix_image::_load(py::object data, std::vector<int> size, std::string mode)
{
    this->v_close();
    if (py::isinstance<py::bytes>(data))
    {
        this->_maix_image_type = mode;
        this->_maix_image_width = size[0];
        this->_maix_image_height = size[1];
        this->_img = libmaix_image_create(this->_maix_image_width, this->_maix_image_height, any_cast<libmaix_image_mode_t>(py_to_pram[this->get_to(this->_maix_image_type)][0]), LIBMAIX_IMAGE_LAYOUT_HWC, NULL, true);
        this->_maix_image_size = this->_maix_image_width * this->_maix_image_height * any_cast<int>(py_to_pram[this->get_to(this->_maix_image_type)][1]);
        if (this->_img)
        {
            std::string tmp = data.cast<std::string>();
            memcpy(this->_img->data, tmp.c_str(), this->_maix_image_size);
        }
        else
        {
            LOG_WARN << "img load fail!" ;
            this->v_close();
            this->_maix_image_size = 0;
        }
        return *this;
    }
    else
    {
        auto maix_image_type = py::module::import("_maix_Image").attr("Image");
        if (py::isinstance(data, maix_image_type))
        {
            auto tobytes = data.attr("tobytes");                                //获取tobytes函数
            auto tmp_img_bytes = tobytes().cast<std::string>();
            this->_maix_image_width = data.attr("width").cast<int>();
            this->_maix_image_height = data.attr("height").cast<int>();
            this->_maix_image_size = data.attr("size").cast<int>();
            this->_maix_image_type = data.attr("mode").cast<std::string>();
            this->_img = libmaix_image_create(this->_maix_image_width, this->_maix_image_height, any_cast<libmaix_image_mode_t>(py_to_pram[this->get_to(this->_maix_image_type)][0]), LIBMAIX_IMAGE_LAYOUT_HWC, NULL, true);
            if (this->_img)
            {
                memcpy(this->_img->data, tmp_img_bytes.c_str(), this->_maix_image_size);
            }
            else
            {
                LOG_WARN << "img load fail!" ;
                this->v_close();
                this->_maix_image_size = 0;
            }
            return *this;
        }
#ifdef enable_PIL
        auto PIL_ = py::module::import("PIL.Image").attr("Image");
        if (py::isinstance(data, PIL_))
        {
            auto tobytes = PIL_.attr("tobytes");                                //获取tobytes函数
            auto img_bytes = tobytes(data);                                     //转化成python对象的img_bytes
            auto img_tmp = img_bytes.cast<std::string>();                            //转换成c++的string
            auto img_size = data.attr("size").cast<std::vector<int>>();
            this->_maix_image_type = data.attr("mode").cast<std::string>();
            this->_maix_image_width = img_size[0];
            this->_maix_image_height = img_size[1];
            this->_img = libmaix_image_create(this->_maix_image_width, this->_maix_image_height, any_cast<libmaix_image_mode_t>(py_to_pram[this->get_to(this->_maix_image_type)][0]), LIBMAIX_IMAGE_LAYOUT_HWC, NULL, true);
            this->_maix_image_size = this->_maix_image_width * this->_maix_image_height * any_cast<int>(py_to_pram[this->get_to(this->_maix_image_type)][1]);
            if (this->_img)
            {
                memcpy(this->_img->data, img_tmp.c_str(), this->_maix_image_size);
            }
            else
            {
                LOG_WARN << "img load fail!" ;
                this->v_close();
                this->_maix_image_size = 0;
            }
            return *this;
        }  
#endif
    }
        return *this;
}

_maix_image &_maix_image::_open_file(std::string path)
{
    LOG_INFO << "clear img" ;
    this->v_close();
    LOG_INFO << "call libmaix_cv_image_open_file img" ;
    libmaix_image_t *tmp_img = NULL;
    if(libmaix_cv_image_open_file(&tmp_img,path.c_str()) != 0)
    {
        this->v_close();
        LOG_WARN << "open file err! " << path << "exec or don't read !";
        std::cout << "open file err! " << path << "exec or don't read !" << std::endl;
        return *this;
    }
    this->_img = tmp_img;
    LOG_INFO << "call libmaix_cv_image_open_file success!";
    this->_maix_image_width = this->_img->width;
    this->_maix_image_height = this->_img->height;
    this->_maix_image_type = "RGB";
    this->_maix_image_size = this->_maix_image_width * this->_maix_image_height * 3;
    return *this;
}



py::object _maix_image::_get_to(std::string im)
{
    if(im == "maix_Image")
    {
        auto _maix_Image_ = py::module::import("_maix_Image");
        auto py_Image = _maix_Image_.attr("Image");
        auto py_img_obj = py_Image();
        auto py_fun_load = py_img_obj.attr("load");
        std::string tmp((const char*)this->_img->data, this->_maix_image_size);
        py_fun_load(tmp, std::vector<int>{this->_maix_image_width, this->_maix_image_height}, this->_maix_image_type);
        return py_img_obj;
    }
#ifdef enable_opencv
    else if (im == "opencv")
    {

    }
#endif
#ifdef enable_PIL
    else if (im == "PIL")
    {
        if(this->_maix_image_type != "RGB16")
        {
            auto _PIL_ = py::module::import("PIL.Image");
            auto frombytes = _PIL_.attr("frombytes");
            std::string tmp((const char*)this->_img->data, this->_maix_image_size);
            py::tuple sizek = py::make_tuple(this->_maix_image_width, this->_maix_image_height);
            auto PIL_img = frombytes(this->_maix_image_type, sizek, tmp);
            return PIL_img;
        }
        else
        {
            return py::none();
        }
    }
#endif
}





_maix_image &_maix_image::_clear()
{
    LOG_INFO << "img clear!";
    v_close();
}

void _maix_image::_show()
{
    if(this->_img == NULL)
    {
        return;
    }
    auto _maix_display = py::module::import("_maix_display");
    auto Display = _maix_display.attr("Display");
    auto show = Display();
    auto draw__ = show.attr("draw");

    if(this->_maix_image_width != 240 || this->_maix_image_height != 240)
    {
        this->_resize(240,240);
    }
    py::bytes tmp((const char *)this->_img->data,this->_maix_image_size);
    draw__(tmp);
}

int _maix_image::_save(std::string file_path, std::string format)
{
    if (this->_img)
    {
        libmaix_cv_image_draw_image_save(this->_img, file_path.c_str());
    }
    return 0;
}

py::bytes _maix_image::_tobytes()
{
    if (this->_img)
    {
        return py::bytes((const char *)this->_img->data, this->_maix_image_size);
    }
    py::bytes return_val;
    return return_val;
}

_maix_image &_maix_image::_resize(int w, int h)
{

    if (this->_img)
    {
        libmaix_image_t *tmp = libmaix_image_create(w, h, this->_img->mode, LIBMAIX_IMAGE_LAYOUT_HWC, NULL, true);
        if (tmp)
        {
            cv::Mat src(this->_img->width, this->_img->height, any_cast<int>(py_to_pram[this->get_to(this->_maix_image_type)][2]), this->_img->data);
            cv::Mat dst(w, h, any_cast<int>(py_to_pram[this->get_to(this->_maix_image_type)][2]), tmp->data);
            cv::resize(src, dst, cv::Size(w, h));
            libmaix_image_destroy(&this->_img), this->_img = tmp;
            this->_maix_image_height = h;
            this->_maix_image_width = w;
            this->_maix_image_size = w * h * any_cast<int>(py_to_pram[this->get_to(this->_maix_image_type)][1]);
        }
        else
        {
            libmaix_image_destroy(&tmp);
        }
    }
    return *this;
}

_maix_image &_maix_image::_draw_line(int x1, int y1, int x2, int y2, std::vector<int> color, int thickness)
{
    if (this->_img)
    {
        libmaix_cv_image_draw_line(this->_img, x1, y1, x2, y2, MaixColor(color[0], color[1], color[2]), thickness);
    }
    return *this;
}

_maix_image &_maix_image::_draw_rectangle(int x, int y, int w, int h, std::vector<int> color, int thickness)
{
    LOG_INFO << "start";
    if (this->_img)
    {
        libmaix_cv_image_draw_rectangle(this->_img, x, y, w, h, MaixColor(color[0], color[1], color[2]), thickness);
        LOG_INFO << "success";
    }
    return *this;
}

_maix_image &_maix_image::_draw_circle(int x, int y, int r, std::vector<int> color, int thickness)
{
    if (this->_img)
    {
        libmaix_cv_image_draw_circle(this->_img, x, y, r, MaixColor(color[0], color[1], color[2]), thickness);
    }
    return *this;
}
_maix_image &_maix_image::_draw_ellipse(int cx, int cy, int rx, int ry, double angle, double startAngle, double endAngle, std::vector<int> color, int thickness)
{
    if (this->_img)
    {
        libmaix_cv_image_draw_ellipse(this->_img, cx, cy, rx, ry, angle, startAngle, endAngle, MaixColor(color[0], color[1], color[2]), thickness);
    }
    return *this;
}

_maix_image &_maix_image::_draw_string(int x, int y, std::string str, std::vector<int> color, double scale, int thickness)
{
    if (this->_img)
    {
        libmaix_cv_image_draw_string(this->_img, x, y, str.c_str(), MaixColor(color[0], color[1], color[2]), scale, thickness);
    }
    return *this;
}

_maix_image &_maix_image::_rotate(double rotate)
{
    if (this->_img)
    {
        libmaix_image_t *tmp = libmaix_image_create(this->_img->width, this->_img->height, this->_img->mode, LIBMAIX_IMAGE_LAYOUT_HWC, NULL, true);
        if (tmp)
        {
            libmaix_cv_image_rotate(this->_img, int(rotate), &tmp);
            libmaix_image_destroy(&this->_img), this->_img = tmp;
        }
        else
        {
            libmaix_image_destroy(&tmp);
        }
    }
    return *this;
}

_maix_image &_maix_image::_convert(std::string mode)
{
    if (this->_img)
    {
        libmaix_image_t *tmp = libmaix_image_create(this->_img->width, this->_img->height, any_cast<libmaix_image_mode_t>(py_to_pram[this->get_to(mode)][0]), LIBMAIX_IMAGE_LAYOUT_HWC, NULL, true);
        if (tmp)
        {
            if(libmaix_cv_image_convert(this->_img, any_cast<libmaix_image_mode_t>(py_to_pram[this->get_to(mode)][0]), &tmp) == 0)
            {
                libmaix_image_destroy(&this->_img), this->_img = tmp;
                this->_maix_image_type = mode;
                this->_maix_image_size = this->_img->width * this->_img->height * any_cast<libmaix_image_mode_t>(py_to_pram[this->get_to(mode)][1]);
            }
            else
            {
                libmaix_image_destroy(&tmp);
            }
            
        }
        else
        {
            libmaix_image_destroy(&tmp);
        }
    }
    else
    {
        std::cout << "please load image!" << std::endl;
    }
    return *this;
}

_maix_image &_maix_image::_draw_crop(int x, int y, int w, int h)
{
    if (this->_img)
    {
        libmaix_image_t *tmp = libmaix_image_create(w, h, this->_img->mode, LIBMAIX_IMAGE_LAYOUT_HWC, NULL, true);
        if (tmp)
        {
            libmaix_cv_image_crop(this->_img, x, y, w, h, &tmp);
            libmaix_image_destroy(&this->_img), this->_img = tmp;
            this->_maix_image_width = this->_img->width;
            this->_maix_image_height = this->_img->height;
        }
        else
        {
            libmaix_image_destroy(&tmp);
        }
    }
    return *this;
}
std::vector<int> _maix_image::_get_pixe(int x, int y)
{
    std::vector<int> color_val;
    if (this->_img)
    {
        libmaix_image_color_t colot_tmp = libmaix_cv_image_get_pixe(this->_img, x, y);
        color_val.push_back(colot_tmp.rgb888.r);
        color_val.push_back(colot_tmp.rgb888.g);
        color_val.push_back(colot_tmp.rgb888.b);
        color_val.push_back(colot_tmp.rgb888.a);
        return color_val;
    }
    return color_val;
}

py::object img_open(std::string path, std::string format)
{
    auto _maix_Image_ = py::module::import("_maix_Image");
    auto py_Image = _maix_Image_.attr("Image");
    auto py_img_obj = py_Image();
    auto py_fun_open = py_img_obj.attr("open_file");
    py_fun_open(path);
    return py_img_obj;
}

py::object img_new(std::vector<int> size, std::vector<int> color, std::string mode)
{
    LOG_INFO << "new img";
    //pybind11 python端的实现方法
    auto _maix_Image_ = py::module::import("_maix_Image");
    auto py_Image = _maix_Image_.attr("Image");
    auto py_img_obj = py_Image();
    auto py_fun_new = py_img_obj.attr("new");
    py_fun_new(size, color, mode);
    return py_img_obj;
    //pybind11 C++端的实现方法
    // _maix_image *img_val = NULL;
    // img_val = new _maix_image();
    // img_nu.push_back(img_val);
    // img_val->_new(size, color, mode);
    // return img_val;
    
}

py::object img_load(py::bytes data, std::vector<int> size, std::string mode)
{
    auto _maix_Image_ = py::module::import("_maix_Image");
    auto py_Image = _maix_Image_.attr("Image");
    auto py_img_obj = py_Image();
    auto py_fun_load = py_img_obj.attr("load");
    py_fun_load(data, size, mode);
    return py_img_obj;
}
void s_log(int nu)
{
    switch (nu)
    {
    case 1:
        nanolog::set_log_level(nanolog::LogLevel::INFO);
        break;
    case 2:
        nanolog::set_log_level(nanolog::LogLevel::WARN);
        break;
    case 3:
        nanolog::set_log_level(nanolog::LogLevel::CRIT);
        break;
    default:
        break;
    }
}

void s_info(std::string s)
{
    LOG_INFO << s;
}

void mode_init()
{
    nanolog::initialize(nanolog::GuaranteedLogger(), "/var/maixpy3/", "maix_Image", 1);
    nanolog::set_log_level(nanolog::LogLevel::INFO);
}

PYBIND11_MODULE(_maix_Image, mo)
{
    mode_init();
    mo.def("open_file", &img_open, py::arg("path"), py::arg("format") = "RGB");
    mo.def("new", &img_new, py::arg("size") = std::vector<int>{240, 240}, py::arg("color") = std::vector<int>{0, 0, 0}, py::arg("mode") = "RGB");
    mo.def("load", &img_load, py::arg("data"), py::arg("size") = std::vector<int>{240, 240}, py::arg("mode") = "RGB");
    mo.def("set_log", &s_log, py::arg("level") = 3);
    mo.def("s_info", &s_info);

    pybind11::class_<_maix_image>(mo, "Image")
        .def(pybind11::init<>())
        //Image 的一些属性
        .def_readonly("width", &_maix_image::_maix_image_width)
        .def_readonly("height", &_maix_image::_maix_image_height)
        .def_readonly("mode", &_maix_image::_maix_image_type)
        .def_readonly("size", &_maix_image::_maix_image_size)
        //Image重载自己python类的方法
        .def("__str__", &_maix_image::str__)
        .def("__repr__", &_maix_image::str__)
        .def("__len__", &_maix_image::len__)

        //Image自己的方法
        .def("new", &_maix_image::_new, py::arg("size") = std::vector<int>{240, 240}, py::arg("color") = std::vector<int>{0, 0, 0}, py::arg("mode") = "RGB")
        .def("load", &_maix_image::_load, py::arg("data"), py::arg("size") = std::vector<int>{240, 240}, py::arg("mode") = "RGB")
        .def("open_file",&_maix_image::_open_file)
        .def("get_to",&_maix_image::_get_to,py::arg("img") = "maix_Image")
        .def("clear",&_maix_image::_clear)
        .def("save", &_maix_image::_save, py::arg("path"), py::arg("format") = "jpeg")
        .def("tobytes", &_maix_image::_tobytes)
        .def("resize", &_maix_image::_resize, py::arg("w"), py::arg("h"))
        .def("draw_line", &_maix_image::_draw_line, py::arg("x1"), py::arg("y1"), py::arg("x2"), py::arg("y2"), py::arg("color") = std::vector<int>{127, 127, 127}, py::arg("thickness") = 1)
        .def("draw_rectangle", &_maix_image::_draw_rectangle, py::arg("x"), py::arg("y"), py::arg("w"), py::arg("h"), py::arg("color") = std::vector<int>{127, 127, 127}, py::arg("thickness") = 1)
        .def("draw_circle", &_maix_image::_draw_circle, py::arg("x"), py::arg("y"), py::arg("radius"), py::arg("color") = std::vector<int>{127, 127, 127}, py::arg("thickness") = 1)
        .def("draw_ellipse", &_maix_image::_draw_ellipse, py::arg("cx"), py::arg("xy"), py::arg("rx"), py::arg("ry"), py::arg("angle"), py::arg("startAngle"), py::arg("endAngle"), py::arg("color") = std::vector<int>{127, 127, 127}, py::arg("thickness") = 1)
        .def("draw_string", &_maix_image::_draw_string, py::arg("x"), py::arg("y"), py::arg("str"), py::arg("color") = std::vector<int>{127, 127, 127}, py::arg("scale") = 1.0, py::arg("thickness") = 1)
        .def("rotate", &_maix_image::_rotate, py::arg("rotate"))
        .def("convert", &_maix_image::_convert, py::arg("mode") = "RGB")
        .def("crop", &_maix_image::_draw_crop, py::arg("x"), py::arg("y"), py::arg("w"), py::arg("h"))
        .def("show", &_maix_image::_show)
        .def("get_pixe", &_maix_image::_get_pixe, py::arg("x"), py::arg("y"))
        // .def("load_freetype", &_maix_image::test)

        //Image继承 version 的方法
        // .def("test", &_maix_image::version_test)
        .def("cv_Canny", &_maix_image::_maix_vision_Canny,py::arg("thr_h"),py::arg("thr_l"))

        //Image继承 image_dsp 的方法

        //Image继承 nn的一些方法(待定)
        ;
}
