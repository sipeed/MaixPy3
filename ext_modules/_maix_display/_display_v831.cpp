#include "_maix_display.h"
#ifdef V831Display

using namespace std;
namespace py = pybind11;


void v_init(v831_Display *tp)
{
    tp->disp = libmaix_disp_creat();
    if(tp->disp == NULL) {
        // tp->disp->swap_rb = 1;
        PyErr_SetString(PyExc_RuntimeError, "libmaix_disp_create err!"); 
        throw py::error_already_set();

    }
}
void v_close(v831_Display *tp)
{
    if (NULL != tp->disp)
        libmaix_disp_destroy(&tp->disp);
}


v831_Display::v831_Display(int w,int h)
{
    v_init(this);
    this->width = w ;
    this->height = h;
}

v831_Display::~v831_Display()
{
    v_close(this);
    return ;
}

pybind11::object v831_Display::draw(pybind11::object py_img,int img_w,int img_h)
{
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
            this->disp->draw(this->disp, in_img, (this->disp->width - img_width) / 2,(this->disp->height - img_height) / 2, img_width, img_height, 1);
          }
      }
    }
    return py::none();
}


void v831_Display::close()
{
    v_close(this);
}


void v831_Display::__enter__()
{
    //do nothing...
}


void v831_Display::__exit__()
{
    v_close(this);
}









#endif