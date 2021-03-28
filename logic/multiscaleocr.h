#ifndef MULTISCALEOCR_H
#define MULTISCALEOCR_H

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
    size_t size();

    //Returns result iterator
    std::shared_ptr<tesseract::ResultIterator> getResult(const size_t i);

    //Returns scale
    double getScale(const size_t i);

    //Clears results
    void clear();

private:
    //Target scales at which to perform OCR
    std::vector<double> targetScales;
    //Actual scales at which OCR is performed (The image might not be scaled exactly)
    std::vector<double> actualScales;

    //The image to OCR
    cv::Mat image;

    //Tesseract api
    std::vector<tesseract::TessBaseAPI> tess_api;
    //Tesseract results
    std::vector<std::shared_ptr<tesseract::ResultIterator>> results;

};

#endif // MULTISCALEOCR_H
