#include "fontmetric.h"

#include <iostream>
#include <cctype>
#include <algorithm>

#include <opencv2/imgproc.hpp>

#include <QCoreApplication>

FontMetric::FontMetric(const cv::Mat &t_image, const cv::Rect t_bounds, const std::string &t_text,
                       const int t_baseline)
    : bounds{t_bounds}, text{t_text}, ascender{0}, capital{0}, median{0}, baseline{t_baseline},
    descender{0}
{
    //Baseline must be atleast 2
    if (baseline < 5)
        throw std::invalid_argument("FontMetric baseline must be atleast 2");

    //Get text properties
    for (auto character: text)
        properties.push_back(CharProperty(character));

    //Text must contain alphanumerics
    bool hasAlphanumerics = false;
    for (const auto character: text)
    {
        if (std::isalnum(character))
        {
            hasAlphanumerics = true;
            break;
        }
    }
    if (!hasAlphanumerics)
        throw std::invalid_argument("FontMetric text must contain atleast one alphanumeric");

    //Image bounded to text
    const cv::Mat textImage = t_image(bounds);

    //Get connected components of text
    cv::Mat componentImage(textImage.size(), CV_16U);
    size_t componentCount = cv::connectedComponents(textImage, componentImage, 8, CV_16U);

    //Calculate components per character and expected total
    const std::vector<size_t> charComponentCounts = getExpectedComponentCount();
    const size_t expectedComponents = std::accumulate(charComponentCounts.cbegin(),
                                                      charComponentCounts.cend(), size_t(0));

    //If we have the expected number of components then getting font metrics is easier :D
    if (expectedComponents != (componentCount - 1))
    {
        std::cerr << __FILE__":" << __LINE__ << " - Expected components:" << expectedComponents <<
            " Actual components:" << componentCount - 1 << "\n";
    }

    //Get all components at baseline
    std::vector<bool> componentAtBaseline(componentCount, false);
    unsigned short *componentPtr;
    //Check two pixels above and below baseline
    for (int y = baseline - 5; y <= baseline + 5 && y < componentImage.rows; ++y)
    {
        componentPtr = componentImage.ptr<unsigned short>(y);
        for (int x = 0; x < componentImage.cols; ++x)
        {
            const size_t component = static_cast<size_t>(componentPtr[x]);
            if (component != 0 && !componentAtBaseline.at(component))
                componentAtBaseline.at(component) = true;
        }
    }

    //Map characters to components
    const auto charComponents = mapCharacterComponents(componentImage, componentCount,
                                                       componentAtBaseline);

    //Calculate median, capital, and descender
    for (size_t i = 0; i < text.length(); ++i)
    {
        const std::vector<size_t> &components = charComponents.at(i);

        //Median and capital
        //Only calculate from alphanumeric characters
        if (std::isalnum(text.at(i)))
        {
            //Find highest point of character component for median/capital
            for (int y = 0; y < baseline && y < componentImage.rows; ++y)
            {
                componentPtr = componentImage.ptr<unsigned short>(y);
                for (int x = 0; x < componentImage.cols; ++x)
                {
                    //Check if component belongs to character
                    if (std::find(components.cbegin(), components.cend(),
                                  componentPtr[x]) != components.cend())
                    {
                        switch (properties.at(i).topPosition)
                        {
                        case CharProperty::Top::Median:
                            if (median < (baseline - y))
                                median = baseline - y;
                            break;
                        case CharProperty::Top::Capital:
                            if (capital < (baseline - y))
                                capital = baseline - y;
                            break;
                        default: std::cerr << __FILE__":" << __LINE__ <<
                                " Non-alphanumeric character should not be here :(\n";
                        }
                    }
                }
            }
        }

        //Descender
        if (properties.at(i).bottomPosition == CharProperty::Bottom::Descender)
        {
            //Find lowest point of character component for descender
            for (int y = componentImage.rows - 1; y > baseline; ++y)
            {
                componentPtr = componentImage.ptr<unsigned short>(y);
                for (int x = 0; x < componentImage.cols; ++x)
                {
                    //Check if component belongs to character
                    if (std::find(components.cbegin(), components.cend(),
                                  componentPtr[x]) != components.cend())
                    {
                        if (descender < (y - baseline))
                            descender = y - baseline;
                    }
                }
            }
        }
    }
}

