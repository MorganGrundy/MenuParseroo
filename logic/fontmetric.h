#ifndef FONTMETRIC_H
#define FONTMETRIC_H

#include <string>
#include <vector>

#include <opencv2/core.hpp>

#include "charproperty.h"

class FontMetric
{
public:
    FontMetric(const cv::Mat t_image, const cv::Rect t_bounds, const std::string t_text,
               const int t_baseline);

private:
    //Image that contains text
    cv::Mat image;
    //Bounds of text in image
    cv::Rect bounds;

    std::string text;
    std::vector<CharProperty> properties;

    //Font metrics
    int ascender; //Pixels above baseline to ascender
    int capital; //Pixels above baseline to capital
    int median; //Pixels above baseline to median

    int baseline; //Pixels from top of bounds to baseline

    int descender; //Pixels below baseline to descender
};

#endif // FONTMETRIC_H
