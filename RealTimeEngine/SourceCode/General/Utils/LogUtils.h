/*
 *  Errors.h
 *  Lib
 *
 *  Created by Leonardo Rojas on 1/15/11.
 *  Copyright 2011 FPL. All rights reserved.
 *
 */

#ifndef LOG_UTILS_H
#define LOG_UTILS_H

#include "vector"

class LogUtils
{
public:
    static void notifyLog(std::string log);
    
    static void notifyError(std::string error);

    static void notifyWarning(std::string warning);

    static std::vector<std::string> errors();

    static std::string lastError();
	
private:
    static std::vector<std::string> _errors;
    static std::string _lastError;
    static std::vector<std::string> _warnings;
    static std::string _lastWarning;

};

#endif
