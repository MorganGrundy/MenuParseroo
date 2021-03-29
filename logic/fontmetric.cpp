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
    size_t nLabels = cv::connectedComponents(textImage, labelImage, 8);

    //Calculate components per character
    std::vector<size_t> charComponents(text.length(), 0);
    for (size_t i = 0; i < text.length(); ++i)
    {
        //3 components = %
        if (text.at(i) == '%')
            charComponents.at(i) = 3;
        //2 components = !":;=?ij
        else if (text.at(i) == '!' || text.at(i) == '"' || text.at(i) == ':' || text.at(i) == ';' ||
                 text.at(i) == '=' || text.at(i) == '?' || text.at(i) == 'i' || text.at(i) == 'j')
            charComponents.at(i) = 2;
        else
            charComponents.at(i) = 1;
    }

    //Calculate expected number of components
    const size_t expectedComponents = std::accumulate(charComponents.cbegin(),
                                                      charComponents.cend(), 0);

    //If we have the expected number of components then getting font metrics is easier :D
    if (expectedComponents != (nLabels - 1))
    {
        std::cerr << __FILE__":" << __LINE__ << " - Expected components:" << expectedComponents <<
            " Actual components:" << nLabels-1 << "\n";
    }

    //Map characters to components
    std::vector<std::vector<size_t>> characterComponents;
    size_t currentCharIndex = 0;
    size_t currentCharComponents = charComponents.at(0);
    std::vector<bool> componentIsMapped(nLabels, false);
    //Iterate columns mapping components to characters
    for (int x = 0; x < labelImage.cols && currentCharIndex < text.length(); ++x)
    {
        for (int y = 0; x < labelImage.rows && currentCharIndex < text.length(); ++y)
        {
            const uchar component = labelImage.at<uchar>(y, x);
            if (!componentIsMapped.at(component))
            {
                //Map component to current character
                characterComponents.at(currentCharIndex).push_back(component);
                componentIsMapped.at(component) = true;

                --currentCharComponents;
                //If all character components are mapped move to next character
                if (currentCharComponents == 0)
                {
                    ++currentCharIndex;
                    if (currentCharIndex > text.length())
                        break;

                    currentCharComponents = charComponents.at(currentCharIndex);
                }
            }
        }
    }

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
