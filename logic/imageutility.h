#ifndef IMAGEUTILITY_H
#define IMAGEUTILITY_H

#include <QPixmap>
#include <opencv2/core.hpp>

namespace ImageUtility
{
//Converts an OpenCV Mat to a QPixmap
QPixmap matToQPixmap(const cv::Mat &t_mat);

//Converts an OpenCV Mat to a QImage
QImage matToQImage(const cv::Mat &t_mat);

//Converts a QImage to an OpenCV Mat
cv::Mat qImageToMat(const QImage &t_image);
};

#endif // IMAGEUTILITY_H
