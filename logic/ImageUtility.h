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

	//Adds an alpha channel to an image
	void matAddAlpha(const cv::Mat &t_in, const cv::Mat &t_alpha, cv::Mat &t_out);

	//Merges alpha channel into image
	//Treats alpha channel as binary
	bool mergeAlpha(QImage &t_image, const QImage &t_alpha);
};

#endif // IMAGEUTILITY_H
