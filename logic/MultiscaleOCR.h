#ifndef MULTISCALEOCR_H
#define MULTISCALEOCR_H

#include "FontMetric.h"

#include <vector>

#include <tesseract/baseapi.h>
#include <leptonica/allheaders.h>

#include <opencv2/core.hpp>

#include <QObject>

class MultiscaleOCR : public QObject
{
	Q_OBJECT
public:
	MultiscaleOCR();
	~MultiscaleOCR();

	//Sets image that is OCRed
	void setImage(const cv::Mat &t_image);

	//Performs OCR on all scales
	void OCR();

	//Clears results
	void clear();
	//Returns results
	const std::vector<FontMetric> &getResults() const;

	//Const iterator for beginning of OCR results
	std::vector<FontMetric>::const_iterator begin() const;
	//Const iterator for end of OCR results
	std::vector<FontMetric>::const_iterator end() const;

signals:
	void started(int t_step);
	void progress(int t_progress);

private:
	//The image to OCR
	cv::Mat image;

	//Tesseract API
	tesseract::TessBaseAPI tess_api;
	//Tesseract results
	std::vector<FontMetric> results;
};

#endif // MULTISCALEOCR_H
