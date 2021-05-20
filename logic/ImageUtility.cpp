#include "ImageUtility.h"

#include <iostream>

#include <QCoreApplication>

//Converts an OpenCV Mat to a QPixmap
QPixmap ImageUtility::matToQPixmap(const cv::Mat &t_mat)
{
	//Single channel is grayscale
	if (t_mat.channels() == 1)
	{
		return QPixmap::fromImage(
			QImage(t_mat.data, t_mat.cols, t_mat.rows, static_cast<int>(t_mat.step),
				QImage::Format_Grayscale8));
	}
	//3-channel is BGR
	else if (t_mat.channels() == 3)
	{
		return QPixmap::fromImage(
			QImage(t_mat.data, t_mat.cols, t_mat.rows, static_cast<int>(t_mat.step),
				QImage::Format_BGR888));
	}

	std::cerr << __FILE__":" << __LINE__ << " - Image type not supported\n";
	QCoreApplication::exit(-1);
	return QPixmap();
}

//Converts an OpenCV Mat to a QImage
QImage ImageUtility::matToQImage(const cv::Mat &t_mat)
{
	//Single channel is grayscale
	if (t_mat.channels() == 1)
	{
		return QImage(t_mat.data, t_mat.cols, t_mat.rows, static_cast<int>(t_mat.step),
			QImage::Format_Grayscale8);
	}
	//3-channel is BGR
	else if (t_mat.channels() == 3)
	{
		return QImage(t_mat.data, t_mat.cols, t_mat.rows, static_cast<int>(t_mat.step),
			QImage::Format_RGB888).rgbSwapped();
	}
	//4-channel is BGRA
	else if (t_mat.channels() == 4)
	{
		return QImage(t_mat.data, t_mat.cols, t_mat.rows, static_cast<int>(t_mat.step),
			QImage::Format_ARGB32);
	}

	std::cerr << __FILE__":" << __LINE__ << " - Image type not supported\n";
	QCoreApplication::exit(-1);
	return QImage();
}

//Converts a QImage to an OpenCV Mat
cv::Mat ImageUtility::qImageToMat(const QImage &t_image)
{
	if (t_image.format() == QImage::Format_RGBA8888)
	{
		cv::Mat mat(t_image.height(), t_image.width(), CV_8UC4);
		uchar *rowPtr;
		for (int row = 0; row < t_image.height(); ++row)
		{
			rowPtr = mat.ptr(row);
			for (int col = 0; col < t_image.width(); ++col)
			{
				const QColor pixel = t_image.pixelColor(col, row);
				rowPtr[col * 3] = pixel.blue();
				rowPtr[col * 3 + 1] = pixel.green();
				rowPtr[col * 3 + 2] = pixel.red();
				rowPtr[col * 3 + 3] = pixel.alpha();
			}
		}

		return mat;
	}

	std::cerr << __FILE__":" << __LINE__ << " - Image type not supported\n";
	QCoreApplication::exit(-1);
	return cv::Mat();
}

//Adds an alpha channel to an image
void ImageUtility::matAddAlpha(const cv::Mat &t_in, const cv::Mat &t_alpha, cv::Mat &t_out)
{
	std::vector<cv::Mat> matChannels;
	cv::split(t_in, matChannels);

	if (t_alpha.empty())
	{
		cv::Mat newAlpha = cv::Mat(t_in.size(), CV_8UC1, cv::Scalar(255));
		matChannels.push_back(newAlpha);
	}
	else
		matChannels.push_back(t_alpha);

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
