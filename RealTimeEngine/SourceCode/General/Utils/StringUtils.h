#ifndef STRING_UTILS_H
#define STRING_UTILS_H

#include <string>
#include <vector>
#include "Point3D.h"

#define INT_TYPE           "%i"
#define STRING_TYPE        "%s"
#define FLOAT_STRING_TYPE  "%f"
#define FLOAT_TYPE         "%.2f"
#define DOUBLE_TYPE        "%d"
#define BOOL_TYPE          "%b"

#define EMPTY_STRING ""
#define SEPARATOR_STRING ","

class StringUtils
{
public:
    static bool boolFromString(const std::string& value);
    static int intFromString(const std::string& value);
    static float floatFromString(const std::string& value);

    static std::string stringFromBool(bool value);
    static std::string stringFromInt(int value);
    static std::string stringFromFloat(float value);
    static std::string stringFromPoint3D(Point3D& point);

    static std::vector<std::string>& splitWithElements(const std::string &s, char delim, std::vector<std::string> &elems);
    static std::vector<std::string> split(const std::string &s, char delim);
};

#endif
