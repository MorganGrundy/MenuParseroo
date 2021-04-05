#include "fontmetric.h"

#include <iostream>
#include <cctype>
#include <algorithm>
#include <execution>

#include <opencv2/imgproc.hpp>

#include <QCoreApplication>

FontMetric::FontMetric(const cv::Mat &t_image, const cv::Rect t_bounds, const std::string &t_text,
                       const int t_baseline)
    : bounds{t_bounds}, text{t_text}, ascender{0}, capital{0}, median{0}, baseline{t_baseline},
    descender{0}
{
    //Enforce minimum baseline
    if (baseline < BASELINE_RANGE)
        throw std::invalid_argument("FontMetric baseline must be atleast "
                                    + std::to_string(BASELINE_RANGE));

    //Get text properties
    for (auto character: text)
        properties.push_back(CharProperty(character));

    //Text must contain alphanumerics
    if (!std::any_of(text.cbegin(), text.cend(), [](const char c){return std::isalnum(c);}))
        throw std::invalid_argument("FontMetric text must contain atleast one alphanumeric");

    //Invert image of text
    cv::Mat textImage;
    cv::bitwise_not(t_image(bounds), textImage);

    //Get connected components, stats, and centroids of text
    cv::Mat componentImage, stats, centroids;
    const size_t componentCount = cv::connectedComponentsWithStats(textImage, componentImage,
                                                                   stats, centroids, 8, CV_16U);

    //Calculate components per character and expected total
    const std::vector<size_t> charComponentCounts = getExpectedComponentCount();
    const size_t expectedComponents = std::reduce(std::execution::par_unseq,
                                                  charComponentCounts.cbegin(),
                                                  charComponentCounts.cend(), 0);

    //Get all foreground components
    std::vector<bool> componentIsForeground = getForegroundComponents(componentImage, textImage,
                                                                      componentCount);
    const size_t foregroundComponentCount = std::count(componentIsForeground.cbegin(),
                                                       componentIsForeground.cend(), true);

    //If we have the expected number of components then getting font metrics is easier :D
    if (expectedComponents != foregroundComponentCount)
    {
        std::cerr << __FILE__":" << __LINE__ << " - Expected components:" << expectedComponents <<
            " Actual components:" << foregroundComponentCount << "\n";
    }

    //Get all components at baseline
    std::vector<bool> componentAtBaseline = getBaselineComponents(stats, componentIsForeground);

    //Map characters to components
    const auto charComponents = mapCharacterComponents(stats, componentIsForeground,
                                                       componentAtBaseline);

    //Calculate median, capital, and descender
    for (size_t i = 0; i < text.length(); ++i)
    {
        const std::vector<size_t> &components = charComponents.at(i);

        //Median and capital
        //Only calculate from alphanumeric characters
        if (std::isalnum(text.at(i)))
        {
            for (const auto component: components)
            {
                const int top = stats.at<int>(static_cast<int>(component), cv::CC_STAT_TOP);
                switch (properties.at(i).topPosition)
                {
                case CharProperty::Top::Median:
                    if (median < (baseline - top))
                        median = baseline - top;
                    break;
                case CharProperty::Top::Capital:
                    if (capital < (baseline - top))
                        capital = baseline - top;
                    break;
                default: std::cerr << __FILE__":" << __LINE__
                              << " Non-alphanumeric character should not be here :(\n";
                }
            }
        }

        //Descender
        if (properties.at(i).bottomPosition == CharProperty::Bottom::Descender)
        {
            for (const auto component: components)
            {
                const int bottom = stats.at<int>(static_cast<int>(component), cv::CC_STAT_TOP)
                                   + stats.at<int>(static_cast<int>(component),
                                                   cv::CC_STAT_HEIGHT);
                if (descender < (bottom - baseline))
                    descender = bottom - baseline;
            }
        }
    }

    //Create estimate of median from capital
    if (median == 0 && capital != 0)
        median = std::round(capital * MEDIAN_CAPITAL_RATIO);
    //Create estimate of capital from median
    else if (capital == 0 && median != 0)
        capital = std::round(median * CAPITAL_MEDIAN_RATIO);

    //Create estimate of descender from capital
    if (descender == 0 && capital != 0)
        descender = std::round(capital * DESCENDER_CAPITAL_RATIO);
    //Create estimate of descender from median
    else if (descender == 0 && median != 0)
        descender = std::round(median * DESCENDER_MEDIAN_RATIO);

    //Update bounds
    bounds.y = (bounds.y + baseline) - capital;
    baseline = capital;
    bounds.height = baseline + descender;
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

//Returns which components are in the foreground
std::vector<bool> FontMetric::getForegroundComponents(const cv::Mat &componentImage,
                                                      const cv::Mat &textImage,
                                                      const size_t componentCount)
{
    std::vector<bool> componentIsForeground(componentCount, false);

    const ushort *componentPtr;
    const uchar *textPtr;
    for (int y = 0; y < componentImage.rows; ++y)
    {
        componentPtr = componentImage.ptr<ushort>(y);
        textPtr = textImage.ptr<uchar>(y);
        for (int x = 0; x < componentImage.cols; ++x)
        {
            if (textPtr[x] != 0 && !componentIsForeground.at(componentPtr[x]))
                componentIsForeground.at(componentPtr[x]) = true;
        }
    }

    return componentIsForeground;
}

//Returns which components are at the baseline
std::vector<bool> FontMetric::getBaselineComponents(const cv::Mat &stats,
                                                    const std::vector<bool> &componentIsForeground)
{
    std::vector<bool> componentAtBaseline(componentIsForeground.size(), false);
    for (size_t component = 0; component < componentIsForeground.size(); ++component)
    {
        const int top = stats.at<int>(static_cast<int>(component), cv::CC_STAT_TOP);
        const int bottom = top + stats.at<int>(static_cast<int>(component), cv::CC_STAT_HEIGHT);

        if (componentIsForeground.at(component) &&
            top < (baseline + BASELINE_RANGE) &&
            bottom > (baseline - BASELINE_RANGE))
        {
            componentAtBaseline.at(component) = true;
        }
    }

    return componentAtBaseline;
}

//Returns the maximum area of components
int FontMetric::getMaxArea(const cv::Mat &stats, const std::vector<bool> &componentIsForeground)
{
    int maxArea = 0;
    for (size_t component = 0; component < componentIsForeground.size(); ++component)
    {
        if (componentIsForeground.at(component))
        {
            if (stats.at<int>(component, cv::CC_STAT_AREA) > maxArea)
                maxArea = stats.at<int>(component, cv::CC_STAT_AREA);
        }
    }

    return maxArea;
}

//Returns for each character in text the components that belong to it
std::vector<std::vector<size_t>>
FontMetric::mapCharacterComponents(const cv::Mat &stats,
                                   const std::vector<bool> &componentIsForeground,
                                   const std::vector<bool> &componentAtBaseline)
{
    const int areaThreshold = std::round(getMaxArea(stats, componentIsForeground) * 0.05);

    std::vector<std::vector<size_t>> characterComponents(text.length());

    //Select first non-floating character
    size_t currentCharIndex = 0;
    while (currentCharIndex < text.length()
           && properties.at(currentCharIndex).bottomPosition == CharProperty::Bottom::Floating)
    {
           ++currentCharIndex;
    }

    //Sort components by stat left
    std::vector<size_t> sortedComponentsLeft(componentAtBaseline.size());
    for (size_t i = 0; i < sortedComponentsLeft.size(); ++i)
        sortedComponentsLeft.at(i) = i;
    std::sort(sortedComponentsLeft.begin(), sortedComponentsLeft.end(),
              [stats](const size_t a, const size_t b) -> bool
              {
                  return stats.at<int>(static_cast<int>(a), cv::CC_STAT_LEFT) <
                         stats.at<int>(static_cast<int>(b), cv::CC_STAT_LEFT);
              });

    //Map components to characters
    for (const auto component: sortedComponentsLeft)
    {
        if (stats.at<int>(component, cv::CC_STAT_AREA) > areaThreshold
            && componentIsForeground.at(component) && componentAtBaseline.at(component))
        {
            //Map component to current character
            characterComponents.at(currentCharIndex).push_back(component);

            //Find component above (representing the dot of i or j) and map to character
            if (text.at(currentCharIndex) == 'i' || text.at(currentCharIndex == 'j'))
            {

            }

            //Select next non-floating character
            ++currentCharIndex;
            while (currentCharIndex < text.length()
                   && properties.at(currentCharIndex).bottomPosition
                          == CharProperty::Bottom::Floating)
            {
                ++currentCharIndex;
            }

            //End of text
            if (currentCharIndex >= text.length())
                break;
        }
    }

    return characterComponents;
}
