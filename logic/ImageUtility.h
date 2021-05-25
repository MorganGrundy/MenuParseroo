#ifndef IMAGEUTILITY_H
#define IMAGEUTILITY_H

#include <QPixmap>
#include <opencv2/core.hpp>

namespace ImageUtility
{
	//Adds an alpha channel to an image
	void matAddAlpha(const cv::Mat &t_in, const cv::Mat &t_alpha, cv::Mat &t_out);
};

#endif // IMAGEUTILITY_H
