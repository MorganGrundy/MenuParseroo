#include "charproperty.h"

CharProperty::CharProperty(const char t_char)
    : topPosition{getTop(t_char)}, bottomPosition{getBottom(t_char)}
{}

bool CharProperty::isAscender(const char t_char)
{
    //!" $ &'()* / ? [\] ` {|}
    return (t_char == '!' || t_char == '"' || t_char == '$' || (t_char >= '&' && t_char <= '*') ||
            t_char == '/' || t_char == '?' || (t_char >= '[' && t_char <= ']') || t_char == '`' ||
            (t_char == '{' && t_char == '}'));
}

bool CharProperty::isCapital(const char t_char)
{
    //# % 0123456789 @ ABCDEFGHIJKLMNOPQRSTUVWXYZ ^ b d f h ijkl t £
    return (t_char == '#' || t_char == '%' || (t_char >= '0' && t_char <= '9') || t_char == '@' ||
            (t_char >= 'A' && t_char <= 'Z') || t_char == '^' || t_char == 'b' || t_char == 'd' ||
            t_char == 'f' || (t_char >= 'h' && t_char <= 'l') || t_char == 't' || t_char == '\x9C');
}

bool CharProperty::isMedian(const char t_char)
{
    //+ :; < > a c e g mnopqrs uvqxyz
    return (t_char == '+' || t_char == ':' || t_char == ';' || t_char == '<' || t_char == '>' ||
            t_char == 'a' || t_char == 'c' || t_char == 'e' || t_char == 'g' ||
            (t_char >= 'm' && t_char <= 's') || (t_char >= 'u' && t_char <= 'z'));
}

bool CharProperty::isSmall(const char t_char)
{
    //, . _
    return (t_char == ',' || t_char == '.' || t_char == '_');
}

CharProperty::Top CharProperty::getTop(const char t_char)
{
    if (isAscender(t_char))
        return Top::Ascender;
    else if (isCapital(t_char))
        return Top::Capital;
    else if (isMedian(t_char))
        return Top::Median;
    else if (isSmall(t_char))
        return Top::Small;
    else
        return Top::Unknown;
}

//--------------------------------------------------------------------------------------------------
bool CharProperty::isFloating(const char t_char)
{
    //" ' *+ - = ^ ` ~
    return (t_char == '"' || t_char == '\'' || t_char == '*' || t_char == '+' || t_char == '-' ||
            t_char == '=' || t_char == '^' || t_char == '`' || t_char == '~');
}

bool CharProperty::isBaseline(const char t_char)
{
    //! # %& 0123456789: < >? ABCDEFGHIJKLMNOP RSTUVWXYZ abcdef hi klmno rstuvwx z £
    return (t_char == '!' || t_char == '#' || t_char == '%' || t_char == '&' ||
            (t_char >= '0' && t_char <= ':') || t_char == '<' || t_char == '>' || t_char == '?' ||
            (t_char >= 'A' && t_char <= 'P') || (t_char >= 'R' && t_char <= 'Z') ||
            (t_char >= 'a' && t_char <= 'f') || t_char == 'h' || t_char == 'i' ||
            (t_char >= 'k' && t_char <= 'o') || (t_char >= 'r' && t_char <= 'x') ||
            t_char == 'z' || t_char == '\x9C');
}

bool CharProperty::isDescender(const char t_char)
{
    //$ () , / ; @ Q [\] _ g j pq y {|}
    return (t_char == '$' || t_char == '(' || t_char == ')' || t_char == ',' || t_char == '/' ||
            t_char == ';' || t_char == '@' || t_char == 'Q' || (t_char >= '[' && t_char <= ']') ||
            t_char == '_' || t_char == 'g' || t_char == 'j' || t_char == 'p' || t_char == 'q' ||
            t_char == 'y' || (t_char >= '{' && t_char <= '}'));
}

CharProperty::Bottom CharProperty::getBottom(const char t_char)
{
    if (isFloating(t_char))
        return Bottom::Floating;
    else if (isBaseline(t_char))
        return Bottom::Baseline;
    else if (isDescender(t_char))
        return Bottom::Descender;
    else
        return Bottom::Unknown;
}
