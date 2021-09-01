#include <pybind11/pybind11.h>

namespace py = pybind11;

extern "C"
{

#include "libmaix_cam.h"
#include "libmaix_image.h"

    unsigned char *g2d_allocMem(unsigned int size);
    int g2d_freeMem(void *vir_ptr);
    unsigned int g2d_getPhyAddrByVirAddr(void *vir_ptr);
}

// #define CALC_FPS(tips)                                                                                     \
//   {                                                                                                        \
//     static int fcnt = 0;                                                                                   \
//     fcnt++;                                                                                                \
//     static struct timespec ts1, ts2;                                                                       \
//     clock_gettime(CLOCK_MONOTONIC, &ts2);                                                                  \
//     if ((ts2.tv_sec * 1000 + ts2.tv_nsec / 1000000) - (ts1.tv_sec * 1000 + ts1.tv_nsec / 1000000) >= 1000) \
//     {                                                                                                      \
//       printf("%s => H26X FPS:%d\n", tips, fcnt);                                                  \
//       ts1 = ts2;                                                                                           \
//       fcnt = 0;                                                                                            \
//     }                                                                                                      \
//   }

class _v83x_vivo
{
    typedef enum
    {
        VO_VI = 0,
        VO_UI = 9,
        VO_INVALID,
    };

    typedef enum
    {
        VI_NONE = 0,
        VI_RGB888,
        VI_ARGB8888,
        VI_YUV420,
        VI_YUV422,
        VI_Y,
        VI_YYY,
    } fmt_t;

    void YtoRGB(unsigned char *YUV422P, unsigned char *Gray24, int width, int height)
    {
        for (int i = 0, sum = width * height; i != sum; i += 1, Gray24 += 3)
        {
            *(Gray24) = *(Gray24 + 1) = *(Gray24 + 2) = YUV422P[i];
        }
    }

    typedef struct
    {
        uint16_t x;
        uint16_t y;
        uint16_t w;
        uint16_t h;
    } rect_size_t;

    typedef struct
    {
        void *buf;
        rect_size_t size;
        fmt_t fmt;
        int ion;
    } frame_t;

    frame_t *frame_new(fmt_t fmt, rect_size_t size, int ion)
    {
        size_t s = 0;
        frame_t *tmp = (frame_t *)malloc(sizeof(frame_t));
        if (tmp != NULL)
        {
            tmp->fmt = fmt;
            tmp->size = size;
            tmp->ion = ion;
            switch (tmp->fmt)
            {
            case VI_YUV420:
                s = (tmp->size.w * tmp->size.h * 3 / 2);
                break;
            case VI_YUV422:
                s = (tmp->size.w * tmp->size.h * 2);
                break;
            case VI_RGB888:
            case VI_YYY:
                s = (tmp->size.w * tmp->size.h * 3);
                break;
            case VI_Y:
                s = (tmp->size.w * tmp->size.h);
                break;
            case VI_ARGB8888:
                s = (tmp->size.w * tmp->size.h * 4);
                break;
            default:
                break;
            }
            if (s)
            {
                if (tmp->ion)
                {
                    tmp->buf = (void *)g2d_allocMem(s);
                }
                else
                {
                    tmp->buf = malloc(s);
                }
                if (tmp->buf != NULL)
                {
                    return tmp;
                }
            }
            free(tmp);
        }
        return NULL;
    }

    unsigned int frame_phy(void *vir_ptr)
    {
        return g2d_getPhyAddrByVirAddr(vir_ptr);
    }

    void frame_del(frame_t **frame)
    {
        if (frame && *frame)
        {
            if ((*frame)->buf)
            {
                if ((*frame)->ion)
                {
                    g2d_freeMem((*frame)->buf), (*frame)->buf = NULL;
                }
                else
                {
                    free((*frame)->buf), (*frame)->buf = NULL;
                }
            }
            free(*frame), *frame = NULL;
        }
    }

