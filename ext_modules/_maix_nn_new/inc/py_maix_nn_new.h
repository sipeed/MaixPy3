#include "libmaix_nn.h"
#include "mud.h"
typedef struct
{
    libmaix_nn_model_path_t  model_path;
    libmaix_nn_opt_param_t opt_param;
    ini_info_t info;
    bool is_init;
    libmaix_nn_t * nn;
    int8_t *quantized_buffer;
    int8_t * int_output_buffer;
    float * float_output_buffer;
    libmaix_nn_layer_t * input_layer;
    libmaix_nn_layer_t * output_layer;
}NNObject;

