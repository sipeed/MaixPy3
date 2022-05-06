#include <math.h>
#include <string.h>

static void maix_nn_functional_softmax_float32(float *data, int n, float *out, int stride)
{
    int i;
    float sum = 0;
    float largest_i = data[0];
    if(!out)
    {
        out = data;
    }

    for (i = 0; i < n; ++i)
    {
        if (data[i * stride] > largest_i)
            largest_i = data[i * stride];
    }
    for (i = 0; i < n; ++i)
    {
        float value = expf(data[i * stride] - largest_i);
        sum += value;
        out[i * stride] = value;
    }
    for (i = 0; i < n; ++i)
	{
        out[i * stride] /= sum;
	}
}

static void maix_nn_functional_softmax_double(double *data, int n, double *out, int stride)
{
    int i;
    double sum = 0;
    double largest_i = data[0];
    if(!out)
    {
        out = data;
    }

    for (i = 0; i < n; ++i)
    {
        if (data[i * stride] > largest_i)
            largest_i = data[i * stride];
    }
    for (i = 0; i < n; ++i)
    {
        double value = expf(data[i * stride] - largest_i);
        sum += value;
        out[i * stride] = value;
    }
    for (i = 0; i < n; ++i)
	{
        out[i * stride] /= sum;
	}
}