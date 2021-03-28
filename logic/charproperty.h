#ifndef CHARPROPERTY_H
#define CHARPROPERTY_H

//Stores the properties of a char
class CharProperty
{
public:
    CharProperty(const char t_char);

    //Determines where top of character lies
    //Ascender = Extends past capital
    //Capital = All uppercase letters and numbers stop here
    //Median = Most lowercase letters stop here (and many symbols)
    //Small = Above baseline, below median
    enum class Top {Unknown, Ascender, Capital, Median, Small};
    //Determines where bottom of character lies
    //Floating = Above baseline
    //Baseline = Most characters stop here
    //Descender = Extends below baseline
    enum class Bottom {Unknown, Floating, Baseline, Descender};

    const Top topPosition;
    const Bottom bottomPosition;

private:
    static bool isAscender(const char t_char);
    static bool isCapital(const char t_char);
    static bool isMedian(const char t_char);
    static bool isSmall(const char t_char);
    static Top getTop(const char t_char);

    static bool isFloating(const char t_char);
    static bool isBaseline(const char t_char);
    static bool isDescender(const char t_char);
    static Bottom getBottom(const char t_char);
};

#endif // CHARPROPERTY_H
