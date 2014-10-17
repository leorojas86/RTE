//
//  Resources.cpp
//
//  Created by Leonardo on 10/26/13.
//
//

#include "Resources.h"
#include "FileUtils.h"

std::string Resources::rootPath = EMPTY_STRING;

std::string Resources::getFullPath(std::string resourceRelativePath)
{
    return Resources::rootPath + DIRECTORY_SEPARATOR + resourceRelativePath;
}