    frame_t *fm[2];
    frame_t *ui;
    struct libmaix_cam *vi[2];
    struct libmaix_vo *vo;

    libmaix_image_t *yuv2rgb;
    libmaix_image_t *y2r[2];

    int vo_dir, ai_dir, inited;

private:
    /* data */
public:
    void set(py::bytes &bytes)
    {
        frame_t *frame = this->ui;
        std::string value = static_cast<std::string>(bytes);
        void *tmp = this->vo->get_frame(this->vo, VO_UI);
        if (tmp != NULL)
        {
            const int frame_size = frame->size.w * frame->size.h;
            if (frame_size * 4 == value.length())
            {
                // puts("rgba");
                // rgba > abgr > argb
                uint32_t *argb = (uint32_t *)frame->buf, *abgr = (uint32_t *)value.c_str();
                for (int i = 0, sum = frame_size; i != sum; i++)
                {
                    argb[i] =   (abgr[i] & 0xFF000000) |         // ______AA
                                ((abgr[i] & 0x00FF0000) >> 16) | // BB______
                                (abgr[i] & 0x0000FF00) |         // __GG____
                                ((abgr[i] & 0x000000FF) << 16);  // ____RR__
                }
            }
            else if (frame_size * 3 == value.length())
            {
                // puts("rgb");
                // rgb > argb
                uint32_t *argb = (uint32_t *)frame->buf;
                uint8_t *rgb = (uint8_t *)value.c_str();
                for (int i = 0, sum = frame_size; i != sum; i++)
                {
                    uint32_t tmp = *rgb << 16;  // ____BB
                    rgb++, tmp |= *rgb << 8;    // __GG__
                    rgb++, tmp |= *rgb;         // RR____
                    rgb++, argb[i] = 0xFF000000 | tmp; // ARGB
                }
            }
            else
            {
                return;
            }
            // memcpy(frame->buf, value.c_str(), frame_size); // bgra
            uint32_t *phy = NULL, *vir = NULL;
            this->vo->frame_addr(this->vo, tmp, &vir, &phy);
            if (this->vo_dir)
            {
                if (frame->ion)
                {
                    _g2d_argb_rotate((void *)frame_phy(frame->buf), (void *)phy[0], frame->size.w, frame->size.h, this->vo_dir);
                }
                else
                {
                    g2d_argb_rotate((uint32_t *)frame->buf, (void *)phy[0], frame->size.w, frame->size.h, this->vo_dir);
                }
            }
            else
            {
                memcpy((uint8_t *)vir[0], frame->buf, frame->size.w * frame->size.h * 4);
            }
            this->vo->set_frame(this->vo, tmp, VO_UI);

        }
    }

