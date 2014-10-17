//
//  PNGLoader.h
//  PVRTextureLoader
//
//  Created by Leonardo on 10/31/13.
//
//

#ifndef PNG_LOADER_H
#define PNG_LOADER_H

#include "png.h"
#include "pngstruct.h"
#include "pnginfo.h"
#include <string>

class PNGLoader
{
public:
    PNGLoader(std::string path);
    ~PNGLoader();
    
    int width;
    int height;
    bool hasAlphaChannel;
    png_byte* data;
    
private:
    void loadData(std::string& path);
};

#endif
