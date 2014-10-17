#include "StringUtils.h"
#include <sstream>

#define STRING_SIZE 10

bool StringUtils::boolFromString(const std::string& value)
{
	return (bool)intFromString(value);
}

int StringUtils::intFromString(const std::string& value)
{
	int intValue;
	sscanf(value.c_str(),INT_TYPE,&intValue);
	return intValue;
}

float StringUtils::floatFromString(const std::string& value)
{
	float floatValue;
    sscanf(value.c_str(),FLOAT_STRING_TYPE,&floatValue);
	return floatValue;
}

std::string StringUtils::stringFromBool(bool value)
{
    return stringFromInt((int)value);
}

std::string StringUtils::stringFromInt(int value)
{
    char cString[STRING_SIZE];
    sprintf(cString, INT_TYPE,value);
    return std::string(cString);
}

std::string StringUtils::stringFromFloat(float value)
{
    char cString[STRING_SIZE];
    sprintf(cString, FLOAT_TYPE,value);
    return std::string(cString);
}

std::string StringUtils::stringFromPoint3D(Point3D& point)
{
    return stringFromFloat(point.x) + SEPARATOR_STRING + stringFromFloat(point.y) + SEPARATOR_STRING + stringFromFloat(point.z);
}

//http://stackoverflow.com/questions/236129/c-how-to-split-a-string
std::vector<std::string>& StringUtils::splitWithElements(const std::string &s, char delim, std::vector<std::string> &elems) 
{
	std::stringstream ss(s);
	std::string item;
	
	while(std::getline(ss, item, delim)) 
		elems.push_back(item);
	
	return elems;
}

//http://stackoverflow.com/questions/236129/c-how-to-split-a-string				 
std::vector<std::string> StringUtils::split(const std::string &s, char delim) 
{
	std::vector<std::string> elems;
	return splitWithElements(s, delim, elems);
}
