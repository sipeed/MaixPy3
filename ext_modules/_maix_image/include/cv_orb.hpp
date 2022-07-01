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

  py::list match(maix_image &src, maix_image &dst, int limit = 10, float min = 0.5f, float max = 100.0f, bool crossCheck = false, bool dump = 0)
  {

    py::list ret;
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
        cv::imwrite("cv_surf_img_src.jpg", imageOutput);
      }
      if (trainPoints.size() > 0)
      {
        cv::Mat imageOutput;
        cv::drawKeypoints(img_dst, trainPoints, imageOutput, cv::Scalar::all(-1), cv::DrawMatchesFlags::DRAW_RICH_KEYPOINTS);
        cv::imwrite("cv_surf_img_dst.jpg", imageOutput);
      }
      if (trainPoints.size() > queryPoints.size())
      {
        cv::Mat imageOutput;
        drawMatches(img_src, queryPoints, img_dst, trainPoints, matches, imageOutput);
        cv::imwrite("cv_surf_img_matches.jpg", imageOutput);
      }
    }

    //过滤出特征点距离 distance 小于 limit 以内的特征点
    for (int i = 0; i < matches.size(); i++)
    {
      // printf("distance: %f\n", matches[i].distance);
      if (matches[i].distance < max)
      {
        max = matches[i].distance;
        if (max < min)
          max = min;
        auto src_points = queryPoints[matches[i].queryIdx], dst_points = trainPoints[matches[i].trainIdx];
        auto tmp = py::make_tuple(matches[i].distance, (int)src_points.pt.x, (int)src_points.pt.y, (int)dst_points.pt.x, (int)dst_points.pt.y);
        ret.append(tmp);
      }
      if (ret.size() >= limit)
        break;
    }
    return ret;
  }
};

#endif