    py::list get(bool show=true)
    {
        py::list result; // l.attr("pop")();
        if (LIBMAIX_ERR_NONE == this->vi[0]->capture(this->vi[0], (uint8_t *)this->fm[0]->buf))
        {
            void *tmp = this->vo->get_frame(this->vo, VO_VI);
            if (tmp != NULL)
            {
                frame_t *frame = this->fm[0];

                if (show) {
                    uint32_t *phy = NULL, *vir = NULL;
                    this->vo->frame_addr(this->vo, tmp, &vir, &phy);
                    if (this->vo_dir)
                    {
                        if (frame->ion)
                        {
                            const int _w_h_ = frame->size.w * frame->size.h; //ALIGN(w, 16) * ALIGN(h, 16);
                            uint32_t *tmp_phy = (uint32_t *)frame_phy(frame->buf);
                            _g2d_nv21_rotate(
                                (void *)tmp_phy, (void *)tmp_phy + _w_h_,
                                (void *)phy[0], (void *)phy[0] + _w_h_,
                                frame->size.w, frame->size.h, this->vo_dir);
                        }
                        else
                        {
                            g2d_nv21_rotate((uint8_t *)frame->buf, frame->size.w, frame->size.h, this->vo_dir);
                            memcpy((uint8_t *)vir[0], frame->buf, frame->size.w * frame->size.h * 3 / 2);
                        }
                    }
                    else
                    {
                        memcpy((uint8_t *)vir[0], frame->buf, frame->size.w * frame->size.h * 3 / 2);
                    }
                    this->vo->set_frame(this->vo, tmp, VO_VI);
                }

                this->yuv2rgb->width = this->fm[0]->size.w;
                this->yuv2rgb->height = this->fm[0]->size.h;
                this->yuv2rgb->data = this->fm[0]->buf;
                libmaix_err_t err = this->yuv2rgb->convert(this->yuv2rgb, LIBMAIX_IMAGE_MODE_RGB888, &this->y2r[0]);
                if (err == LIBMAIX_ERR_NONE)
                {
                    result.append(py::bytes((char *)this->y2r[0]->data, this->yuv2rgb->height * this->yuv2rgb->width * 3));
                    if (LIBMAIX_ERR_NONE == this->vi[1]->capture(this->vi[1], (uint8_t *)this->fm[1]->buf))
                    {
                        frame_t *frame = this->fm[1];
                        if (this->ai_dir)
                        {
                            g2d_nv21_rotate((uint8_t *)frame->buf, frame->size.w, frame->size.h, this->ai_dir);
                            this->yuv2rgb->width = this->fm[1]->size.h;
                            this->yuv2rgb->height = this->fm[1]->size.w;
                        }
                        else
                        {
                            this->yuv2rgb->width = this->fm[1]->size.w;
                            this->yuv2rgb->height = this->fm[1]->size.h;
                        }
                        this->yuv2rgb->data = this->fm[1]->buf;
                        libmaix_err_t err = this->yuv2rgb->convert(this->yuv2rgb, LIBMAIX_IMAGE_MODE_RGB888, &this->y2r[1]);
                        if (err == LIBMAIX_ERR_NONE)
                        {
                            result.append(py::bytes((char *)this->y2r[1]->data, this->yuv2rgb->height * this->yuv2rgb->width * 3));
                            // CALC_FPS("get");
                        }
                    }
                }
            }
        }
        return std::move(result);
    }

    _v83x_vivo(int vi_w, int vi_h, int ai_w, int ai_h, int vo_dir, int ai_dir) : inited(false), ui(NULL), vi({NULL}), vo(NULL), y2r({NULL}), yuv2rgb(NULL), fm({NULL})
    {
        init(vi_w, vi_h, ai_w, ai_h, vo_dir, ai_dir);
    }

    int resize(int w, int h, int i)
    {
        if (this->inited && this->vi[i]->width != w && this->vi[i]->height != h)
        {
            if (NULL != this->vi[i])
                libmaix_cam_destroy(&this->vi[i]);
            if (NULL != this->fm[i])
                frame_del(&this->fm[i]);
            if (NULL != this->y2r[i])
                libmaix_image_destroy(&this->y2r[i]);
            this->vi[i] = libmaix_cam_create(i, w, h, 1, 0);
            if (NULL == this->vi[i])
                return exit();
            this->fm[i] = frame_new(VI_YUV420, {0, 0, w, h}, i == 0 ? this->vo_dir : 0);
            if (NULL == this->fm[i])
                return exit();
            this->y2r[i] = libmaix_image_create(w, h, LIBMAIX_IMAGE_MODE_RGB888, LIBMAIX_IMAGE_LAYOUT_HWC, NULL, true);
            if (NULL == this->y2r[i])
                return exit();
            this->vi[i]->start_capture(this->vi[i]);
            return 1;
        }
        return 0;
    }

