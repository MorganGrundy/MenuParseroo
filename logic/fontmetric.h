#ifndef FONTMETRIC_H
#define FONTMETRIC_H

#include <string>
#include <vector>

#include <opencv2/core.hpp>

#include "charproperty.h"

class FontMetric
{
public:
    FontMetric(const cv::Mat &t_image, const cv::Rect t_bounds, const std::string &t_text,
               const int t_baseline);

    //Returns bounds of text
    const cv::Rect &getBounds() const;

    //Returns text
    std::string getText() const;

    //Returns ascender
    int getAscender() const;
    //Returns capital
    int getCapital() const;
    //Returns median
    int getMedian() const;

    //Returns baseline
    int getBaseline() const;

    //Returns descender
    int getDescender() const;

    //Scales the font metric by given factor
    void scale(const double factor);

private:
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

    //----------------------------------------------------------------------------------------------
    //Returns the number of expected components for each character in text
    std::vector<size_t> getExpectedComponentCount();

    //Returns for each character in text the components that belong to it
    std::vector<std::vector<size_t>>
    mapCharacterComponents(const cv::Mat &componentImage, const size_t componentCount,
                           const std::vector<bool> &componentAtBaseline);
};

#endif // FONTMETRIC_H
