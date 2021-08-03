
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

  void LineFitLeastSquares(int *data_x, int *data_y, int data_n, float *x_k, float *x_b)
  {
    float A = 0.0;
    float B = 0.0;
    float C = 0.0;
    float D = 0.0;
    float E = 0.0;
    float F = 0.0;
    for (int i = 0; i < data_n; i++)
    {
      A += data_x[i] * data_x[i];
      B += data_x[i];
      C += data_x[i] * data_y[i];
      D += data_y[i];
    }
    float a, b, temp = 0;
    if (temp = (data_n * A - B * B))
    {
      a = (data_n * C - B * D) / temp;
      b = (A * D - B * C) / temp;
    }
    else
    {
      a = 1;
      b = 0;
    }
    float Xmean, Ymean;
    Xmean = B / data_n;
    Ymean = D / data_n;
    float tempSumXX = 0.0, tempSumYY = 0.0;
    for (int i = 0; i < data_n; i++)
    {
      tempSumXX += (data_x[i] - Xmean) * (data_x[i] - Xmean);
      tempSumYY += (data_y[i] - Ymean) * (data_y[i] - Ymean);
      E += (data_x[i] - Xmean) * (data_y[i] - Ymean);
    }
    F = sqrt(tempSumXX) * sqrt(tempSumYY);
    float r;
    r = E / F;
    *x_k = a;
    *x_b = b;
  }

  int Distance(int x1, int y1, int x2, int y2) //定义拷贝构造函数
  {
    int x = abs(x1 - x2);
    int y = abs(y1 - y2);
    return int(round(sqrt(x * x + y * y)));
  }

  list<float> find_line(py::bytes &rgb)
  {
    Mat src_gray, dst;
    list<float> return_line;
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

    for (int i = 0; i < dst.rows; i += heigh_t)
    {
      rect.y = i;
      if (dst.rows - i < 100)
      {
        rect.height = dst.rows - i;
      }
      rectangle(dst, Point(rect.x, rect.y), Point(rect.x + rect.width, rect.y + rect.height), Scalar(255), 2, 8, 0);
    }

    vector<vector<Point>> contours;
    vector<Vec4i> hierarchy;
    findContours(dst, contours, hierarchy, RETR_TREE, CHAIN_APPROX_NONE, Point());

    if (contours.size() < 3 || contours.size() > 400)
    {
      // return_line.push_back(0.0);
      // return_line.push_back(0.0);
      // return_line.push_back(0.0);
      return return_line;
    }
    int x_max[500], x_min[500], y_max[500], y_min[500];
    for (int i = 0; i < contours.size(); i++)
    {
      x_max[i] = 0;
      x_min[i] = 10000;
      y_max[i] = 0;
      y_min[i] = 10000;

      for (int j = 0; j < contours[i].size(); j++)
      {
        if (x_max[i] < contours[i][j].x)
        {
          x_max[i] = contours[i][j].x;
        }
        if (x_min[i] > contours[i][j].x)
        {
          x_min[i] = contours[i][j].x;
        }
        if (y_max[i] < contours[i][j].y)
        {
          y_max[i] = contours[i][j].y;
        }
        if (y_min[i] > contours[i][j].y)
        {
          y_min[i] = contours[i][j].y;
        }
      }
    }
    vector<vector<vector<int>>> linesss;
    vector<vector<int>> liness;
    vector<int> lines;
    vector<int> line_p;
    int m;
    bool point = 0;
    int x1, x2, x3, x4, w1, w2, xmin, xmax;
    for (int i = 1; i < contours.size(); i++)
    {

      x1 = x_min[i];
      x2 = x_max[i];

      if (abs((x_max[i] - x_min[i]) * (y_max[i] - y_min[i])) < heigh_t * heigh_t * 2)
      {
        continue;
      }

      if (linesss.size() == 0)
      {
        x3 = y_min[i];
        x4 = y_max[i];

        lines.push_back(x1);
        lines.push_back(x2);
        lines.push_back(x3);
        lines.push_back(x4);

        liness.push_back(lines);
        linesss.push_back(liness);
        lines.clear();
        liness.clear();
      }
      else
      {
        point = 0;
        for (int j = 0; j < linesss.size(); j++)
        {

          m = linesss[j].size() - 1;
          // liness = linesss[j];
          // lines = liness[liness.size() - 1];

          x3 = linesss[j][m][0];
          x4 = linesss[j][m][1];

          // cout << "s1:" << j << "s2:" << liness.size() - 1 << endl;
          // int a = linesss[j].size() - 1;

          w1 = x2 - x1;
          w2 = x4 - x3;

          line_p.push_back(x1);
          line_p.push_back(x2);
          line_p.push_back(x3);
          line_p.push_back(x4);
          xmin = *min_element(line_p.begin(), line_p.end()); //min x_p
          xmax = *max_element(line_p.begin(), line_p.end()); //max x_p
          line_p.clear();

          // cout << "size:" << liness.size() << endl;

          if ((xmax - xmin) < (w1 + w2))
          {
            x3 = int((x2 - x1) / 2 + x1);                   //cx
            x4 = int((y_max[i] - y_min[i]) / 2 + y_min[i]); //cy

            w1 = int((linesss[j][m][1] - linesss[j][m][0]) / 2 + linesss[j][m][0]); //cx
            w2 = int((linesss[j][m][3] - linesss[j][m][2]) / 2 + linesss[j][m][2]); //cy

            if (Distance(x3, x4, w1, w2) < heigh_t * 3)
            {
              // cout << "x_min:" << x_min[i] << "x_max:" << x_max[i] << endl;
              // cout << "size:" << liness.size() << endl;
              x3 = y_min[i];
              x4 = y_max[i];

              lines.push_back(x1);
              lines.push_back(x2);
              lines.push_back(x3);
              lines.push_back(x4);

              linesss[j].push_back(lines);
              lines.clear();
              point = 1;
            }
          }
        }

        if (point == 0)
        {
          x3 = y_min[i];
          x4 = y_max[i];
          lines.push_back(x1);
          lines.push_back(x2);
          lines.push_back(x3);
          lines.push_back(x4);

          liness.push_back(lines);
          lines.clear();

          linesss.push_back(liness);
          liness.clear();
        }
      }
    }
    int max = 0;
    int max_num = 0;
    for (int i = 0; i < linesss.size(); i++) //选出最大的值
    {
      if (linesss[i].size() == 1) //点的数量检查
      {
        continue;
      }
      int mmax;
      m = linesss[i].size() - 1;

      x1 = int((linesss[i][0][1] - linesss[i][0][0]) / 2 + linesss[i][0][0]); //cx
      x2 = int((linesss[i][0][3] - linesss[i][0][2]) / 2 + linesss[i][0][2]); //cy

      x3 = int((linesss[i][m][1] - linesss[i][m][0]) / 2 + linesss[i][m][0]); //cx
      x4 = int((linesss[i][m][3] - linesss[i][m][2]) / 2 + linesss[i][m][2]); //cy
      mmax = Distance(x1, x2, x3, x4);
      if (max < mmax)
      {
        max = mmax;
        max_num = i;
      }
    }
    if (linesss.size() != 0)
    {
      if (linesss[max_num].size() < 2) //点数检查
      {
        // return_line.push_back(0.0);
        // return_line.push_back(0.0);
        // return_line.push_back(0.0);
        return return_line;
      }
    }
    else
    {
      return return_line;
    }
    for (int i = 0; i < linesss[max_num].size(); i++)
    {
      x_max[i] = int((linesss[max_num][i][1] - linesss[max_num][i][0]) / 2 + linesss[max_num][i][0]); //cx
      x_min[i] = int((linesss[max_num][i][3] - linesss[max_num][i][2]) / 2 + linesss[max_num][i][2]); //cy
    }

    float kkk = 0.0;
    float bbb = 0.0;
    LineFitLeastSquares(x_max, x_min, linesss[max_num].size(), &kkk, &bbb);
    return_line.push_back(kkk);
    return_line.push_back(bbb);
    return_line.push_back(float(max));
    return return_line;
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
