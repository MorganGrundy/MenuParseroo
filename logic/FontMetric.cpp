#include "FontMetric.h"

#include <iostream>
#include <cctype>
#include <algorithm>
#include <execution>

#include <opencv2/imgproc.hpp>

FontMetric::FontMetric(const cv::Mat &t_image, const cv::Rect t_bounds, const std::string &t_text,
	const int t_baseline)
	: bounds{ t_bounds }, text{ t_text }, ascent{ 0 }, capHeight{ 0 }, xHeight{ 0 }, baseline{ t_baseline },
	descent{ 0 }
{
	//Text must contain alphanumerics
	if (!std::any_of(text.cbegin(), text.cend(), [](const char c) {return std::isalnum(c); }))
		throw std::invalid_argument("FontMetric text must contain at least one alphanumeric");

	//Get text properties
	//Ascending characters are too unreliable and just interfere so treat as capital
	size_t ascendingCount = 0;
	size_t capitalCount = 0;
	size_t medianCount = 0;
	size_t descendingCount = 0;
	for (auto character : text)
	{
		properties.push_back(CharProperty(character));
		switch (properties.back().topPosition)
		{
		case CharProperty::Top::Ascender: ++ascendingCount; break;
		case CharProperty::Top::Capital: ++capitalCount; break;
		case CharProperty::Top::Median: ++medianCount; break;
		}
		if (properties.back().bottomPosition == CharProperty::Bottom::Descender)
			++descendingCount;
	}

	//Calculate row mass (sum of blurred rows)
	cv::Mat rowMasses;
	cv::reduce(t_image, rowMasses, 1, cv::ReduceTypes::REDUCE_SUM, CV_32S);
	//Calculate average row mass
	const double averageRowMass = cv::sum(rowMasses)[0] / bounds.height;

	//Calculate ascent
	if (ascendingCount > 0)
	{
		//Distance from top of image to baseline
		ascent = baseline;
	}

	//Calculate Cap height
	if (ascendingCount == 0 && capitalCount > 0)
	{
		//Distance from top of image to baseline
		capHeight = baseline;
	}
	else if (ascendingCount > 0 && capitalCount > 0)
	{
		//Calculate the percentage increase of characters from ascending to capital row
		const double charPercentageIncrease = capitalCount / static_cast<double>(ascendingCount);
		//Calculate mass threshold for capital row
		const int ascenderRow = baseline - ascent;
		const int massThreshold = std::round(calculateRowMass(rowMasses, ascenderRow) * (1.0 + charPercentageIncrease * 0.8));

		//Calculate range to search for capital row
		const int minCapitalRow = ascenderRow;
		const int maxCapitalRow = baseline - std::round(ascent * (CAPITAL_ASCENDER_RATIO_AVG + 2 * CAPITAL_ASCENDER_RATIO_STDDEV));

		//Find highest row below ascender with a mass exceeding threshold
		for (int row = minCapitalRow; row < maxCapitalRow; ++row)
		{
			if (rowMasses.at<int>(row, 0) > massThreshold)
			{
				capHeight = baseline - row;
				break;
			}
		}
	}

	//Calculate x-height
	if (ascendingCount == 0 && capitalCount == 0 && medianCount > 0)
	{
		//Distance from top of image to baseline
		xHeight = baseline;
	}
	else if (capitalCount > 0 && medianCount > 0)
	{
		//Calculate the percentage increase of characters from capital to median row
		const double charPercentageIncrease = medianCount / static_cast<double>(capitalCount + ascendingCount);
		//Calculate mass threshold for median row
		const int capitalRow = baseline - capHeight;
		const int massThreshold = std::round(calculateRowMass(rowMasses, capitalRow) * (1.0 + charPercentageIncrease * 0.8));

		//Calculate range to search for median row
		const int minMedianRow = std::max(capitalRow + 1,
			baseline - static_cast<int>(std::round(capHeight * (MEDIAN_CAPITAL_RATIO_AVG - 2 * MEDIAN_CAPITAL_RATIO_STDDEV))));
		const int maxMedianRow = baseline - std::round(capHeight * (MEDIAN_CAPITAL_RATIO_AVG + 2 * MEDIAN_CAPITAL_RATIO_STDDEV));

		//Find highest row below capital with a mass exceeding threshold
		for (int row = minMedianRow; row < maxMedianRow; ++row)
		{
			if (rowMasses.at<int>(row, 0) > massThreshold)
			{
				xHeight = baseline - row;
				break;
			}
		}
	}
	else if (ascendingCount > 0 && capitalCount == 0 && medianCount > 0)
	{
		//Calculate the percentage increase of characters from ascending to median row
		const double charPercentageIncrease = medianCount / static_cast<double>(ascendingCount);
		//Calculate mass threshold for median row
		const int ascenderRow = baseline - ascent;
		const int massThreshold = std::round(rowMasses.at<int>(ascenderRow, 0) * (1.0 + charPercentageIncrease * 0.8));

		//Calculate range to search for capital row
		const int minMedianRow = ascenderRow;
		const int maxMedianRow = baseline - std::round(ascent * (MEDIAN_ASCENDER_RATIO_AVG + 2 * MEDIAN_ASCENDER_RATIO_STDDEV));

		//Find highest row below ascender with a mass exceeding threshold
		for (int row = minMedianRow; row < maxMedianRow; ++row)
		{
			if (rowMasses.at<int>(row, 0) > massThreshold)
			{
				xHeight = baseline - row;
				break;
			}
		}
	}

	//Calculate descent
	if (descendingCount > 0)
	{
		descent = bounds.height - baseline;
	}

	//Create estimate of Descent from Cap height
	if (descent == 0 && capHeight != 0)
		descent = std::round(capHeight * DESCENDER_CAPITAL_RATIO);
	//Create estimate of Descent from x-height
	else if (descent == 0 && xHeight != 0)
		descent = std::round(xHeight * DESCENDER_MEDIAN_RATIO);

	//Create estimate of x-height from Cap height
	if (xHeight == 0 && capHeight != 0)
		xHeight = std::round(capHeight * MEDIAN_CAPITAL_RATIO_AVG);
	//Create estimate of Cap height from x-height
	else if (capHeight == 0 && xHeight != 0)
		capHeight = std::round(xHeight * CAPITAL_MEDIAN_RATIO_AVG);

	//Create estimate of Cap height from Ascent
	if (capHeight == 0 && ascent != 0)
		capHeight = std::round(ascent * CAPITAL_ASCENDER_RATIO_AVG);
	//Create estimate of x-height from Ascent
	if (xHeight == 0 && ascent != 0)
		xHeight = std::round(ascent * MEDIAN_ASCENDER_RATIO_AVG);

	//Update bounds
	bounds.y = bounds.y + baseline - std::max(ascent, capHeight);
	baseline = std::max(ascent, capHeight);
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

//Calculate the maximum mass within range of target row
int FontMetric::calculateRowMass(const cv::Mat &rowMasses, const int targetRow)
{
	int range = std::ceil(baseline * ROW_PERCENTAGE_RANGE);
	//Find max mass in range
	int mass = 0;
	for (int row = std::max(0, targetRow - range); row < targetRow + range &&
		row < rowMasses.rows; ++row)
	{
		mass = std::max(mass, rowMasses.at<int>(row, 0));
	}

	return mass;
}
