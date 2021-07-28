
#include <pybind11/pybind11.h>

#include "opencv2/core.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/videoio.hpp"
#include "opencv2/imgcodecs.hpp"

using namespace cv;
using namespace std;

namespace py = pybind11;

class _v83x_opencv
{

private:
  /* data */
public:
  void set_ui(py::bytes &argb)
  {
  }

  py::bytes get_vi()
  {
    return py::bytes();
  }

  py::bytes get_ai()
  {
  }

  py::bytes test(py::bytes &rgb)
  {
    std::string tmp = static_cast<std::string>(rgb);
    cv::Mat input(240, 240, CV_8UC3,const_cast<char*>(tmp.c_str()));

    cv::Mat output = Mat::zeros(240, 240, CV_8UC4);
    
    Mat gray, edges;
    // Mat standard_hough, probabilistic_hough;
    int min_threshold = 50;
    int max_trackbar = 150;
    int s_trackbar = max_trackbar;
    int p_trackbar = max_trackbar;

    cvtColor(input, gray, COLOR_RGB2GRAY);

    Canny(gray, edges, 50, 200, 3);

    cvtColor(edges, output, COLOR_GRAY2BGRA);

    vector<Vec2f> s_lines;
    // cvtColor(edges, standard_hough, COLOR_GRAY2BGR);
    
    /// 1. Use Standard Hough Transform
    HoughLines(edges, s_lines, 1, CV_PI / 180, min_threshold + s_trackbar, 0, 0);

    /// Show the result
    for (size_t i = 0; i < s_lines.size(); i++)
    {
      float r = s_lines[i][0], t = s_lines[i][1];
      double cos_t = cos(t), sin_t = sin(t);
      double x0 = r * cos_t, y0 = r * sin_t;
      double alpha = 1000;

      Point pt1(cvRound(x0 + alpha * (-sin_t)), cvRound(y0 + alpha * cos_t));
      Point pt2(cvRound(x0 - alpha * (-sin_t)), cvRound(y0 - alpha * cos_t));
      line(output, pt1, pt2, Scalar(255, 0, 0, 200), 3, LINE_AA);
    }

    // vector<Vec4i> p_lines;
    // cvtColor(edges, probabilistic_hough, COLOR_GRAY2BGR);

    // /// 2. Use Probabilistic Hough Transform
    // HoughLinesP(edges, p_lines, 1, CV_PI / 180, min_threshold + p_trackbar, 30, 10);

    // /// Show the result
    // for (size_t i = 0; i < p_lines.size(); i++)
    // {
    //   Vec4i l = p_lines[i];
    //   line(output, Point(l[0], l[1]), Point(l[2], l[3]), Scalar(255, 0, 0, 200), 3, LINE_AA);
    // }

    int size = output.total() * output.elemSize();
    return py::bytes((char *)output.data, size);
    // std::vector buff;
    // cv::imencode(".bmp", image, buff);
    // std::string image_string(reinterpret_cast<char*>(&buff[0]), buff.size());
    // return py::bytes();
  }

  _v83x_opencv(int vi_w, int vi_h, int ai_w, int ai_h, int vo_dir, int ai_dir)
  {
    init(vi_w, vi_h, ai_w, ai_h, vo_dir, ai_dir);
  }

  int init(int vi_w, int vi_h, int ai_w, int ai_h, int vo_dir, int ai_dir)
  {
  }

  int exit()
  {
  }

  ~_v83x_opencv()
  {
    exit();
  }
};

PYBIND11_MODULE(_maix_opencv, m)
{
  pybind11::class_<_v83x_opencv>(m, "_v83x_opencv")
      .def(pybind11::init<int, int, int, int, int, int>(),
           py::arg("vi_w") = 240, py::arg("vi_h") = 240, py::arg("ai_w") = 192, py::arg("ai_h") = 128, py::arg("vo_dir") = 0, py::arg("ai_dir") = 0)
      .def("test", &_v83x_opencv::test)
      .def("get_ai", &_v83x_opencv::get_ai)
      .def("get_vi", &_v83x_opencv::get_vi)
      .def("set_ui", &_v83x_opencv::set_ui);
}
