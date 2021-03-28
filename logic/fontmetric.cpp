#include "fontmetric.h"

#include <iostream>

#include <QCoreApplication>

FontMetric::FontMetric(const std::string t_text, const int t_height, const int t_baseline)
    : text{t_text}, height{t_height},
    ascender{0}, capital{0}, median{0},
    baseline{t_baseline},
    descender{0}
{
    for (auto character: text)
        properties.push_back(CharProperty(character));

    descender = height - baseline;

    //If any character in text reaches ascender then calculate ascender
    for (auto property: properties)
    {
        if (property.topPosition == CharProperty::Top::Ascender)
        {
            ascender = baseline;
            break;
        }
    }

    //If no character reaches ascender then check for capital
    if (ascender == 0)
    {
        for (auto property: properties)
        {
            if (property.topPosition == CharProperty::Top::Capital)
            {
                capital = baseline;
                break;
            }
        }

        //If no character reaches capital then check for median
        if (capital == 0)
        {
            for (auto property: properties)
            {
                if (property.topPosition == CharProperty::Top::Median)
                {
                    median = baseline;
                    break;
                }
            }

            //Cannot obtain matrics from the given text
            if (median == 0)
            {
                std::cerr << __FILE__":" << __LINE__ << " - Could not obtain metrics from given text " << text << "\n";
                throw std::invalid_argument("Cannot get font metrics from text: " + text);
            }
            else
                ascender = median * 1.6;
        }
        else
            ascender = capital * 1.1;
    }
}

FontMetric::FontMetric(const std::string t_text, const cv::Mat t_img, const int t_baseline)
{

}
