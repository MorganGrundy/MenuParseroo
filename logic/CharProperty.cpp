#include "CharProperty.h"

const std::string CharProperty::ASCENDER_LIST = "!\"$&'()*/?[\\]`{|}";
const std::string CharProperty::CAPITAL_LIST = "#%0123456789@ABCDEFGHIJKLMNOPQRSTUVWXYZ^bdfhijklt\x9C";
const std::string CharProperty::MEDIAN_LIST = "+:;<>acegmnopqrsuvqxyz";
const std::string CharProperty::SMALL_LIST = ",._";

const std::string CharProperty::FLOATING_LIST = "\"'*+-=^`~";
const std::string CharProperty::BASELINE_LIST = "!#%&0123456789:<>?ABCDEFGHIJKLMNOPRSTUVWXYZabcdefhiklmnorstuvwxz\x9C";
const std::string CharProperty::DESCENDER_LIST = "$(),/;@Q[\\]_gjpqy{|}";

CharProperty::CharProperty(const char t_char)
	: topPosition{ getTop(t_char) }, bottomPosition{ getBottom(t_char) }
{}

CharProperty::Top CharProperty::getTop(const char t_char)
{
	if (ASCENDER_LIST.find(t_char) < ASCENDER_LIST.length())
		return Top::Ascender;
	else if (CAPITAL_LIST.find(t_char) < CAPITAL_LIST.length())
		return Top::Capital;
	else if (MEDIAN_LIST.find(t_char) < MEDIAN_LIST.length())
		return Top::Median;
	else if (SMALL_LIST.find(t_char) < SMALL_LIST.length())
		return Top::Small;
	else
		return Top::Unknown;
}

CharProperty::Bottom CharProperty::getBottom(const char t_char)
{
	if (FLOATING_LIST.find(t_char) < FLOATING_LIST.length())
		return Bottom::Floating;
	else if (BASELINE_LIST.find(t_char) < BASELINE_LIST.length())
		return Bottom::Baseline;
	else if (DESCENDER_LIST.find(t_char) < DESCENDER_LIST.length())
		return Bottom::Descender;
	else
		return Bottom::Unknown;
}
