#ifndef FONTMETRIC_H
#define FONTMETRIC_H

#include "charproperty.h"

#include <string>
#include <vector>

#include <opencv2/core.hpp>

class FontMetric
{
public:
	FontMetric(const cv::Mat &t_image, const cv::Rect t_bounds, const std::string &t_text,
		const int t_baseline);

	//Returns bounds of text
	const cv::Rect &getBounds() const;

	//Returns text
	std::string getText() const;

	//Returns ascent
	int getAscent() const;
	//Returns cap height
	int getCapHeight() const;
	//Returns x-height
	int getXHeight() const;

	//Returns pixels from top of bounds to baseline
	int getBaseline() const;

	//Returns descent
	int getDescent() const;

	//Scales the font metric by given factor
	void scale(const double factor);

private:
	//The pixel range from the baseline that a component must be in to be considered at the baseline
	static const int BASELINE_RANGE = 5;

	//The ratios between median and capital to use as fall back if only one value is calculated
	static constexpr double MEDIAN_CAPITAL_RATIO = 50.0 / 66.0;
	static constexpr double CAPITAL_MEDIAN_RATIO = 66.0 / 50.0;
	//The ratios between capital/median and descender to use as fall back if descender not calculated
	static constexpr double DESCENDER_CAPITAL_RATIO = 16.0 / 66.0;
	static constexpr double DESCENDER_MEDIAN_RATIO = 16.0 / 50.0;

	//Bounds of text in image
	cv::Rect bounds;

	std::string text;
	std::vector<CharProperty> properties;

	//Font metrics
	int ascent; //Pixels above baseline to ascender line
	int capHeight; //Pixels above baseline to capital line
	int xHeight; //Pixels above baseline to median line

	int baseline; //Pixels from top of bounds to baseline

	int descent; //Pixels below baseline to descender line

	//----------------------------------------------------------------------------------------------
	//Returns the number of expected components for each character in text
	std::vector<size_t> getExpectedComponentCount();

	//Returns which components are in the foreground
	std::vector<bool> getForegroundComponents(const cv::Mat &componentImage,
		const cv::Mat &textImage,
		const size_t componentCount);

	//Returns which components are at the baseline
	std::vector<bool> getBaselineComponents(const cv::Mat &stats,
		const std::vector<bool> &componentIsForeground);

	//Returns the maximum area of components
	int getMaxArea(const cv::Mat &stats, const std::vector<bool> &componentIsForeground);

	//Returns for each character in text the components that belong to it
	std::vector<std::vector<size_t>>
		mapCharacterComponents(const cv::Mat &stats,
			const std::vector<bool> &componentIsForeground,
			const std::vector<bool> &componentAtBaseline);
};

#endif // FONTMETRIC_H
