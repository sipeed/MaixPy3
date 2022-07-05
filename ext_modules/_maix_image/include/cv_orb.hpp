#ifndef cv_orb_hpp
#define cv_orb_hpp

#include "maix_image.h"

/*
from maix import *

orb = image.orb()
print(orb)

src = None
for i in range(10):
  src = camera.capture()
  display.show(src)

while True:
  img = camera.capture()
  res = orb.match(src, img, limit=30, max=100, dump=0)
  # print(len(res))
  for point in res:
    # print(point)
    img.draw_circle(point[3], point[4], 4, color=(0, 255, 0), thickness=1)
  display.show(img)
*/
struct cv_orb
{
  cv::Ptr<cv::ORB> detector;
  std::vector<cv::KeyPoint> queryPoints, trainPoints;
  cv::Mat queryDescriptors, trainDescriptors;
  void *cache_src;

  cv_orb(int nfeatures = 500, float scaleFactor = 1.2f, int nlevels = 8, int edgeThreshold = 31, int firstLevel = 0, int WTA_K = 2, int scoreType = 0, int patchSize = 31, int fastThreshold = 20)
  {
    // printf("cv_orb: %d %f %d %d %d %d %d %d %d\n", nfeatures, scaleFactor, nlevels, edgeThreshold, firstLevel, WTA_K, scoreType, patchSize, fastThreshold);
    this->detector = cv::ORB::create(nfeatures, scaleFactor, nlevels, edgeThreshold, firstLevel, WTA_K, (cv::ORB::ScoreType)scoreType, patchSize, fastThreshold);
    this->cache_src = NULL;
  }