//Returns bounds of text
const cv::Rect &FontMetric::getBounds() const
{
    return bounds;
}

//Returns text
std::string FontMetric::getText() const
{
    return text;
}

//Returns ascender
int FontMetric::getAscender() const
{
    return ascender;
}

//Returns capital
int FontMetric::getCapital() const
{
    return capital;
}

//Returns median
int FontMetric::getMedian() const
{
    return median;
}

//Returns baseline
int FontMetric::getBaseline() const
{
    return baseline;
}

//Returns descender
int FontMetric::getDescender() const
{
    return descender;
}

//Scales the font metric by given factor
void FontMetric::scale(const double factor)
{
    if (factor != 1.0)
    {
        const int scaledX = std::round(bounds.x * factor);
        const int scaledY = std::round(bounds.y * factor);
        const int scaledWidth = std::round(bounds.br().x * factor - bounds.x * factor);
        const int scaledHeight = std::round(bounds.br().y * factor - bounds.y * factor);
        bounds = cv::Rect(scaledX, scaledY, scaledWidth, scaledHeight);

        ascender = std::round(ascender * factor);
        capital = std::round(capital * factor);
        median = std::round(median * factor);

        baseline = std::round(baseline * factor);

        descender = std::round(descender * factor);
    }
}

//Returns the number of expected components for each character in text
std::vector<size_t> FontMetric::getExpectedComponentCount()
{
    std::vector<size_t> charComponents(text.length(), 1);

    for (size_t i = 0; i < text.length(); ++i)
    {
        //3 components = %
        if (text.at(i) == '%')
            charComponents.at(i) = 3;
        //2 components = !":;=?ij
        else if (text.at(i) == '!' || text.at(i) == '"' || text.at(i) == ':' || text.at(i) == ';' ||
                 text.at(i) == '=' || text.at(i) == '?' || text.at(i) == 'i' || text.at(i) == 'j')
            charComponents.at(i) = 2;
    }

    return charComponents;
}

//Returns for each character in text the components that belong to it
std::vector<std::vector<size_t>>
FontMetric::mapCharacterComponents(const cv::Mat &componentImage, const size_t componentCount,
                                   const std::vector<bool> &componentAtBaseline)
{
    std::vector<std::vector<size_t>> characterComponents(text.length());

    //Select first non-floating character
    size_t currentCharIndex = 0;
    while (currentCharIndex < text.length() && properties.at(currentCharIndex).bottomPosition ==
                                                   CharProperty::Bottom::Floating)
           ++currentCharIndex;

    std::vector<bool> componentIsMapped(componentCount, false);
    //Iterate columns mapping components to characters (only rows close to baseline)
    for (int x = 0; x < componentImage.cols && currentCharIndex < text.length(); ++x)
    {
        for (int y = baseline - 5; y <= baseline + 5 && y < componentImage.rows; ++y)
        {
            const size_t component = static_cast<size_t>(componentImage.at<unsigned short>(y, x));
            if (!componentIsMapped.at(component) && componentAtBaseline.at(component))
            {
                //Map component to current character
                characterComponents.at(currentCharIndex).push_back(component);
                componentIsMapped.at(component) = true;

                //Find component above (representing the dot of i or j) and map to character
                if (text.at(currentCharIndex) == 'i' || text.at(currentCharIndex == 'j'))
                {

                }

                //Select next non-floating character
                do
                {
                    ++currentCharIndex;
                } while (currentCharIndex < text.length() &&
                         properties.at(currentCharIndex).bottomPosition ==
                             CharProperty::Bottom::Floating);

                break;
            }
        }
    }

    return characterComponents;
}