    int init(int vi_w, int vi_h, int ai_w, int ai_h, int vo_dir, int ai_dir)
    {
        if (!this->inited)
        {
            libmaix_camera_module_init();
            libmaix_image_module_init();

            this->ai_dir = ai_dir;
            this->vo_dir = this->vo_dir = vo_dir;

            this->yuv2rgb = libmaix_image_create(0, 0, LIBMAIX_IMAGE_MODE_YUV420SP_NV21, LIBMAIX_IMAGE_LAYOUT_HWC, (void *)NULL, false);
            if (NULL == this->yuv2rgb)
                return exit();

            this->vi[0] = libmaix_cam_create(0, vi_w, vi_h, 1, 0);
            if (NULL == this->vi[0])
                return exit();
            this->fm[0] = frame_new(VI_YUV420, {0, 0, vi_w, vi_h}, this->vo_dir);
            if (NULL == this->fm[0])
                return exit();
            this->y2r[0] = libmaix_image_create(vi_w, vi_h, LIBMAIX_IMAGE_MODE_RGB888, LIBMAIX_IMAGE_LAYOUT_HWC, NULL, true);
            if (NULL == this->y2r[0])
                return exit();
            this->vi[0]->start_capture(this->vi[0]);

            this->vi[1] = libmaix_cam_create(1, ai_w, ai_h, 1, 0);
            if (NULL == this->vi[1])
                return exit();

            // if (this->ai_dir % 2 == 1)
            // {
            //     int tmp = ai_w;
            //     ai_w = ai_h, ai_h = tmp;
            // }

            this->fm[1] = frame_new(VI_YUV420, {0, 0, ai_w, ai_h}, 0);
            if (NULL == this->fm[1])
                return exit();
            this->y2r[1] = libmaix_image_create(ai_w, ai_h, LIBMAIX_IMAGE_MODE_RGB888, LIBMAIX_IMAGE_LAYOUT_HWC, NULL, true);
            if (NULL == this->y2r[1])
                return exit();
            this->vi[1]->start_capture(this->vi[1]);

            int _w = vi_w, _h = vi_h;

            if (this->vo_dir % 2 == 1)
            {
                int tmp = vi_w;
                vi_w = vi_h, vi_h = tmp;
            }

            // vi 320*240 > 90 > 240*320 => vo 240*320
            this->ui = frame_new(VI_ARGB8888, (rect_size_t){0, 0, vi_w, vi_h}, this->vo_dir);
            if (NULL == this->ui)
                return exit();

            this->vo = libmaix_vo_create(vi_w, vi_h, 0, 0, _w, _h);
            if (NULL == this->vo)
                return exit();

            this->inited = true;
            return this->inited;
        }
    }

    int exit()
    {
        if (this->inited)
        {
            for (int i = 0; i < 2; i++)
            {
                if (NULL != this->vi[i])
                    libmaix_cam_destroy(&this->vi[i]);
                if (NULL != this->fm[i])
                    frame_del(&this->fm[i]);
                if (NULL != this->y2r[i])
                    libmaix_image_destroy(&this->y2r[i]);
            }
            if (NULL != this->ui)
                frame_del(&this->ui);
            if (NULL != this->yuv2rgb)
                libmaix_image_destroy(&this->yuv2rgb);
            if (NULL != this->vo)
                libmaix_vo_destroy(&this->vo);
            libmaix_camera_module_deinit();
            usleep(200000); // wait 1s to deinit
            libmaix_image_module_deinit();
            libmaix_camera_module_deinit();
            this->inited = false;
        }
        return this->inited;
    }

    ~_v83x_vivo()
    {
        exit();
    }
};

PYBIND11_MODULE(_maix_vivo, m)
{
    pybind11::class_<_v83x_vivo>(m, "_v83x_vivo")
        .def(pybind11::init<int, int, int, int, int, int>(),
             py::arg("vi_w") = 240, py::arg("vi_h") = 240, py::arg("ai_w") = 192, py::arg("ai_h") = 128, py::arg("vo_dir") = 0, py::arg("ai_dir") = 0)
        .def("get", &_v83x_vivo::get, py::arg("show") = 1)
        .def("resize", &_v83x_vivo::resize, py::arg("w") = 240, py::arg("h") = 240, py::arg("i") = 1)
        .def("set", &_v83x_vivo::set);
}