  py::dict match(maix_image &src, maix_image &dst, int limit = 10, float min = 0.0f, float max = 1000.0f, bool crossCheck = false, bool dump = 0)
  {

    py::dict ret;
    if (NULL == src._img || NULL == dst._img)
      return ret;

    if (src._img->width != dst._img->width && src._img->height != dst._img->height)
    {
      printf("[orb] match images of the same size are required\r\n");
      return ret;
    }

    cv::Mat img_src(src._img->height, src._img->width, CV_8UC3, src._img->data);
    if (this->cache_src != src._img)
    {
      cv::cvtColor(img_src, img_src, cv::COLOR_RGB2GRAY);
      this->detector->detectAndCompute(img_src, cv::Mat(), queryPoints, queryDescriptors);
      // printf("queryPoints: %d\n", queryPoints.size());
      if (queryPoints.size() != 0)
      {
        this->cache_src = src._img;
      }
      else
      {
        this->cache_src = NULL;
        printf("[orb] this image get queryPoints.size() == 0, replace it with another image.\r\n");
        return ret;
      }
    }

    // printf("queryDescriptors: rows %d\n", queryDescriptors.rows);
    // printf("queryDescriptors: cols %d\n" , queryDescriptors.cols);
    // printf("queryDescriptors: type %d\n", queryDescriptors.type());
    // printf("queryPoints: %d\n", queryPoints.size());

    cv::Mat img_dst(dst._img->height, dst._img->width, CV_8UC3, dst._img->data);
    cv::cvtColor(img_dst, img_dst, cv::COLOR_RGB2GRAY);
    this->detector->detectAndCompute(img_dst, cv::Mat(), trainPoints, trainDescriptors);

    // printf("trainDescriptors: rows %d\n", trainDescriptors.rows);
    // printf("trainDescriptors: cols %d\n" , trainDescriptors.cols);
    // printf("trainDescriptors: type %d\n", trainDescriptors.type());
    // printf("trainPoints: %d\n", trainPoints.size());

    if (trainPoints.size() == 0)
      return ret;

    std::vector<cv::DMatch> matches;
    cv::BFMatcher matcher(cv::NORM_HAMMING, crossCheck);

    //如果采用flannBased方法 那么 desp通过orb的到的类型不同需要先转换类型
    // cv::FlannBasedMatcher matcher(new cv::flann::LshIndexParams(table_number, key_size, multi_probe_level));
    // if (queryDescriptors.type() != CV_32F || trainDescriptors.type() != CV_32F)
    // {
    //   queryDescriptors.convertTo(queryDescriptors, CV_32F);
    //   trainDescriptors.convertTo(trainDescriptors, CV_32F);
    // }

    matcher.match(queryDescriptors, trainDescriptors, matches);

    if (matches.size() == 0)
      return ret;

    if (dump) // 避免之后 matches 顺序改变
    {
      if (queryPoints.size() > 0)
      {
        cv::Mat imageOutput;
        cv::drawKeypoints(img_src, queryPoints, imageOutput, cv::Scalar::all(-1), cv::DrawMatchesFlags::DRAW_RICH_KEYPOINTS);
        cv::imwrite("cv_img_src.jpg", imageOutput);
      }
      if (trainPoints.size() > 0)
      {
        cv::Mat imageOutput;
        cv::drawKeypoints(img_dst, trainPoints, imageOutput, cv::Scalar::all(-1), cv::DrawMatchesFlags::DRAW_RICH_KEYPOINTS);
        cv::imwrite("cv_img_dst.jpg", imageOutput);
      }
      if (trainPoints.size() > queryPoints.size())
      {
        cv::Mat imageOutput;
        cv::drawMatches(img_src, queryPoints, img_dst, trainPoints, matches, imageOutput);
        cv::imwrite("cv_img_matches.jpg", imageOutput);
      }
    }

    for (int i = 0; i < queryDescriptors.rows; i++)
    {
      double dist = matches[i].distance;
      if (dist > max)
      {
        max = dist;
      }
      if (dist < min)
      {
        min = dist;
      }
    }

    std::vector<cv::DMatch> good_matches;
    for (int i = 0; i < queryDescriptors.rows; i++)
    {
      double dist = matches[i].distance;
      // printf("distance : %f\n", dist);
      if (min < dist && dist < max)
      {
        good_matches.push_back(matches[i]);
      }
      if (good_matches.size() >= limit)
        break;
    }

    if (good_matches.size() == 0)
      return ret;

    //-- Draw matches
    // cv::Mat img_matches;
    // cv::drawMatches(img_src, queryPoints, img_dst, trainPoints, good_matches, img_matches, cv::Scalar::all(-1),
    //                 cv::Scalar::all(-1), std::vector<char>(), cv::DrawMatchesFlags::NOT_DRAW_SINGLE_POINTS);
    // //-- Localize the object
    // std::vector<cv::Point2f> obj, scene;
    // for (size_t i = 0; i < good_matches.size(); i++)
    // {
    //   //-- Get the keypoints from the good matches
    //   obj.push_back(queryPoints[good_matches[i].queryIdx].pt);
    //   scene.push_back(trainPoints[good_matches[i].trainIdx].pt);
    // }
    // // for (size_t i = 0; i < obj.size(); i++)
    // // {
    // //   printf("%d   obj: %0.1f %0.1f\n", i, obj[i].x, obj[i].y);
    // //   printf("%d scene: %0.1f %0.1f\n", i, scene[i].x, scene[i].y);
    // // }
    // cv::Mat H = cv::findHomography(obj, scene, cv::RHO);
    // //-- Get the corners from the image_1 ( the object to be "detected" )
    // std::vector<cv::Point2f> obj_corners(4);
    // obj_corners[0] = cv::Point2f(0, 0);
    // obj_corners[1] = cv::Point2f((float)img_src.cols, 0);
    // obj_corners[2] = cv::Point2f((float)img_src.cols, (float)img_src.rows);
    // obj_corners[3] = cv::Point2f(0, (float)img_src.rows);
    // std::vector<cv::Point2f> scene_corners(4);
    // cv::perspectiveTransform(obj_corners, scene_corners, H);
    // //-- Draw lines between the corners (the mapped object in the scene - image_2 )
    // cv::line(img_matches, scene_corners[0] + cv::Point2f((float)img_src.cols, 0),
    //          scene_corners[1] + cv::Point2f((float)img_src.cols, 0), cv::Scalar(0, 255, 0), 4);
    // cv::line(img_matches, scene_corners[1] + cv::Point2f((float)img_src.cols, 0),
    //          scene_corners[2] + cv::Point2f((float)img_src.cols, 0), cv::Scalar(0, 255, 0), 4);
    // cv::line(img_matches, scene_corners[2] + cv::Point2f((float)img_src.cols, 0),
    //          scene_corners[3] + cv::Point2f((float)img_src.cols, 0), cv::Scalar(0, 255, 0), 4);
    // cv::line(img_matches, scene_corners[3] + cv::Point2f((float)img_src.cols, 0),
    //          scene_corners[0] + cv::Point2f((float)img_src.cols, 0), cv::Scalar(0, 255, 0), 4);

    // cv::imwrite("img_matches.jpg", img_matches);

    // py::list corners; // orb 因为匹配尺寸相同，最终计算的都两张图的相识度，所以不同于 sift 的特征点，结果只会存在有框或是无框。
    // printf("scene_corners[0] : %0.1f %0.1f\n", scene_corners[0].x, scene_corners[0].y);
    // printf("scene_corners[1] : %0.1f %0.1f\n", scene_corners[1].x, scene_corners[1].y);
    // printf("scene_corners[2] : %0.1f %0.1f\n", scene_corners[2].x, scene_corners[2].y);
    // printf("scene_corners[3] : %0.1f %0.1f\n", scene_corners[3].x, scene_corners[3].y);
    // int x = (int)scene_corners[0].x, y = (int)scene_corners[0].y, w = (int)scene_corners[2].x, h = (int)scene_corners[2].y;
    // corners.append(x);
    // corners.append(y);
    // corners.append(w);
    // corners.append(h);
    // ret["rect"] = corners;

    std::vector<cv::Point2f> obj_src, triangle;
    for (size_t i = 0; i < good_matches.size(); i++) {
      obj_src.push_back(trainPoints[good_matches[i].trainIdx].pt);
    }

    cv::RotatedRect box = cv::minAreaRect(obj_src);
    ret["angle"] = box.angle;
    ret["center"] = py::make_tuple((int)box.center.x, (int)box.center.y);
    ret["size"] = py::make_tuple((int)box.size.width, (int)box.size.height);

    py::list points;
    for (auto &p : good_matches)
    {
      points.append(py::make_tuple((int)trainPoints[p.trainIdx].pt.x, (int)trainPoints[p.trainIdx].pt.y));
    }
    ret["points"] = points;
    return ret;
  }
};

#endif