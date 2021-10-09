
#include "_maix_display.h"

using namespace std;
namespace py = pybind11;


class _dispay_pybind11
{
private:
    
public:
    string version = "0.2";
    string name = "_maix_display";
    _dispay_pybind11()
    {
    }
    ~_dispay_pybind11()
    {
    }
    string _maix_display_help()
    {
        return "MaixPy3 Display\n";
    }
};




PYBIND11_MODULE(_maix_display, m)
{
    m.doc() = "This is MaixPy3 Display\n";
    pybind11::class_<_dispay_pybind11>(m, "_maix_display")
        .def(pybind11::init())
        .def_readonly("_VERSION_", &_dispay_pybind11::version)  
        .def_readonly("_NAME_", &_dispay_pybind11::name)  
        .def("help", &_dispay_pybind11::_maix_display_help);

    pybind11::class_<_Display>(m, "Display")
        .def(py::init<int,int>(),py::arg("w") = 240, py::arg("h") = 240)
        .def_readwrite("width",&_Display::width)
        .def_readwrite("height",&_Display::height)
        .def("draw", &_Display::draw,py::arg("py_img"),py::arg("img_w") = 240,py::arg("img_w") = 240)
        .def("__enter__", &_Display::__enter__)
        .def("__exit__", &_Display::__exit__);
}





#ifdef R329Display
static void conversion_rgb888_to_rgb565(_Display *tp, void *tmp)
{
    uint8_t *src_pix = (uint8_t *)tmp;
    uint16_t *fb_pix = (uint16_t *)tp->fb_img.data;
    for (int i = 0; i < tp->fb_img.w * tp->fb_img.h; i++)
    {
        *fb_pix = ((((src_pix[0] >> 3) & 31) << 11) | (((src_pix[1] >> 2) & 63) << 5) | ((src_pix[2] >> 3) & 31));
        fb_pix++;
        src_pix += 3;
    }
}
#endif





void v_close(_Display *tp)
{

#ifdef R329Display
    if (NULL != tp->disp)
        libmaix_disp_destroy(&tp->disp);
    if (NULL != tp->fb_img.data)
        free(tp->fb_img.data);
#endif 

#ifdef V83xDisplay
    if (NULL != tp->disp)
        libmaix_disp_destroy(&tp->disp);
#endif // V83xDisplay

}


void v_init(_Display *tp)
{
#ifdef V83xDisplay
    tp->disp = libmaix_disp_create(0);
    if(tp->disp == NULL) {
        v_close(tp);
        PyErr_SetString(PyExc_RuntimeError, "libmaix_disp_create err!"); 
        throw py::error_already_set();
    }
#endif // V83xDisplay

#ifdef R329Display
    tp->disp = libmaix_disp_create(0);
    if(tp->disp == NULL) {
        v_close(tp);
        PyErr_SetString(PyExc_RuntimeError, "libmaix_disp_create err!"); 
        throw py::error_already_set();
    }
    tp->fb_img.w = tp->disp->width;
    tp->fb_img.h = tp->disp->height;
    tp->fb_img.bpp = LIBMAIX_IMAGE_MODE_RGB565;
    tp->fb_img.data = (uint8_t *)malloc(tp->fb_img.w * tp->fb_img.h * 2);
    if (tp->fb_img.data == NULL)
    {
        v_close(tp);
        PyErr_SetString(PyExc_RuntimeError, "display fb_img create err,please cheack memory\n");
        throw py::error_already_set();
    }
#endif // R329Display

}



_Display::_Display(int w,int h)
{
#ifdef R329Display
    this->fb_img.data = NULL;
#endif // R329Display
    this->width = w ;
    this->height = h;
    v_init(this);
}

_Display::~_Display()
{
    v_close(this);
}

pybind11::object _Display::draw(pybind11::object py_img,int img_w,int img_h)
{

#ifdef R329Display
    uint8_t *in_img = NULL;
    int img_width = img_w;
    int img_height = img_h;
    if (py::isinstance<py::bytes>(py_img))
    {
        string tmp = py_img.cast<string>();
        in_img = (uint8_t*)tmp.c_str();
        conversion_rgb888_to_rgb565(this, in_img);
    }
    else
    {
      auto PIL_ = py::module::import("PIL.Image").attr("Image");
      if (py::isinstance(py_img, PIL_))
      {
        auto tobytes = PIL_.attr("tobytes");
        auto img_bytes = tobytes(py_img);
        string tmp = img_bytes.cast<string>();
        in_img = (uint8_t*)tmp.c_str();
        conversion_rgb888_to_rgb565(this, in_img);
        auto img_size = py_img.attr("size").cast<vector<int>>();
        img_width = img_size[0];
        img_height = img_size[1];
      }
    }

    if (NULL != this->disp)
    {
        if (this->disp->width >= img_width && this->disp->height >= img_height)
        {
            if (in_img != NULL)
            {
                this->disp->draw(this->disp, this->fb_img.data);
            }
        }
    }
    return py::none();

#endif // R329Display

#ifdef V83xDisplay
    uint8_t *in_img = NULL;
    int img_width = img_w;
    int img_height = img_h;
    if (py::isinstance<py::bytes>(py_img))
    {
        string tmp = py_img.cast<string>();
        in_img = (uint8_t*)tmp.c_str();
    }
    else
    {
      auto PIL_ = py::module::import("PIL.Image").attr("Image");
      if (py::isinstance(py_img, PIL_))
      {
        auto tobytes = PIL_.attr("tobytes");
        auto img_bytes = tobytes(py_img);
        string tmp = img_bytes.cast<string>();
        in_img = (uint8_t*)tmp.c_str();
        auto img_size = py_img.attr("size").cast<vector<int>>();
        img_width = img_size[0];
        img_height = img_size[1];
      }
    }

    if (NULL != this->disp) {
      if (this->disp->width >= img_width && this->disp->height >= img_height) {
          if (in_img != NULL) {
            this->disp->draw(this->disp, in_img);
          }
      }
    }
    return py::none();



#endif // V83xDisplay
}


void _Display::close()
{
    v_close(this);
}


void _Display::__enter__()
{
    //do nothing...
}


void _Display::__exit__()
{
    v_close(this);
}





















