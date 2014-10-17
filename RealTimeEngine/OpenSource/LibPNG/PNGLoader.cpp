//
//  PNGLoader.cpp
//  PVRTextureLoader
//
//  Created by Leonardo on 10/31/13.
//
//

#include "PNGLoader.h"
#include "LogUtils.h"

PNGLoader::PNGLoader(std::string path)
{
    data            = NULL;
    width           = 0;
    height          = 0;
    hasAlphaChannel = false;
    
    loadData(path);
}

PNGLoader::~PNGLoader()
{
    if(data != NULL)
    {
        delete[] data;
        data = NULL;
    }
}

void PNGLoader::loadData(std::string& path)
{
    //header for testing if it is a png
    png_byte header[8];
    
    //open file as binary
    FILE *fp = fopen(path.c_str(), "rb");
    
    if(!fp)
    {
        LogUtils::notifyError("Can't read png file at path = " + path);
        return;
    }
    
    //read the header
    fread(header, 1, 8, fp);
    
    //test if png
    int is_png = !png_sig_cmp(header, 0, 8);
    
    if(!is_png)
    {
        fclose(fp);
        LogUtils::notifyError("File is not a png format, path = " + path);
    }
    
    //create png struct
    png_structp png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL,
                                                 NULL, NULL);
    if(!png_ptr)
    {
        fclose(fp);
        LogUtils::notifyError("Can't create png read struct for png = " + path);
    }
    
    //create png info struct
    png_infop info_ptr = png_create_info_struct(png_ptr);
    
    if(!info_ptr)
    {
        png_destroy_read_struct(&png_ptr, (png_infopp) NULL, (png_infopp) NULL);
        fclose(fp);
        LogUtils::notifyError("Can't create png info struct for png = " + path);
    }
    
    //create png info struct
    png_infop end_info = png_create_info_struct(png_ptr);
    
    if(!end_info)
    {
        png_destroy_read_struct(&png_ptr, &info_ptr, (png_infopp) NULL);
        fclose(fp);
        LogUtils::notifyError("Can't create png info struct for png = " + path);
    }
    
    //png error stuff, not sure libpng man suggests this.
    if(setjmp(png_jmpbuf(png_ptr)))
    {
        png_destroy_read_struct(&png_ptr, &info_ptr, &end_info);
        fclose(fp);
        LogUtils::notifyError("Can't create png read struct for png = " + path);
    }
    
    //init png reading
    png_init_io(png_ptr, fp);
    
    //let libpng know you already read the first 8 bytes
    png_set_sig_bytes(png_ptr, 8);
    
    // read all the info up to the image data
    png_read_info(png_ptr, info_ptr);
    
    //variables to pass to get info
    int bit_depth, color_type;
    png_uint_32 twidth, theight;
    
    // get info about png
    png_get_IHDR(png_ptr, info_ptr, &twidth, &theight, &bit_depth, &color_type, NULL, NULL, NULL);
    
    //update width and height based on png info
    width  = twidth;
    height = theight;
    
    switch(color_type)
    {
        case PNG_COLOR_TYPE_RGBA:
            hasAlphaChannel = true;
        break;
        case PNG_COLOR_TYPE_RGB:
            hasAlphaChannel = false;
        break;
        default:
            //std::cout << "Color type " << info_ptr->color_type << " not supported" << std::endl;
            png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
            fclose(fp);
            LogUtils::notifyError("Unknow color type was loaded for png = " + path);
        break;
    }
    
    // Update the png info struct.
    png_read_update_info(png_ptr, info_ptr);
    
    // Row size in bytes.
    int rowbytes = png_get_rowbytes(png_ptr, info_ptr);
    
    // Allocate the image_data as a big block, to be given to opengl
    data = new png_byte[rowbytes * height];
    
    if(!data)
    {
        //clean up memory and close stuff
        png_destroy_read_struct(&png_ptr, &info_ptr, &end_info);
        fclose(fp);
        LogUtils::notifyError("Can't allocate memory for png = " + path);
    }
    
    //row_pointers is for pointing to image_data for reading the png with libpng
    png_bytep *row_pointers = new png_bytep[height];
    
    if(!row_pointers)
    {
        //clean up memory and close stuff
        png_destroy_read_struct(&png_ptr, &info_ptr, &end_info);
        delete[] data;
        data = NULL;
        fclose(fp);
        LogUtils::notifyError("Can't allocate memory for png = " + path);
    }
    
    // set the individual row_pointers to point at the correct offsets of image_data
    for(int i = 0; i < height; ++i)
        row_pointers[height - 1 - i] = data + i * rowbytes;
    
    //read the png into image_data through row_pointers
    png_read_image(png_ptr, row_pointers);
    
    png_destroy_read_struct(&png_ptr, &info_ptr, &end_info);
    delete[] row_pointers;
    fclose(fp);
}