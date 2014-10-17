//
//  Resources.h
//
//  Created by Leonardo on 10/26/13.
//
//

#ifndef RESOURCES_H
#define RESOURCES_H

#include "StringUtils.h"

class Resources
{
public:
    static std::string rootPath;
    
    static std::string getFullPath(std::string resourceRelativePath);
};

#endif
