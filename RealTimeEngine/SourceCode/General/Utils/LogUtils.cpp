/*
 *  Errors.cpp
 *  Lib
 *
 *  Created by Leonardo Rojas on 1/15/11.
 *  Copyright 2011 FPL. All rights reserved.
 *
 */

#include "LogUtils.h"
#include "assert.h"
#include <string>

std::vector<std::string> LogUtils::_errors;
std::string LogUtils::_lastError;

std::vector<std::string> LogUtils::_warnings;
std::string LogUtils::_lastWarning;

void LogUtils::notifyLog(std::string log)
{
    log += "\n\n";
    printf("Notice: %s", log.c_str());
}

void LogUtils::notifyError(std::string error)
{
    printf("Error: %s", error.c_str());
    _lastError = error;
    _errors.push_back(error);

    assert(0);
}

void LogUtils::notifyWarning(std::string warning)
{
    warning += "\n\n";
    printf("Warning: %s", warning.c_str());
    _lastWarning = warning;
    _warnings.push_back(warning);
}
	
std::vector<std::string> LogUtils::errors() { return _errors; }

std::string LogUtils::lastError() { return _lastError; }
