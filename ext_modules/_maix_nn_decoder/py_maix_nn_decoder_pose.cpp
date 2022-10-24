#include <iostream>
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
#include "libmaix_nn.h"
#include "libmaix_nn_decoder.h"
#include "libmaix_nn_decoder_pose.h"
namespace py = pybind11;

typedef struct
{
    libmaix_nn_decoder_pose_config_t config;
    libmaix_nn_decoder_t *decoder;
    libmaix_nn_layer_t feature_maps[4];
    libmaix_nn_decoder_pose_result_t pose_result;
    bool init = false;
} DecoderPoseObject;

class _pose_decoder
{
private:
public:
    DecoderPoseObject object;
    _pose_decoder(std::string path, float hm_th);
    ~_pose_decoder();
    std::string decoder_pose_str()
    {
        return std::string("pose decoder object");
    }
    py::object decoder_pose_run(py::list fmaps);
};

_pose_decoder ::_pose_decoder(std::string path, float hm_th)
{
    libmaix_err_t err = LIBMAIX_ERR_NONE;
    this->object.config.center_weight_path = path.c_str();
    this->object.config.hm_th = hm_th;
    this->object.config.image_size = 192;
    this->object.config.num_joints = 17;
    this->object.decoder = libmaix_nn_decoder_pose_create();
    if (!this->object.decoder)
    {
        // this->object.init = false;
        std ::cout << "pose decoder create faild" << std::endl;
    }
    err = this->object.decoder->init(this->object.decoder, &(this->object.config));
    if (err != LIBMAIX_ERR_NONE)
    {
        this->object.init = false;
        std ::cout << "py maix nn pose decdoer init faild" << std::endl;
    }
    else
    {
        this->object.init = true;
        std ::cout << "py maix nn pose decdoer init success" << std::endl;
    }

    this->object.feature_maps[0].c = 1;
    this->object.feature_maps[0].h = 48;
    this->object.feature_maps[0].w = 48;

    this->object.feature_maps[1].c = 17;
    this->object.feature_maps[1].h = 48;
    this->object.feature_maps[1].w = 48;

    this->object.feature_maps[2].c = 32;
    this->object.feature_maps[2].h = 48;
    this->object.feature_maps[2].w = 48;

    this->object.feature_maps[3].c = 32;
    this->object.feature_maps[3].h = 48;
    this->object.feature_maps[3].w = 48;
}

_pose_decoder ::~_pose_decoder()
{
    if (this->object.decoder)
    {
        libmaix_nn_decoder_pose_deinit(this->object.decoder);
        libmaix_nn_decoder_pose_destroy(&(this->object.decoder));
    }
}

py::object _pose_decoder::decoder_pose_run(py::list fmaps)
{
    libmaix_err_t err = LIBMAIX_ERR_NONE;
    // py ::object res;
    // return res;
    auto numpy_object_type = py::module::import("numpy").attr("ndarray");
    int output_num = py::len(fmaps);

    // output_list
    std::vector<std::string> bytes_output_buffer;

    for (int id = 0; id != output_num; id++)
    {
        this->object.feature_maps[id].dtype = LIBMAIX_NN_DTYPE_FLOAT;
        this->object.feature_maps[id].data = NULL;
        this->object.feature_maps[id].layout = LIBMAIX_NN_LAYOUT_CHW;
        if (py::isinstance(fmaps[id], numpy_object_type))
        {
            auto tobytes = fmaps[id].attr("tobytes");
            bytes_output_buffer.push_back(tobytes().cast<std::string>());
        }
        else if (py::isinstance<py::bytes>(fmaps[id]))
        {
            bytes_output_buffer.push_back(fmaps[id].cast<std::string>());
        }
        this->object.feature_maps[id].data = (void *)bytes_output_buffer[id].c_str();
    }
    libmaix_nn_decoder_pose_result_t outer_pose_result;
    this->object.decoder->decode(this->object.decoder, this->object.feature_maps, &outer_pose_result);
    py::list key_points;
    int joint_num = this->object.config.num_joints;
    for (int joint = 0; joint != joint_num; joint)
    {
        py::list point;
        int x = (int)outer_pose_result.keypoints[joint * 2];
        int y = (int)outer_pose_result.keypoints[joint * 2 + 1];
        point.append(x);
        point.append(y);
        key_points.append(point);
    }
    return key_points;
}

PYBIND11_MODULE(_maix_nn_decoder_pose, m)
{
    pybind11::class_<_pose_decoder>(m, "pose")
        .def(pybind11::init<std::string , float >(), py::arg("center_weight_path"), py::arg("hm_th"))
        .def("__str__", &_pose_decoder::decoder_pose_str)
        .def("run", &_pose_decoder::decoder_pose_run, py::arg("fmaps"));
}
