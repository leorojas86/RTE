/*
 *  FileUtils.h
 *  Lib
 *
 *  Created by Leonardo Rojas on 1/25/11.
 *  Copyright 2011 FPL. All rights reserved.
 *
 */

#ifndef FILE_UTILS_H
#define FILE_UTILS_H

#include <string>
#include <vector>

#define EXTENSION_SEPARATOR "."
#define DIRECTORY_SEPARATOR "/"

typedef struct
{
	void* data;
	unsigned int length;
} FileData;

class FileUtils
{
public:
    static std::string getExtension(std::string filePath);
    static std::string getName(std::string filePath, bool withExtension = true);
    static std::string replaceName(std::string filePath, std::string fileName);
    static bool exist(std::string filePath);
    static void getBytes(void* buffer,unsigned int position,unsigned int length,FILE* file);
    static FileData getData(unsigned int position,unsigned int length,FILE* file);
    static long length(FILE* file);
};

#endif

