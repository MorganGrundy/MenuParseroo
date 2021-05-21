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

//Merges alpha channel into image
//Treats alpha channel as binary
bool ImageUtility::mergeAlpha(QImage &t_image, const QImage &t_alpha)
{
	//Image and alpha must be same size
	//Alpha must have an alpha channel
	if (t_image.size() != t_alpha.size() || !t_alpha.hasAlphaChannel())
		return false;

	//If image has no alpha then add one
	if (!t_image.hasAlphaChannel())
		t_image = t_image.convertToFormat(QImage::Format_RGBA8888);

	//Merge alpha channels
	for (int x = 0; x < t_image.width(); ++x)
	{
		for (int y = 0; y < t_image.height(); ++y)
		{
			QColor imageColour = t_image.pixelColor(x, y);
			const QColor alphaColour = t_alpha.pixelColor(x, y);

			if (imageColour.alpha() != 0 && alphaColour.alpha() == 0)
			{
				imageColour.setAlpha(0);
				t_image.setPixelColor(x, y, imageColour);
			}
		}
	}

	return true;
}
