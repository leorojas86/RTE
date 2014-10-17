/*
 *  FileUtils.cpp
 *  Lib
 *
 *  Created by Leonardo Rojas on 1/25/11.
 *  Copyright 2011 FPL. All rights reserved.
 *
 */

#include "FileUtils.h"
#include "StringUtils.h"
#include <iostream>
#include <fstream>

std::string FileUtils::getExtension(std::string filePath)
{
    if(filePath.find_last_of(EXTENSION_SEPARATOR) != std::string::npos)
        return filePath.substr(filePath.find_last_of(EXTENSION_SEPARATOR)+1);
    return "";
}

std::string FileUtils::getName(std::string filePath,bool withExtension)
{
    if(filePath.find_last_of(DIRECTORY_SEPARATOR) != std::string::npos)
    {
        std::string fileName = filePath.substr(filePath.find_last_of(DIRECTORY_SEPARATOR) + 1);

        if(withExtension)
            return fileName;
        else
        {
            if(fileName.find_last_of(EXTENSION_SEPARATOR) != std::string::npos)
                return fileName.substr(0, fileName.find_last_of(EXTENSION_SEPARATOR));
        }
    }

    return EMPTY_STRING;
}

std::string FileUtils::replaceName(std::string filePath, std::string fileName)
{
    if(filePath.find_last_of(DIRECTORY_SEPARATOR) != std::string::npos)
    {
        std::string path = filePath.substr(0, filePath.find_last_of(DIRECTORY_SEPARATOR) + 1);
        return path + fileName;
    }
    return "";
}

bool FileUtils::exist(std::string filePath)
{
    FILE* file = fopen(filePath.c_str(), "r");
    
    if(file != NULL)
    {
        fclose(file);
        return true;
    }
    
    return false;
}

void FileUtils::getBytes(void* buffer,unsigned int position,unsigned int length,FILE* file)
{
    fseek(file, position, SEEK_SET);
    fread(buffer, 1, length, file);
}

FileData FileUtils::getData(unsigned int position,unsigned int length,FILE* file)
{
    FileData fileData = (FileData){ malloc(length), length };
    getBytes(fileData.data, position, length, file);
    return fileData;
}

long FileUtils::length(FILE* file)
{
    fseek(file, 0L, SEEK_END);
    long length = ftell(file);
    fseek(file, 0L, SEEK_SET);
    return length;
}
