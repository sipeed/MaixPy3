
#include <iostream>
#include <pybind11/pybind11.h>
#include <pybind11/embed.h>
#include <pybind11/stl.h>
#include <vector>
#include <list>
#include <array>
#include <map>
#include <set>

#include "opencv2/core.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/videoio.hpp"
#include "opencv2/imgcodecs.hpp"

using namespace cv;
using namespace std;

namespace py = pybind11;
#define heigh_t 10
#define debug_line printf("%s:%d %s %s %s \r\n", __FILE__, __LINE__, __FUNCTION__, __DATE__, __TIME__)
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
    cv::Mat input(240, 240, CV_8UC3, const_cast<char *>(tmp.c_str()));

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

  py::bytes opencv_test(py::bytes &rgb)
  {
    std::string tmp = static_cast<std::string>(rgb);
    cv::Mat input(240, 240, CV_8UC3, const_cast<char *>(tmp.c_str()));
    int size = input.total() * input.elemSize();
    return py::bytes((char *)input.data, size);
  }

  py::list find_blob(py::bytes &rgb, vector<int> &hsv_da)
  {
    string tmp = static_cast<string>(rgb);
    cv::Mat input(240, 240, CV_8UC3, const_cast<char *>(tmp.c_str()));
    Mat hsv, mask;
    cvtColor(input, hsv, COLOR_RGB2HSV);
    inRange(hsv, Scalar(hsv_da[0], hsv_da[1], hsv_da[2]), Scalar(hsv_da[3], hsv_da[4], hsv_da[5]), mask);
    // cout << hsv_da <<endl;
    Mat se = getStructuringElement(MORPH_RECT, Size(5, 5), Point(-1, -1));
    morphologyEx(mask, mask, MORPH_OPEN, se);
    vector<vector<Point>> contours;
    vector<Vec4i> hiearchy;
    findContours(mask, contours, hiearchy, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);

    py::list out;

    for (int i = 0; i < contours.size(); i++)
    {
      py::list tmp;

      int x_max = 0, x_min = 1000, y_max = 0, y_min = 1000;

      for (int j = 0; j < contours[i].size(); j++)
      {
        if (x_max < contours[i][j].x)
          x_max = contours[i][j].x;
        if (x_min > contours[i][j].x)
          x_min = contours[i][j].x;
        if (y_max < contours[i][j].y)
          y_max = contours[i][j].y;
        if (y_min > contours[i][j].y)
          y_min = contours[i][j].y;
      }

      tmp.append(x_min);
      tmp.append(y_min);
      tmp.append(x_max);
      tmp.append(y_max);

      out.append(tmp);
    }
    return std::move(out);
  }

  // [{"x":54, "y":32, "w":158, "h":164, "pixels":14197, "cx":131, "cy":116, "rotation":0.934584, "code":1, "count":1, "perimeter":707, "roundness":0.718467}]
  py::list find_ball(py::bytes &rgb, vector<int> &hsv_da)
  {
    string tmp = static_cast<string>(rgb);
    cv::Mat input(240, 240, CV_8UC3, const_cast<char *>(tmp.c_str()));
    Mat hsv, mask;
    cvtColor(input, hsv, COLOR_RGB2HSV);
    inRange(hsv, Scalar(hsv_da[0], hsv_da[1], hsv_da[2]), Scalar(hsv_da[3], hsv_da[4], hsv_da[5]), mask);
    // cout << hsv_da <<endl;
    Mat se = getStructuringElement(MORPH_RECT, Size(5, 5), Point(-1, -1));
    morphologyEx(mask, mask, MORPH_OPEN, se);
    vector<vector<Point>> contours;
    vector<Vec4i> hiearchy;
    findContours(mask, contours, hiearchy, RETR_EXTERNAL, CHAIN_APPROX_NONE);

    py::list out;

    for (int i = 0; i < contours.size(); i++)
    {
      /* 当拟合点数少于6个时，不进行拟合 */
      if (contours[i].size() < 6)
      {
        break;
      }

      // 圆拟合
      RotatedRect rrt = fitEllipse(contours[i]);

      int cr_x, cr_y, cr_w, cr_h;

      cr_x = rrt.center.x;
      cr_y = rrt.center.y;
      cr_w = rrt.size.width;
      cr_h = rrt.size.height;

      /* 当图形长宽相差太大 或者 图形面积太小时，不进行处理 */
      if ((abs(cr_w - cr_h) > 10) ||
          (cr_w * cr_h) < 400)
      {
        break;
      }

      py::list tmp;

      tmp.append(cr_x);
      tmp.append(cr_y);
      tmp.append(cr_w);
      tmp.append(cr_h);

      out.append(tmp);
    }
    return std::move(out);
  }
  int Distance(int x1, int y1, int x2, int y2)
  {
    int x = abs(x1 - x2);
    int y = abs(y1 - y2);
    return int(round(sqrt(x * x + y * y)));
  }

  py::dict find_line(py::bytes &rgb)
  {
    Mat src_gray, dst;
    // list<float> return_line;
    py::dict return_val;
    string tmp = static_cast<string>(rgb);
    cv::Mat input(240, 240, CV_8UC3, const_cast<char *>(tmp.c_str())); //图片输入
    Mat src_gary, mask;
    cvtColor(input, src_gray, COLOR_RGB2GRAY); //将图片变成灰度图
    Mat element = getStructuringElement(MORPH_RECT, Size(5, 5));
    erode(src_gray, src_gray, element);
    dilate(src_gray, src_gray, element);
    threshold(src_gray, src_gray, 0, 255, THRESH_BINARY | THRESH_OTSU);
    dilate(src_gray, dst, element); //膨胀

    Rect rect;
    rect.x = 0;
    rect.y = 0;
    rect.width = dst.cols;
    rect.height = heigh_t;
    dst(rect).setTo(255);
    rect.x = 0;
    rect.y = dst.rows - heigh_t;
    rect.width = dst.cols;
    rect.height = heigh_t;
    dst(rect).setTo(255);
    rect.x = 0;
    rect.y = 0;
    rect.width = heigh_t;
    rect.height = dst.rows;
    dst(rect).setTo(255);
    rect.x = dst.cols - heigh_t;
    rect.y = 0;
    rect.width = heigh_t;
    rect.height = dst.rows;
    dst(rect).setTo(255);
    vector<vector<Point>> contours;
    vector<Vec4i> hierarchy;
    findContours(dst, contours, hierarchy, RETR_TREE, CHAIN_APPROX_NONE, Point());
    if (contours.size() == 0)
    {

      return std::move(return_val);
    }
    int area = 0, a_n = 0;
    for (int i = 1; i < contours.size(); i++)
    {
      int ar = contourArea(contours[i]);
      if (ar > area)
      {
        area = ar;
        a_n = i;
      }
    }

    RotatedRect minRect = minAreaRect(contours[a_n]);
    Point2f rect_points[4];
    minRect.points(rect_points);
    py::list tmps;
    tmps.append(int(rect_points[0].x));
    tmps.append(int(rect_points[0].y));
    tmps.append(int(rect_points[1].x));
    tmps.append(int(rect_points[1].y));
    tmps.append(int(rect_points[2].x));
    tmps.append(int(rect_points[2].y));
    tmps.append(int(rect_points[3].x));
    tmps.append(int(rect_points[3].y));
    return_val["rect"] = tmps;
    return_val["pixels"] = area;
    int cx, cy;
    cx = int((((rect_points[0].x - rect_points[1].x) + (rect_points[2].x - rect_points[1].x)) / 2) + rect_points[1].x);
    cy = int((((rect_points[0].y - rect_points[1].y) + (rect_points[2].y - rect_points[1].y)) / 2) + rect_points[1].y);
    return_val["cx"] = cx;
    return_val["cy"] = cy;
    int tmp1 = Distance(int(rect_points[0].x), int(rect_points[0].y), int(rect_points[1].x), int(rect_points[1].y));
    int tmp2 = Distance(int(rect_points[0].x), int(rect_points[0].y), int(rect_points[3].x), int(rect_points[3].y));
    float x1, y1, k;
    if (tmp1 > tmp2)
    {
      x1 = rect_points[1].x - rect_points[0].x;
      y1 = rect_points[1].y - rect_points[0].y;
      k = atan(y1 / x1);
    }
    else
    {
      x1 = rect_points[3].x - rect_points[0].x;
      y1 = rect_points[3].y - rect_points[0].y;
      k = atan(y1 / x1);
    }
    return_val["rotation"] = k;
    return std::move(return_val);
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
      .def("set_ui", &_v83x_opencv::set_ui)
      .def("opencv_test", &_v83x_opencv::opencv_test)
      .def("find_blob", &_v83x_opencv::find_blob)
      .def("find_ball", &_v83x_opencv::find_ball)
      .def("find_line", &_v83x_opencv::find_line);
}
