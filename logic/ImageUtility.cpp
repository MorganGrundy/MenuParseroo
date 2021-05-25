#include "ImageUtility.h"

#include <iostream>

#include <QCoreApplication>

//Adds an alpha channel to an image
void ImageUtility::matAddAlpha(const cv::Mat &t_in, const cv::Mat &t_alpha, cv::Mat &t_out)
{
	//Split input channels
	std::vector<cv::Mat> matChannels;
	cv::split(t_in, matChannels);

	//If input image is grayscale (1 channel) add the channel again to make BGR
	if (matChannels.size() == 1)
	{
		matChannels.push_back(matChannels.front());
		matChannels.push_back(matChannels.front());
	}

	//Add alpha channel
	if (t_alpha.empty())
	{
		cv::Mat newAlpha = cv::Mat(t_in.size(), CV_8UC1, cv::Scalar(255));
		matChannels.push_back(newAlpha);
	}
	else
		matChannels.push_back(t_alpha);

	//Merge channels
	cv::merge(matChannels, t_out);
}
