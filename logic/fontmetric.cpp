#include "fontmetric.h"

#include <iostream>

#include <opencv2/imgproc.hpp>

#include <QCoreApplication>

FontMetric::FontMetric(const cv::Mat t_image, const cv::Rect t_bounds, const std::string t_text,
                       const int t_baseline)
    : image{t_image}, bounds{t_bounds}, text{t_text}, baseline{t_baseline}, descender{0}
{
    //Baseline must be atleast 2
    if (baseline < 2)
        throw std::invalid_argument("FontMetric baseline must be atleast 2");

    //Get text properties
    for (auto character: text)
        properties.push_back(CharProperty(character));

    //Find which char property in text
    bool hasAscender = false;
    bool hasCapital = false;
    bool hasMedian = false;
    //
    bool hasBaseline = false;
    bool hasDescender = false;
    for (const CharProperty property: properties)
    {
        if (property.topPosition == CharProperty::Top::Ascender)
            hasAscender = true;
        else if (property.topPosition == CharProperty::Top::Capital)
            hasCapital = true;
        else if (property.topPosition == CharProperty::Top::Median)
            hasMedian = true;

        if (property.bottomPosition == CharProperty::Bottom::Baseline)
            hasBaseline = true;
        else if (property.bottomPosition == CharProperty::Bottom::Descender)
            hasDescender = true;
    }

    //Get part of image containing text
    const cv::Mat textImage = image(bounds);

    //Get connected components of text
    cv::Mat labelImage(textImage.size(), CV_16U);
    int nLabels = cv::connectedComponents(textImage, labelImage, 8);

    //Calculate expected number of components
    int expectedComponents = text.length();
    //Characters with more than one component
    for (const char character: text)
    {
        //2 components = !":;=?ij
        if (character == '!' || character == '"' || character == ':' || character == ';' ||
            character == '=' || character == '?' || character == 'i' || character == 'j')
            ++expectedComponents;
        //3 components = %
        else if (character == '%')
            expectedComponents += 2;
    }

    //If we have the expected number of components then getting font metrics is easier :D

    //Get all labels at baseline
    std::vector<bool> baselineLabels(nLabels, false);
    uchar *labelPtr;
    //Check two pixels above and below baseline
    for (int y = baseline - 2; y <= baseline + 2; ++y)
    {
        labelPtr = labelImage.ptr<uchar>(y);
        for (int x = 0; x < labelImage.cols; ++x)
        {
            if (labelPtr[x] != 0 && !baselineLabels.at(labelPtr[x]))
                baselineLabels.at(labelPtr[x]) = true;
        }
    }

    //Calculate descender
    if (hasDescender)
    {
        //Find lowest point with a label that touches baseline
        for (int y = bounds.height - 1; y > baseline && descender == 0; ++y)
        {
            labelPtr = labelImage.ptr<uchar>(y);
            for (int x = 0; x < labelImage.cols; ++x)
            {
                if (baselineLabels.at(labelPtr[x]))
                {
                    descender = y - baseline;
                    break;
                }
            }
        }
    }
    else
    {
        //Guess descender
    }
}
