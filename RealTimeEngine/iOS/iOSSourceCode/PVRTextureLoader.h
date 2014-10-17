/*
 *  PVRTextureLoader.h
 *  PVRTextureLoader
 *
 *  Created by Leonardo Rojas on 3/18/11.
 *  Copyright 2011 FPL. All rights reserved.
 *
 */

#ifndef PVR_TEXTURE_LOADER_H
#define PVR_TEXTURE_LOADER_H

#include <OpenGLES/ES1/gl.h>
#include <OpenGLES/ES2/glext.h>
#include <vector>

/*typedef struct _PVRTexHeader
{
        uint32_t headerLength;
        uint32_t height;
        uint32_t width;
        uint32_t numMipmaps;
        uint32_t flags;
        uint32_t dataLength;
        uint32_t bpp;
        uint32_t bitmaskRed;
        uint32_t bitmaskGreen;
        uint32_t bitmaskBlue;
        uint32_t bitmaskAlpha;
        uint32_t pvrTag;
        uint32_t numSurfs;
} PVRTexHeader;*/

typedef unsigned int       PVRTuint32;
typedef unsigned char      PVRTuint8;

typedef PVRTuint32 U32;
typedef PVRTuint8 U8;

/*!***************************************************************************
 Describes the header of a PVR header-texture
 *****************************************************************************/
struct PVR_Texture_Header
{
	PVRTuint32 dwHeaderSize;			/*!< size of the structure */
	PVRTuint32 dwHeight;				/*!< height of surface to be created */
	PVRTuint32 dwWidth;				/*!< width of input surface */
	PVRTuint32 dwMipMapCount;			/*!< number of mip-map levels requested */
	PVRTuint32 dwpfFlags;				/*!< pixel format flags */
	PVRTuint32 dwTextureDataSize;		/*!< Total size in bytes */
	PVRTuint32 dwBitCount;			/*!< number of bits per pixel  */
	PVRTuint32 dwRBitMask;			/*!< mask for red bit */
	PVRTuint32 dwGBitMask;			/*!< mask for green bits */
	PVRTuint32 dwBBitMask;			/*!< mask for blue bits */
	PVRTuint32 dwAlphaBitMask;		/*!< mask for alpha channel */
	PVRTuint32 dwPVR;					/*!< magic number identifying pvr file */
	PVRTuint32 dwNumSurfs;			/*!< the number of surfaces present in the pvr */
};

typedef struct
{
	// Uses 64 bits pre block
	U32 PackedData[2];
}AMTC_BLOCK_STRUCT;

typedef struct
{
	int target;
	int level;
	int internalformat;
	int width;
	int height;
	int format;
	int type;
	unsigned int length;
	void* data;
	bool compressed;
	bool free;
} PVRData;

typedef struct
{
	int textureType;
	int type;
	int parameter;
} GlFilter;


class PVRTextureLoader
{
public:
	PVRTextureLoader();
	PVRTextureLoader(std::string filePath);
	~PVRTextureLoader();
	
	void loadPVR(std::string filePath);
	
	//PVRTexHeader& header();
	PVR_Texture_Header* header();
	
	GlFilter& filter1();
	GlFilter& filter2();
	GlFilter& filter3();
	GlFilter& filter4();
	
	std::vector<PVRData>& imageData();
	
	/*static void PVRTTextureLoadFromPointer(	const void* pointer,
										   GLuint *const texName,
										   int* width,
										   int* height);*/
private:
	std::vector<PVRData> _imageData;
	
	//PVRTexHeader _header;
	PVR_Texture_Header* _header;
	//GLenum _internalFormat;
	void* _bytes;
	
	
	GlFilter _filter1;
	GlFilter _filter2;
	GlFilter _filter3;
	GlFilter _filter4;
	
	
	
	static void PVRTDecompressPVRTC(const void *pCompressedData,
									const int Do2bitMode,
									const int XDim,
									const int YDim,
									unsigned char* pResultImage);
	
	static void Decompress(AMTC_BLOCK_STRUCT *pCompressedData,
						   const int Do2bitMode,
						   const int XDim,
						   const int YDim,
						   const int AssumeImageTiles,
						   unsigned char* pResultImage);
};

#endif
