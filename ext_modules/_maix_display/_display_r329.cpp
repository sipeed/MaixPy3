#include "_maix_display.h"
#ifdef R329Display
using namespace std;
namespace py = pybind11;

static void conversion_rgb888_to_rgb565(r329_Display *tp, void *tmp)
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
void v_init(r329_Display *tp)
{
    tp->disp = libmaix_disp_create(0);
    if (tp->disp == NULL)
    {
        PyErr_SetString(PyExc_RuntimeError, "libmaix_disp_create err!");
        throw py::error_already_set();
    }
    tp->fb_img.w = tp->disp->width;
    tp->fb_img.h = tp->disp->height;
    tp->fb_img.bpp = LIBMAIX_IMAGE_MODE_RGB565;
    tp->fb_img.data = (uint8_t *)malloc(tp->fb_img.w * tp->fb_img.h * 2);
    if (tp->fb_img.data == NULL)
    {
        PyErr_SetString(PyExc_RuntimeError, "display fb_img create err,please cheack memory\n");
        throw py::error_already_set();
    }
}
void v_close(r329_Display *tp)
{
    if (NULL != tp->disp)
        libmaix_disp_destroy(&tp->disp);
    if (NULL != tp->fb_img.data)
        free(tp->fb_img.data);
}

r329_Display::r329_Display(int w, int h)
{
    v_init(this);
    this->width = w;
    this->height = h;
}

r329_Display::~r329_Display()
{
    v_close(this);
    return;
}

pybind11::object r329_Display::draw(pybind11::object py_img, int img_w, int img_h)
{
    uint8_t *in_img = NULL;
    int img_width = img_w;
    int img_height = img_h;
    if (py::isinstance<py::bytes>(py_img))
    {
        string tmp = py_img.cast<string>();
        in_img = (uint8_t *)tmp.c_str();
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
            in_img = (uint8_t *)tmp.c_str();
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
}

void r329_Display::close()
{
    v_close(this);
}

void r329_Display::__enter__()
{
    //do nothing...
}

void r329_Display::__exit__()
{
    v_close(this);
}

#endif
