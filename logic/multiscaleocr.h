#ifndef MULTISCALEOCR_H
#define MULTISCALEOCR_H

#include "fontmetric.h"

#include <vector>

#include <tesseract/baseapi.h>
#include <leptonica/allheaders.h>

#include <opencv2/core.hpp>

class MultiscaleOCR
{
public:
	MultiscaleOCR();
	~MultiscaleOCR();

	//Sets image that is OCRed
	void setImage(const cv::Mat &t_image);

	//Performs OCR on all scales
	void OCR();

	//Return number of scales
	size_t size() const;

	//Returns scale
	double getScale(const size_t i) const;

	//Clears results
	void clear();

	//Const iterator for beginning of OCR results
	std::vector<FontMetric>::const_iterator begin() const;
	//Const iterator for end of OCR results
	std::vector<FontMetric>::const_iterator end() const;

private:
	//Target scales at which to perform OCR
	std::vector<double> targetScales;
	//Actual scales at which OCR is performed (The image might not be scaled exactly)
	std::vector<double> actualScales;

	//The image to OCR
	cv::Mat image;

	//Tesseract api
	tesseract::TessBaseAPI tess_api;
	//Tesseract results
	std::vector<FontMetric> results;
};

#endif // MULTISCALEOCR_H
