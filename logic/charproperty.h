#ifndef CHARPROPERTY_H
#define CHARPROPERTY_H

#include <string>

//Stores the properties of a char
class CharProperty
{
public:
	//!" $ &'()* / ? [\] ` {|}
	static const std::string ASCENDER_LIST;
	//# % 0123456789 @ ABCDEFGHIJKLMNOPQRSTUVWXYZ ^ b d f h ijkl t £
	static const std::string CAPITAL_LIST;
	//+ :; < > a c e g mnopqrs uvqxyz
	static const std::string MEDIAN_LIST;
	//, . _
	static const std::string SMALL_LIST;

	//" ' *+ - = ^ ` ~
	static const std::string FLOATING_LIST;
	//! # %& 0123456789: < >? ABCDEFGHIJKLMNOP RSTUVWXYZ abcdef hi klmno rstuvwx z £
	static const std::string BASELINE_LIST;
	//$ () , / ; @ Q [\] _ g j pq y {|}
	static const std::string DESCENDER_LIST;

	CharProperty(const char t_char);

	//Determines where top of character lies
	//Ascender = Extends past capital
	//Capital = All uppercase letters and numbers stop here
	//Median = Most lowercase letters stop here (and many symbols)
	//Small = Above baseline, below median
	enum class Top { Unknown, Ascender, Capital, Median, Small };
	//Determines where bottom of character lies
	//Floating = Above baseline
	//Baseline = Most characters stop here
	//Descender = Extends below baseline
	enum class Bottom { Unknown, Floating, Baseline, Descender };

	const Top topPosition;
	const Bottom bottomPosition;

private:
	static Top getTop(const char t_char);
	static Bottom getBottom(const char t_char);
};

#endif // CHARPROPERTY_H
