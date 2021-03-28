#ifndef FONTMETRIC_H
#define FONTMETRIC_H

#include <string>
#include <vector>

#include <opencv2/core.hpp>

#include "charproperty.h"

class FontMetric
{
public:
    FontMetric(const std::string t_text, const int t_height, const int t_baseline);
    FontMetric(const std::string t_text, const cv::Mat t_img, const int t_baseline);

    std::string text;
    std::vector<CharProperty> properties;

    int height; //Pixels from top to bottom of text

    int ascender; //Pixels above baseline to ascender
    int capital; //Pixels above baseline to capital
    int median; //Pixels above baseline to median

    int baseline; //Pixels from top to baseline

    int descender; //Pixels below baseline to descender

private:
};

#endif // FONTMETRIC_H
