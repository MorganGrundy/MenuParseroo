#include "fontmetric.h"

#include <iostream>
#include <cctype>
#include <algorithm>
#include <execution>

#include <opencv2/imgproc.hpp>

#include <QCoreApplication>

FontMetric::FontMetric(const cv::Mat &t_image, const cv::Rect t_bounds, const std::string &t_text,
	const int t_baseline)
	: bounds{ t_bounds }, text{ t_text }, ascent{ 0 }, capHeight{ 0 }, xHeight{ 0 }, baseline{ t_baseline },
	descent{ 0 }
{
	//Enforce minimum baseline
	if (baseline < BASELINE_RANGE)
		throw std::invalid_argument("FontMetric baseline must be at least " + std::to_string(BASELINE_RANGE));

	//Get text properties
	for (auto character : text)
		properties.push_back(CharProperty(character));

	//Text must contain alphanumerics
	if (!std::any_of(text.cbegin(), text.cend(), [](const char c) {return std::isalnum(c); }))
		throw std::invalid_argument("FontMetric text must contain at least one alphanumeric");

	//Calculate row mass (sum of blurred rows)
	cv::Mat rowMasses;
	cv::reduce(t_image, rowMasses, 1, cv::ReduceTypes::REDUCE_SUM, CV_32S);
	//Calculate average row mass
	const double averageRowMass = cv::sum(rowMasses)[0] / bounds.height;

	//Find row corresponding to the text baseline
	//The last row with a mass exceeding the average is assumed to be the baseline
	int baselineRow = -1;
	for (int row = bounds.height - 1; row >= 0; --row)
	{
		const int rowMass = rowMasses.at<int>(row, 0);
		if (rowMass > averageRowMass)
		{
			baselineRow = row;
			break;
		}
	}
	//Verify given baseline is similar to calculated baseline
	if (baselineRow >= 0)
	{
		std::cerr << "Baseline = " << baseline << ", " << baselineRow << "\n";
		if (std::abs(baseline - baselineRow) > bounds.height * 0.08)
			std::cerr << "Exceeds 8%\n";
	}
	else
		std::cerr << "No baseline\n";

	//Find row corresponding to the text median
	//The first row with a mass exceeding the average is assumed to be the median
	int medianRow = -1;
	for (int row = 0; row < bounds.height; ++row)
	{
		const int rowMass = rowMasses.at<int>(row, 0);
		if (rowMass > averageRowMass)
		{
			medianRow = row;
			break;
		}
	}
	//Set x-height
	if (medianRow >= 0)
		xHeight = baseline - medianRow;
	else
		std::cerr << "No median\n";

	//Create estimate of X-height from Cap height
	if (xHeight == 0 && capHeight != 0)
		xHeight = std::round(capHeight * MEDIAN_CAPITAL_RATIO);
	//Create estimate of Cap height from X-height
	else if (capHeight == 0 && xHeight != 0)
		capHeight = std::round(xHeight * CAPITAL_MEDIAN_RATIO);

	//Create estimate of Descent from Cap height
	if (descent == 0 && capHeight != 0)
		descent = std::round(capHeight * DESCENDER_CAPITAL_RATIO);
	//Create estimate of Descent from X-height
	else if (descent == 0 && xHeight != 0)
		descent = std::round(xHeight * DESCENDER_MEDIAN_RATIO);

	//Update bounds
	bounds.y = (bounds.y + baseline) - capHeight;
	baseline = capHeight;
	bounds.height = baseline + descent;
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

//Returns ascent
int FontMetric::getAscent() const
{
	return ascent;
}

//Returns cap height
int FontMetric::getCapHeight() const
{
	return capHeight;
}

//Returns x-height
int FontMetric::getXHeight() const
{
	return xHeight;
}

//Returns pixels from top of bounds to baseline
int FontMetric::getBaseline() const
{
	return baseline;
}

//Returns descent
int FontMetric::getDescent() const
{
	return descent;
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

		ascent = std::round(ascent * factor);
		capHeight = std::round(capHeight * factor);
		xHeight = std::round(xHeight * factor);

		baseline = std::round(baseline * factor);

		descent = std::round(descent * factor);
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
std::vector<bool> FontMetric::getForegroundComponents(const cv::Mat &componentImage, const cv::Mat &textImage,
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

		if (componentIsForeground.at(component) && top < (baseline + BASELINE_RANGE) &&
			bottom >(baseline - BASELINE_RANGE))
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
	for (int component = 0; component < componentIsForeground.size(); ++component)
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
FontMetric::mapCharacterComponents(const cv::Mat &stats, const std::vector<bool> &componentIsForeground,
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
		[stats](const size_t a, const size_t b) -> bool 		{
			return stats.at<int>(static_cast<int>(a), cv::CC_STAT_LEFT) <
				stats.at<int>(static_cast<int>(b), cv::CC_STAT_LEFT);
		});

	//Map components to characters
	for (const auto component : sortedComponentsLeft)
	{
		if (stats.at<int>(static_cast<int>(component), cv::CC_STAT_AREA) > areaThreshold
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
				&& properties.at(currentCharIndex).bottomPosition == CharProperty::Bottom::Floating)
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
