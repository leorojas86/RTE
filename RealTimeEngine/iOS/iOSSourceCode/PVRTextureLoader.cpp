/*
 *  PVRTextureLoader.cpp
 *  PVRTextureLoader
 *
 *  Created by Leonardo Rojas on 3/18/11.
 *  Copyright 2011 FPL. All rights reserved.
 *
 */

#include "PVRTextureLoader.h"
#include "assert.h"
#include <math.h>
#include <string>
#include "FileUtils.h"
#include "LogUtils.h"

#define PVR_TEXTURE_FLAG_TYPE_MASK	0xff

#ifndef GL_COMPRESSED_RGBA_PVRTC_4BPPV1_IMG
    #define GL_COMPRESSED_RGBA_PVRTC_4BPPV1_IMG                     0x8C02
#endif

#ifndef GL_COMPRESSED_RGBA_PVRTC_2BPPV1_IMG
    #define GL_COMPRESSED_RGBA_PVRTC_2BPPV1_IMG                     0x8C03
#endif

enum
{
	kPVRTextureFlagTypePVRTC_2 = 24,
	kPVRTextureFlagTypePVRTC_4
};

#define PVRT_MIN(a,b)            (((a) < (b)) ? (a) : (b))
#define PVRT_MAX(a,b)            (((a) > (b)) ? (a) : (b))
#define PVRT_CLAMP(x, l, h)      (PVRT_MIN((h), PVRT_MAX((x), (l))))

#define FREE(X)		{ if(X) { free(X); (X) = 0; } }


const PVRTuint32 PVRTEX_IDENTIFIER	= 0x21525650;	// the pvr identifier is the characters 'P','V','R'

const PVRTuint32 PVRTEX_CUBEMAP		= (1<<12);		// is a cubemap/skybox
const PVRTuint32 PVRTEX_TWIDDLE		= (1<<9);

const PVRTuint32 PVRTEX_V1_HEADER_SIZE = 44;			// old header size was 44 for identification purposes

const PVRTuint32 PVRTEX_PIXELTYPE		= 0xff;

const PVRTuint32 PVRTEX_MIPMAP		= (1<<8);

const PVRTuint32 PVRTC2_MIN_TEXWIDTH		= 16;
const PVRTuint32 PVRTC2_MIN_TEXHEIGHT		= 8;
const PVRTuint32 PVRTC4_MIN_TEXWIDTH		= 8;
const PVRTuint32 PVRTC4_MIN_TEXHEIGHT		= 8;
const PVRTuint32 ETC_MIN_TEXWIDTH			= 4;
const PVRTuint32 ETC_MIN_TEXHEIGHT		= 4;
const PVRTuint32 DXT_MIN_TEXWIDTH			= 4;
const PVRTuint32 DXT_MIN_TEXHEIGHT		= 4;

enum PixelType
{
	MGLPT_ARGB_4444 = 0x00,
	MGLPT_ARGB_1555,
	MGLPT_RGB_565,
	MGLPT_RGB_555,
	MGLPT_RGB_888,
	MGLPT_ARGB_8888,
	MGLPT_ARGB_8332,
	MGLPT_I_8,
	MGLPT_AI_88,
	MGLPT_1_BPP,
	MGLPT_VY1UY0,
	MGLPT_Y1VY0U,
	MGLPT_PVRTC2,
	MGLPT_PVRTC4,
	MGLPT_PVRTC2_2,
	MGLPT_PVRTC2_4,
	
	OGL_RGBA_4444= 0x10,
	OGL_RGBA_5551,
	OGL_RGBA_8888,
	OGL_RGB_565,
	OGL_RGB_555,
	OGL_RGB_888,
	OGL_I_8,
	OGL_AI_88,
	OGL_PVRTC2,
	OGL_PVRTC4,
	
	// OGL_BGRA_8888 extension
	OGL_BGRA_8888,
	OGL_A_8,
	
	D3D_DXT1 = 0x20,
	D3D_DXT2,
	D3D_DXT3,
	D3D_DXT4,
	D3D_DXT5,
	
	D3D_RGB_332,
	D3D_AI_44,
	D3D_LVU_655,
	D3D_XLVU_8888,
	D3D_QWVU_8888,
	
	//10 bits per channel
	D3D_ABGR_2101010,
	D3D_ARGB_2101010,
	D3D_AWVU_2101010,
	
	//16 bits per channel
	D3D_GR_1616,
	D3D_VU_1616,
	D3D_ABGR_16161616,
	
	//HDR formats
	D3D_R16F,
	D3D_GR_1616F,
	D3D_ABGR_16161616F,
	
	//32 bits per channel
	D3D_R32F,
	D3D_GR_3232F,
	D3D_ABGR_32323232F,
	
	// Ericsson
	ETC_RGB_4BPP,
	ETC_RGBA_EXPLICIT,
	ETC_RGBA_INTERPOLATED,
	
	// DX10
	
	
	ePT_DX10_R32G32B32A32_FLOAT= 0x50,
	ePT_DX10_R32G32B32A32_UINT ,
	ePT_DX10_R32G32B32A32_SINT,
	
	ePT_DX10_R32G32B32_FLOAT,
	ePT_DX10_R32G32B32_UINT,
	ePT_DX10_R32G32B32_SINT,
	
	ePT_DX10_R16G16B16A16_FLOAT ,
	ePT_DX10_R16G16B16A16_UNORM,
	ePT_DX10_R16G16B16A16_UINT ,
	ePT_DX10_R16G16B16A16_SNORM ,
	ePT_DX10_R16G16B16A16_SINT ,
	
	ePT_DX10_R32G32_FLOAT ,
	ePT_DX10_R32G32_UINT ,
	ePT_DX10_R32G32_SINT ,
	
	ePT_DX10_R10G10B10A2_UNORM ,
	ePT_DX10_R10G10B10A2_UINT ,
	
	ePT_DX10_R11G11B10_FLOAT ,
	
	ePT_DX10_R8G8B8A8_UNORM ,
	ePT_DX10_R8G8B8A8_UNORM_SRGB ,
	ePT_DX10_R8G8B8A8_UINT ,
	ePT_DX10_R8G8B8A8_SNORM ,
	ePT_DX10_R8G8B8A8_SINT ,
	
	ePT_DX10_R16G16_FLOAT ,
	ePT_DX10_R16G16_UNORM ,
	ePT_DX10_R16G16_UINT ,
	ePT_DX10_R16G16_SNORM ,
	ePT_DX10_R16G16_SINT ,
	
	ePT_DX10_R32_FLOAT ,
	ePT_DX10_R32_UINT ,
	ePT_DX10_R32_SINT ,
	
	ePT_DX10_R8G8_UNORM ,
	ePT_DX10_R8G8_UINT ,
	ePT_DX10_R8G8_SNORM ,
	ePT_DX10_R8G8_SINT ,
	
	ePT_DX10_R16_FLOAT ,
	ePT_DX10_R16_UNORM ,
	ePT_DX10_R16_UINT ,
	ePT_DX10_R16_SNORM ,
	ePT_DX10_R16_SINT ,
	
	ePT_DX10_R8_UNORM,
	ePT_DX10_R8_UINT,
	ePT_DX10_R8_SNORM,
	ePT_DX10_R8_SINT,
	
	ePT_DX10_A8_UNORM,
	ePT_DX10_R1_UNORM,
	ePT_DX10_R9G9B9E5_SHAREDEXP,
	ePT_DX10_R8G8_B8G8_UNORM,
	ePT_DX10_G8R8_G8B8_UNORM,
	
	ePT_DX10_BC1_UNORM,
	ePT_DX10_BC1_UNORM_SRGB,
	
	ePT_DX10_BC2_UNORM,
	ePT_DX10_BC2_UNORM_SRGB,
	
	ePT_DX10_BC3_UNORM,
	ePT_DX10_BC3_UNORM_SRGB,
	
	ePT_DX10_BC4_UNORM,
	ePT_DX10_BC4_SNORM,
	
	ePT_DX10_BC5_UNORM,
	ePT_DX10_BC5_SNORM,
	
	//ePT_DX10_B5G6R5_UNORM,			// defined but obsolete - won't actually load in DX10
	//ePT_DX10_B5G5R5A1_UNORM,
	//ePT_DX10_B8G8R8A8_UNORM,
	//ePT_DX10_B8G8R8X8_UNORM,
	
	// OpenVG
	
	// RGB{A,X} channel ordering 
	ePT_VG_sRGBX_8888  = 0x90,
	ePT_VG_sRGBA_8888,
	ePT_VG_sRGBA_8888_PRE,
	ePT_VG_sRGB_565,
	ePT_VG_sRGBA_5551,
	ePT_VG_sRGBA_4444,
	ePT_VG_sL_8,
	ePT_VG_lRGBX_8888,
	ePT_VG_lRGBA_8888,
	ePT_VG_lRGBA_8888_PRE,
	ePT_VG_lL_8,
	ePT_VG_A_8,
	ePT_VG_BW_1,
	
	// {A,X}RGB channel ordering 
	ePT_VG_sXRGB_8888,
	ePT_VG_sARGB_8888,
	ePT_VG_sARGB_8888_PRE,
	ePT_VG_sARGB_1555,
	ePT_VG_sARGB_4444,
	ePT_VG_lXRGB_8888,
	ePT_VG_lARGB_8888,
	ePT_VG_lARGB_8888_PRE,
	
	// BGR{A,X} channel ordering 
	ePT_VG_sBGRX_8888,
	ePT_VG_sBGRA_8888,
	ePT_VG_sBGRA_8888_PRE,
	ePT_VG_sBGR_565,
	ePT_VG_sBGRA_5551,
	ePT_VG_sBGRA_4444,
	ePT_VG_lBGRX_8888,
	ePT_VG_lBGRA_8888,
	ePT_VG_lBGRA_8888_PRE,
	
	// {A,X}BGR channel ordering 
	ePT_VG_sXBGR_8888,
	ePT_VG_sABGR_8888 ,
	ePT_VG_sABGR_8888_PRE,
	ePT_VG_sABGR_1555,
	ePT_VG_sABGR_4444,
	ePT_VG_lXBGR_8888,
	ePT_VG_lABGR_8888,
	ePT_VG_lABGR_8888_PRE,
	
	// max cap for iterating
	END_OF_PIXEL_TYPES,
	
	MGLPT_NOTYPE = 0xff
	
};

PVRTextureLoader::PVRTextureLoader()
{
    _bytes = NULL;
}

PVRTextureLoader::PVRTextureLoader(std::string filePath)
{
	_bytes = NULL;
	loadPVR(filePath);
}

PVRTextureLoader::~PVRTextureLoader()
{
    if(_bytes)
        free(_bytes);
	
	for(std::vector<PVRData>::iterator it = _imageData.begin(); it != _imageData.end(); it++)
	{
		PVRData pvrData = *it;
		if(pvrData.free)
			free(pvrData.data);
	}
}

/*PVRTexHeader& PVRTextureLoader::header()
{
    return _header;
}*/

PVR_Texture_Header* PVRTextureLoader::header() { return _header; }

std::vector<PVRData>& PVRTextureLoader::imageData() { return _imageData; }

GlFilter& PVRTextureLoader::filter1(){ return _filter1; }
GlFilter& PVRTextureLoader::filter2(){ return _filter2; }
GlFilter& PVRTextureLoader::filter3(){ return _filter3; }
GlFilter& PVRTextureLoader::filter4(){ return _filter4; }


/*void PVRTextureLoader::loadPVR(std::string filePath)
{ 
	if(_bytes)
		delete _bytes;
	
	FILE* file = fopen(filePath.c_str(), "r");
	FileUtils::getBytes(&_header, 0, sizeof(PVRTexHeader), file);
	
	uint32_t pvrTag = _header.pvrTag;

	if ('P' != ((pvrTag >>  0) & 0xff) ||
		'V' != ((pvrTag >>  8) & 0xff) ||
		'R' != ((pvrTag >> 16) & 0xff) ||
		'!' != ((pvrTag >> 24) & 0xff))
	{
		Errors::notifyError("The provided file is not a pvr format " + filePath);
	}
	
	uint32_t formatFlags = _header.flags & PVR_TEXTURE_FLAG_TYPE_MASK;
	
	if (formatFlags == kPVRTextureFlagTypePVRTC_4 || formatFlags == kPVRTextureFlagTypePVRTC_2)
	{
                //_imageData.clear();
		_internalFormat = formatFlags == kPVRTextureFlagTypePVRTC_4 ? GL_COMPRESSED_RGBA_PVRTC_4BPPV1_IMG : GL_COMPRESSED_RGBA_PVRTC_2BPPV1_IMG;

		uint32_t width  = _header.width;
		uint32_t height = _header.height;
		uint32_t bpp    = 4;
		uint32_t dataOffset = 0, dataSize = 0;
		uint32_t blockSize  = 0, widthBlocks = 0, heightBlocks = 0;

		long fileLength      = FileUtils::length(file);
		size_t headerLength  = sizeof(PVRTexHeader);
		size_t bufferSize    = fileLength - headerLength;
		_bytes			     = (uint8_t*)malloc(bufferSize);
		
		FileUtils::getBytes(_bytes, headerLength, bufferSize, file);
		
		bool isPVR4 = formatFlags == kPVRTextureFlagTypePVRTC_4;
		// Calculate the data size for each texture level and respect the minimum number of blocks
		while (dataOffset < _header.dataLength)
		{
			blockSize    = (isPVR4 ? 4 : 8) * 4; // Pixel by pixel block size for 4bpp
			widthBlocks  = width / (isPVR4 ? 4 : 8);
			heightBlocks = height / 4;
			bpp          = isPVR4 ? 4 : 2;
			
			// Clamp to minimum number of blocks
			widthBlocks  = fmaxf(widthBlocks, 2);
			heightBlocks = fmaxf(heightBlocks, 2);
			
			dataSize = widthBlocks * heightBlocks * ((blockSize  * bpp) / 8);
			
			FileData fileData = (FileData){_bytes + dataOffset, dataSize};
			_imageData.push_back(fileData);
			
			dataOffset += dataSize;
			
			width  = fmaxf(width >> 1, 1);
			height = fmaxf(height >> 1, 1);
		}
	}
	
	fclose(file);
}*/

void PVRTextureLoader::loadPVR(std::string filePath)
{
	FILE* file        = fopen(filePath.c_str(), "r");
	long length       =  FileUtils::length(file);
	_bytes			  = malloc(length);
	FileUtils::getBytes(_bytes, 0, length, file);
	fclose(file);
	
	bool bAllowDecompress = true;
	
	_header = (PVR_Texture_Header*)_bytes;
	unsigned int u32NumSurfs = 0;
	
	if(_header->dwHeaderSize != sizeof(PVR_Texture_Header))
	{
		if(_header->dwHeaderSize == PVRTEX_V1_HEADER_SIZE)
			u32NumSurfs = _header->dwpfFlags & PVRTEX_CUBEMAP ? 6 : 1;
		//else
			//Errors::notifyError("PVRTTextureLoadFromPointer failed: not a valid pvr.\n");
	}
	else
	{	// Header V2
		if(_header->dwNumSurfs < 1)
			u32NumSurfs = _header->dwpfFlags & PVRTEX_CUBEMAP ? 6 : 1;
		else
			u32NumSurfs = _header->dwNumSurfs;
	}
	
	GLenum eTextureFormat		  = 0;
	GLenum eTextureInternalFormat = 0;	// often this is the same as textureFormat, but not for BGRA8888 on the iPhone, for instance
	GLenum eTextureType           = 0;
	GLenum eTarget;
	
	bool bIsPVRTCSupported = true;
	bool bIsBGRA8888Supported = true;
	bool bIsFloat16Supported = true;
	bool bIsFloat32Supported = true;	
	bool bIsETCSupported = true;
	
	bool bIsCompressedFormatSupported = false, bIsCompressedFormat = false;
	
	/* Only accept untwiddled data UNLESS texture format is PVRTC */
	if ( ((_header->dwpfFlags & PVRTEX_TWIDDLE) == PVRTEX_TWIDDLE)
		&& ((_header->dwpfFlags & PVRTEX_PIXELTYPE)!= OGL_PVRTC2)
		&& ((_header->dwpfFlags & PVRTEX_PIXELTYPE)!= OGL_PVRTC4) )
    {
		//Errors::notifyError("PVRTTextureLoadFromPointer failed: texture should be untwiddled.");
    }
	
	unsigned int ePixelType = _header->dwpfFlags & PVRTEX_PIXELTYPE;
	
	switch(ePixelType)
	{
		case OGL_RGBA_4444:
			eTextureFormat = eTextureInternalFormat = GL_RGBA;
			eTextureType   = GL_UNSIGNED_SHORT_4_4_4_4;
		break;
		case OGL_RGBA_5551:
			eTextureFormat = eTextureInternalFormat = GL_RGBA ;
			eTextureType   = GL_UNSIGNED_SHORT_5_5_5_1;
		break;
		case OGL_RGBA_8888:
			eTextureFormat = eTextureInternalFormat = GL_RGBA;
			eTextureType   = GL_UNSIGNED_BYTE ;
		break;
			/* New OGL Specific Formats Added */
		case OGL_RGB_565:
			eTextureFormat = eTextureInternalFormat = GL_RGB ;
			eTextureType   = GL_UNSIGNED_SHORT_5_6_5;
		break;
		case OGL_RGB_555:
			//Errors::notifyError("PVRTTextureLoadFromPointer failed: pixel type OGL_RGB_555 not supported.");
		break;
		case OGL_RGB_888:
			eTextureFormat = eTextureInternalFormat = GL_RGB ;
			eTextureType   = GL_UNSIGNED_BYTE;
		break;
		case OGL_I_8:
			eTextureFormat = eTextureInternalFormat = GL_LUMINANCE;
			eTextureType   = GL_UNSIGNED_BYTE;
		break;
		case OGL_AI_88:
			eTextureFormat = eTextureInternalFormat = GL_LUMINANCE_ALPHA ;
			eTextureType   = GL_UNSIGNED_BYTE;
		break;
		case MGLPT_PVRTC2:
		case OGL_PVRTC2:
			if(bIsPVRTCSupported)
			{
				bIsCompressedFormatSupported = bIsCompressedFormat = true;
				eTextureFormat = _header->dwAlphaBitMask==0 ? GL_COMPRESSED_RGB_PVRTC_2BPPV1_IMG : GL_COMPRESSED_RGBA_PVRTC_2BPPV1_IMG ;	// PVRTC2
			}
			else
			{
				if(bAllowDecompress)
				{
					bIsCompressedFormatSupported = false;
					bIsCompressedFormat          = true;
					eTextureFormat               = eTextureInternalFormat = GL_RGBA;
					eTextureType                 = GL_UNSIGNED_BYTE;
					//PVRTErrorOutputDebug("PVRTTextureLoadFromPointer warning: PVRTC2 not supported. Converting to RGBA8888 instead.\n");
				}
				else
					LogUtils::notifyError("PVRTTextureLoadFromPointer error: PVRTC2 not supported.");
			}
		break;
		case MGLPT_PVRTC4:
		case OGL_PVRTC4:
			if(bIsPVRTCSupported)
			{
				bIsCompressedFormatSupported = bIsCompressedFormat = true;
				eTextureFormat = _header->dwAlphaBitMask==0 ? GL_COMPRESSED_RGB_PVRTC_4BPPV1_IMG : GL_COMPRESSED_RGBA_PVRTC_4BPPV1_IMG ;	// PVRTC4
			}
			else
			{
				if(bAllowDecompress)
				{
					bIsCompressedFormatSupported = false;
					bIsCompressedFormat          = true;
					eTextureFormat               = eTextureInternalFormat = GL_RGBA;
					eTextureType                 = GL_UNSIGNED_BYTE;
					//PVRTErrorOutputDebug("PVRTTextureLoadFromPointer warning: PVRTC4 not supported. Converting to RGBA8888 instead.\n");
				}
				else
					LogUtils::notifyError("PVRTTextureLoadFromPointer error: PVRTC4 not supported.");
			}
		break;
		case OGL_A_8:
			eTextureFormat = eTextureInternalFormat = GL_ALPHA;
			eTextureType   = GL_UNSIGNED_BYTE;
		break;
		case OGL_BGRA_8888:
			if(bIsBGRA8888Supported)
			{
				eTextureType = GL_UNSIGNED_BYTE;
#ifndef __APPLE__
				eTextureFormat = eTextureInternalFormat = GL_BGRA;
#else
				eTextureFormat         = GL_BGRA;
				eTextureInternalFormat = GL_RGBA;
#endif
			}
			else
				LogUtils::notifyError("PVRTTextureLoadFromPointer failed: Unable to load GL_BGRA texture as extension GL_IMG_texture_format_BGRA8888 is unsupported.");
		break;
		case D3D_ABGR_16161616F:
			if(bIsFloat16Supported)
			{
				eTextureFormat = eTextureInternalFormat =  GL_RGBA ;
				eTextureType   = GL_HALF_FLOAT_OES;
			}
		break;
		case D3D_ABGR_32323232F:
			if(bIsFloat32Supported)
			{
				eTextureFormat = eTextureInternalFormat =  GL_RGBA ;
				eTextureType = GL_FLOAT;
			}
		break;
#ifndef TARGET_OS_IPHONE
		case ETC_RGB_4BPP:
			if(bIsETCSupported)
			{
				bIsCompressedFormatSupported = bIsCompressedFormat = true;
				//eTextureFormat = GL_ETC1_RGB8_OES;
			}
			else
			{
				if(bAllowDecompress)
				{
					bIsCompressedFormatSupported = false;
					bIsCompressedFormat = true;
					eTextureFormat = GL_RGBA;
					eTextureType = GL_UNSIGNED_BYTE;
					//PVRTErrorOutputDebug("PVRTTextureLoadFromPointer warning: ETC not supported. Converting to RGBA8888 instead.\n");
				}
				else
					LogUtils::notifyError("PVRTTextureLoadFromPointer error: ETC not supported.");
			}
		break;
#endif
			
		default:											// NOT SUPPORTED
			LogUtils::notifyError("PVRTTextureLoadFromPointer failed: pixel type not supported.");
		break;
	}
	//  check that this data is cube map data or not.
	eTarget = _header->dwpfFlags & PVRTEX_CUBEMAP ? GL_TEXTURE_CUBE_MAP : GL_TEXTURE_2D;
	
	bool isOGL_PVRTC2	  = (_header->dwpfFlags & PVRTEX_PIXELTYPE) == OGL_PVRTC2;
	bool isOGL_PVRTC4	  = (_header->dwpfFlags & PVRTEX_PIXELTYPE) == OGL_PVRTC4;
	bool isPVRTEX_CUBEMAP = _header->dwpfFlags & PVRTEX_CUBEMAP;
	
	for(unsigned int i=0; i < u32NumSurfs; i++)
	{	
		char *theTexturePtr	= (char*)_header + _header->dwHeaderSize + _header->dwTextureDataSize * i;
		int		nTextureLevelsNeeded = (_header->dwpfFlags & PVRTEX_MIPMAP)? _header->dwMipMapCount : 0;
		unsigned int		nSizeX   = _header->dwWidth, nSizeY = _header->dwHeight;
		unsigned int		compressedImageSize = 0;
		
		for(int	x = 0; x <= nTextureLevelsNeeded; nSizeX = PVRT_MAX(nSizeX/2, (unsigned int)1), nSizeY = PVRT_MAX(nSizeY/2, (unsigned int)1), x++)
		{
			char *theTextureToLoad = theTexturePtr;
			
			// Load the Texture
			// If the texture is PVRTC or ETC then use GLCompressedTexImage2D
			if(bIsCompressedFormat)
			{
				/* Calculate how many bytes this MIP level occupies */
				if(isOGL_PVRTC2)
					compressedImageSize = ( PVRT_MAX(nSizeX, PVRTC2_MIN_TEXWIDTH) * PVRT_MAX(nSizeY, PVRTC2_MIN_TEXHEIGHT) * _header->dwBitCount) / 8;
				else if(isOGL_PVRTC4)
					compressedImageSize = ( PVRT_MAX(nSizeX, PVRTC4_MIN_TEXWIDTH) * PVRT_MAX(nSizeY, PVRTC4_MIN_TEXHEIGHT) * _header->dwBitCount) / 8;
				else// ETC
					compressedImageSize = ( PVRT_MAX(nSizeX, ETC_MIN_TEXWIDTH) * PVRT_MAX(nSizeY, ETC_MIN_TEXHEIGHT) * _header->dwBitCount) / 8;
				
				
				if((signed int)x >= 0)
				{
					if(bIsCompressedFormatSupported)
					{
                        PVRData pvrData        = PVRData();
                        pvrData.target         = isPVRTEX_CUBEMAP ? GL_TEXTURE_CUBE_MAP_POSITIVE_X+i : GL_TEXTURE_2D;
                        pvrData.level          = x;
                        pvrData.internalformat = eTextureFormat;
                        pvrData.width          = nSizeX;
                        pvrData.height         = nSizeY;
                        pvrData.format         = 0;
                        pvrData.type           = 0;
                        pvrData.length         = compressedImageSize;
                        pvrData.data           = theTextureToLoad;
                        pvrData.compressed     = true;
                        pvrData.free           = false;
                        
                        _imageData.push_back(pvrData);
                    }
					else
					{
						// Convert PVRTC to 32-bit
						PVRTuint8 *u8TempTexture = (PVRTuint8*)malloc(nSizeX*nSizeY*4);
						if (isOGL_PVRTC2)
							PVRTDecompressPVRTC(theTextureToLoad, 1, nSizeX, nSizeY, u8TempTexture);
						else if(isOGL_PVRTC4)
							PVRTDecompressPVRTC(theTextureToLoad, 0, nSizeX, nSizeY, u8TempTexture);
						else
						{	
							LogUtils::notifyError("ETC decompresion is not implemented.");
							// ETC
							//PVRTDecompressETC(theTextureToLoad, nSizeX, nSizeY, u8TempTexture, 0);
						}
                        
                        PVRData pvrData        = PVRData();
                        pvrData.target         = isPVRTEX_CUBEMAP ? GL_TEXTURE_CUBE_MAP_POSITIVE_X+i : GL_TEXTURE_2D;
                        pvrData.level          = x;
                        pvrData.internalformat = GL_RGBA;
                        pvrData.width          = nSizeX;
                        pvrData.height         = nSizeY;
                        pvrData.format         = GL_RGBA;
                        pvrData.type           = GL_UNSIGNED_BYTE;
                        pvrData.length         = 0;
                        pvrData.data           = u8TempTexture;
                        pvrData.compressed     = false;
                        pvrData.free           = true;
                        
                        _imageData.push_back(pvrData);
					}
				}
			}
			else
			{
				if((signed int)x >= 0)
				{
                    PVRData pvrData        = PVRData();
                    pvrData.target         = isPVRTEX_CUBEMAP ? GL_TEXTURE_CUBE_MAP_POSITIVE_X+i : GL_TEXTURE_2D;
                    pvrData.level          = x;
                    pvrData.internalformat = eTextureInternalFormat;
                    pvrData.width          = nSizeX;
                    pvrData.height         = nSizeY;
                    pvrData.format         = eTextureFormat;
                    pvrData.type           = eTextureType;
                    pvrData.length         = 0;
                    pvrData.data           = theTextureToLoad;
                    pvrData.compressed     = false;
                    pvrData.free           = false;
                    
                    _imageData.push_back(pvrData);
				}
			}
			// offset the texture pointer by one mip-map level
			
			/* PVRTC case */
			if( bIsCompressedFormat )
				theTexturePtr += compressedImageSize;
			else/* New formula that takes into account bit counts inferior to 8 (e.g. 1 bpp) */
				theTexturePtr += (nSizeX * nSizeY * _header->dwBitCount + 7) / 8;
		}
	}
	
	if(eTextureType==GL_FLOAT || eTextureType==GL_HALF_FLOAT_OES)
	{
		if(!_header->dwMipMapCount)
		{	// Texture filter modes are limited to these for float textures
            GlFilter filter1    = GlFilter();
            filter1.textureType = eTarget;
            filter1.type        = GL_TEXTURE_MIN_FILTER;
            filter1.parameter   = GL_NEAREST;
			_filter1            = filter1;
            
            GlFilter filter2    = GlFilter();
            filter2.textureType = eTarget;
            filter2.type        = GL_TEXTURE_MAG_FILTER;
            filter2.parameter   = GL_NEAREST;
			_filter2            = filter2;
		}
		else
		{
            GlFilter filter1    = GlFilter();
            filter1.textureType = eTarget;
            filter1.type        = GL_TEXTURE_MIN_FILTER;
            filter1.parameter   = GL_NEAREST_MIPMAP_NEAREST;
			_filter1            = filter1;
            
            GlFilter filter2    = GlFilter();
            filter2.textureType = eTarget;
            filter2.type        = GL_TEXTURE_MAG_FILTER;
            filter2.parameter   = GL_NEAREST;
			_filter2            = filter2;
		}
	}
	else
	{
		if(!_header->dwMipMapCount)
		{
            GlFilter filter1    = GlFilter();
            filter1.textureType = eTarget;
            filter1.type        = GL_TEXTURE_MIN_FILTER;
            filter1.parameter   = GL_LINEAR;
			_filter1            = filter1;
            
            GlFilter filter2    = GlFilter();
            filter2.textureType = eTarget;
            filter2.type        = GL_TEXTURE_MAG_FILTER;
            filter2.parameter   = GL_LINEAR;
			_filter2            = filter2;
		}
		else
		{
            GlFilter filter1    = GlFilter();
            filter1.textureType = eTarget;
            filter1.type        = GL_TEXTURE_MIN_FILTER;
            filter1.parameter   = GL_LINEAR_MIPMAP_NEAREST;
			_filter1            = filter1;
            
            GlFilter filter2    = GlFilter();
            filter2.textureType = eTarget;
            filter2.type        = GL_TEXTURE_MAG_FILTER;
            filter2.parameter   = GL_LINEAR;
			_filter2            = filter2;
		}
	}
	
	if((_header->dwWidth & (_header->dwWidth - 1)) |
	   (_header->dwHeight & (_header->dwHeight - 1)))
	{
		//NPOT textures requires the wrap mode to be set explicitly to
		//GL_CLAMP_TO_EDGE or the texture will be inconsistent.
        GlFilter filter3    = GlFilter();
        filter3.textureType = eTarget;
        filter3.type        = GL_TEXTURE_WRAP_S;
        filter3.parameter   = GL_CLAMP_TO_EDGE;
		_filter3            = filter3;
        
        GlFilter filter4    = GlFilter();
        filter4.textureType = eTarget;
        filter4.type        = GL_TEXTURE_WRAP_T;
        filter4.parameter   = GL_CLAMP_TO_EDGE;
		_filter4            = filter4;
	}
	else
	{
        GlFilter filter3    = GlFilter();
        filter3.textureType = GL_TEXTURE_2D;
        filter3.type        = GL_TEXTURE_WRAP_S;
        filter3.parameter   = GL_REPEAT;
		_filter3            = (GlFilter) filter3;
        
        GlFilter filter4    = GlFilter();
        filter4.textureType = GL_TEXTURE_2D;
        filter4.type        = GL_TEXTURE_WRAP_T;
        filter4.parameter   = GL_REPEAT;
		_filter4            = filter4;
	}
}























/*!***************************************************************************
 @Function		PVRTTextureLoadFromPointer
 @Input			pointer				Pointer to header-texture's structure
 @Modified		texName				the OpenGL ES texture name as returned by glBindTexture
 @Modified		psTextureHeader		Pointer to a PVR_Texture_Header struct. Modified to
 contain the header data of the returned texture Ignored if NULL.
 @Input			bAllowDecompress	Allow decompression if PVRTC is not supported in hardware.
 @Input			nLoadFromLevel		Which mipmap level to start loading from (0=all)
 @Input			texPtr				If null, texture follows header, else texture is here.
 @Return		PVR_SUCCESS on success
 @Description	Allows textures to be stored in C header files and loaded in. Can load parts of a
 mipmaped texture (ie skipping the highest detailed levels).In OpenGL Cube Map, each
 texture's up direction is defined as next (view direction, up direction),
 (+x,-y)(-x,-y)(+y,+z)(-y,-z)(+z,-y)(-z,-y).
 Sets the texture MIN/MAG filter to GL_LINEAR_MIPMAP_NEAREST/GL_LINEAR
 if mipmaps are present, GL_LINEAR/GL_LINEAR otherwise.
 *****************************************************************************/
/*void PVRTextureLoader::PVRTTextureLoadFromPointer(	const void* pointer,
										   GLuint *const texName,
										   int* width,
										   int* height)
{
	bool bAllowDecompress = true;
	int nLoadFromLevel	  = 0;
	
	PVR_Texture_Header* psPVRHeader = (PVR_Texture_Header*)pointer;
	unsigned int u32NumSurfs;
	
	// perform checks for old PVR psPVRHeader
	if(psPVRHeader->dwHeaderSize!=sizeof(PVR_Texture_Header))
	{	// Header V1
		if(psPVRHeader->dwHeaderSize==PVRTEX_V1_HEADER_SIZE)
		{	// react to old psPVRHeader: i.e. fill in numsurfs as this is missing from old header
			//PVRTErrorOutputDebug("PVRTTextureLoadFromPointer warning: this is an old pvr"
			 //" - you can use PVRTexTool to update its header.\n");
			if(psPVRHeader->dwpfFlags&PVRTEX_CUBEMAP)
				u32NumSurfs = 6;
			else
				u32NumSurfs = 1;
		}
		else
		{	// not a pvr at all
			//PVRTErrorOutputDebug("PVRTTextureLoadFromPointer failed: not a valid pvr.\n");
			//return PVR_FAIL;
		}
	}
	else
	{	// Header V2
		if(psPVRHeader->dwNumSurfs<1)
		{	// encoded with old version of PVRTexTool before zero numsurfs bug found.
			if(psPVRHeader->dwpfFlags & PVRTEX_CUBEMAP)
				u32NumSurfs = 6;
			else
				u32NumSurfs = 1;
		}
		else
		{
			u32NumSurfs = psPVRHeader->dwNumSurfs;
		}
	}
	
	GLuint textureName;
	GLenum eTextureFormat = 0;
	GLenum eTextureInternalFormat = 0;	// often this is the same as textureFormat, but not for BGRA8888 on the iPhone, for instance
	GLenum eTextureType = 0;
	GLenum eTarget;
	
	//bool bIsPVRTCSupported = CPVRTgles2Ext::IsGLExtensionSupported("GL_IMG_texture_compression_pvrtc");
	
#ifdef COMPILE_IPHONE
	bool bIsPVRTCSupported = true;
#endif
	
#ifdef COMPILE_QT
	bool bIsPVRTCSupported = false;
#endif
	
	//#ifndef TARGET_OS_IPHONE
	 //bool bIsBGRA8888Supported  = CPVRTgles2Ext::IsGLExtensionSupported("GL_IMG_texture_format_BGRA8888");
	 //#else
	 //bool bIsBGRA8888Supported  = CPVRTgles2Ext::IsGLExtensionSupported("GL_APPLE_texture_format_BGRA8888");
	 //#endif
	bool bIsBGRA8888Supported = true;
	
	//bool bIsFloat16Supported = CPVRTgles2Ext::IsGLExtensionSupported("GL_OES_texture_half_float");
	bool bIsFloat16Supported = true;
	
	//bool bIsFloat32Supported = CPVRTgles2Ext::IsGLExtensionSupported("GL_OES_texture_float");
	
	bool bIsFloat32Supported = true;
	//#ifndef TARGET_OS_IPHONE
	 //bool bIsETCSupported = CPVRTgles2Ext::IsGLExtensionSupported("GL_OES_compressed_ETC1_RGB8_texture");
	 //#endif
	
	bool bIsETCSupported = true;
	
	*texName = 0;	// install warning value
	bool bIsCompressedFormatSupported = false, bIsCompressedFormat = false;
	
	// Only accept untwiddled data UNLESS texture format is PVRTC 
	if ( ((psPVRHeader->dwpfFlags & PVRTEX_TWIDDLE) == PVRTEX_TWIDDLE)
		&& ((psPVRHeader->dwpfFlags & PVRTEX_PIXELTYPE)!=OGL_PVRTC2)
		&& ((psPVRHeader->dwpfFlags & PVRTEX_PIXELTYPE)!=OGL_PVRTC4) )
	{
		Errors::notifyError("PVRTTextureLoadFromPointer failed: texture should be untwiddled.");
	}
	
	unsigned int ePixelType = psPVRHeader->dwpfFlags & PVRTEX_PIXELTYPE;
	
	switch(ePixelType)
	{
		case OGL_RGBA_4444:
			eTextureFormat = eTextureInternalFormat = GL_RGBA;
			eTextureType = GL_UNSIGNED_SHORT_4_4_4_4;
		break;
		case OGL_RGBA_5551:
			eTextureFormat = eTextureInternalFormat = GL_RGBA ;
			eTextureType = GL_UNSIGNED_SHORT_5_5_5_1;
		break;
		case OGL_RGBA_8888:
			eTextureFormat = eTextureInternalFormat = GL_RGBA;
			eTextureType = GL_UNSIGNED_BYTE ;
		break;
			// New OGL Specific Formats Added 
		case OGL_RGB_565:
			eTextureFormat = eTextureInternalFormat = GL_RGB ;
			eTextureType = GL_UNSIGNED_SHORT_5_6_5;
		break;
		case OGL_RGB_555:
			Errors::notifyError("PVRTTextureLoadFromPointer failed: pixel type OGL_RGB_555 not supported.");
		break;
		case OGL_RGB_888:
			eTextureFormat = eTextureInternalFormat = GL_RGB ;
			eTextureType = GL_UNSIGNED_BYTE;
		break;
		case OGL_I_8:
			eTextureFormat = eTextureInternalFormat = GL_LUMINANCE;
			eTextureType = GL_UNSIGNED_BYTE;
		break;
		case OGL_AI_88:
			eTextureFormat = eTextureInternalFormat = GL_LUMINANCE_ALPHA ;
			eTextureType = GL_UNSIGNED_BYTE;
		break;
		case MGLPT_PVRTC2:
		case OGL_PVRTC2:
			if(bIsPVRTCSupported)
			{
				bIsCompressedFormatSupported = bIsCompressedFormat = true;
				eTextureFormat = psPVRHeader->dwAlphaBitMask==0 ? GL_COMPRESSED_RGB_PVRTC_2BPPV1_IMG : GL_COMPRESSED_RGBA_PVRTC_2BPPV1_IMG ;	// PVRTC2
			}
			else
			{
				if(bAllowDecompress)
				{
					bIsCompressedFormatSupported = false;
					bIsCompressedFormat = true;
					eTextureFormat = eTextureInternalFormat = GL_RGBA;
					eTextureType = GL_UNSIGNED_BYTE;
					//PVRTErrorOutputDebug("PVRTTextureLoadFromPointer warning: PVRTC2 not supported. Converting to RGBA8888 instead.\n");
				}
				else
					Errors::notifyError("PVRTTextureLoadFromPointer error: PVRTC2 not supported.");
			}
			break;
		case MGLPT_PVRTC4:
		case OGL_PVRTC4:
			if(bIsPVRTCSupported)
			{
				bIsCompressedFormatSupported = bIsCompressedFormat = true;
				eTextureFormat = psPVRHeader->dwAlphaBitMask==0 ? GL_COMPRESSED_RGB_PVRTC_4BPPV1_IMG : GL_COMPRESSED_RGBA_PVRTC_4BPPV1_IMG ;	// PVRTC4
			}
			else
			{
				if(bAllowDecompress)
				{
					bIsCompressedFormatSupported = false;
					bIsCompressedFormat = true;
					eTextureFormat = eTextureInternalFormat = GL_RGBA;
					eTextureType = GL_UNSIGNED_BYTE;
					//PVRTErrorOutputDebug("PVRTTextureLoadFromPointer warning: PVRTC4 not supported. Converting to RGBA8888 instead.\n");
				}
				else
					Errors::notifyError("PVRTTextureLoadFromPointer error: PVRTC4 not supported.");
			}
			break;
		case OGL_A_8:
			eTextureFormat = eTextureInternalFormat = GL_ALPHA;
			eTextureType = GL_UNSIGNED_BYTE;
			break;
			
		case OGL_BGRA_8888:
			if(bIsBGRA8888Supported)
			{
				eTextureType = GL_UNSIGNED_BYTE;
#ifndef __APPLE__
				eTextureFormat = eTextureInternalFormat = GL_BGRA;
#else
				eTextureFormat = GL_BGRA;
				eTextureInternalFormat = GL_RGBA;
#endif
			}
			else
				Errors::notifyError("PVRTTextureLoadFromPointer failed: Unable to load GL_BGRA texture as extension GL_IMG_texture_format_BGRA8888 is unsupported.");
			break;
		case D3D_ABGR_16161616F:
			if(bIsFloat16Supported)
			{
				eTextureFormat = eTextureInternalFormat =  GL_RGBA ;
				eTextureType = GL_HALF_FLOAT_OES;
			}
			break;
		case D3D_ABGR_32323232F:
			if(bIsFloat32Supported)
			{
				eTextureFormat = eTextureInternalFormat =  GL_RGBA ;
				eTextureType = GL_FLOAT;
			}
			break;
#ifndef TARGET_OS_IPHONE
		case ETC_RGB_4BPP:
			if(bIsETCSupported)
			{
				bIsCompressedFormatSupported = bIsCompressedFormat = true;
				//eTextureFormat = GL_ETC1_RGB8_OES;
			}
			else
			{
				if(bAllowDecompress)
				{
					bIsCompressedFormatSupported = false;
					bIsCompressedFormat = true;
					eTextureFormat = GL_RGBA;
					eTextureType = GL_UNSIGNED_BYTE;
					//PVRTErrorOutputDebug("PVRTTextureLoadFromPointer warning: ETC not supported. Converting to RGBA8888 instead.\n");
				}
				else
					Errors::notifyError("PVRTTextureLoadFromPointer error: ETC not supported.");
			}
			break;
#endif
			
		default:											// NOT SUPPORTED
			Errors::notifyError("PVRTTextureLoadFromPointer failed: pixel type not supported.");
		break;
	}
	
	// load the texture up
	glPixelStorei(GL_UNPACK_ALIGNMENT,1);				// Never have row-aligned in psPVRHeaders
	
	glGenTextures(1, &textureName);
	
	//  check that this data is cube map data or not.
	eTarget = psPVRHeader->dwpfFlags & PVRTEX_CUBEMAP ? GL_TEXTURE_CUBE_MAP : GL_TEXTURE_2D;
	
	glBindTexture(eTarget, textureName);
	
	if(glGetError())
	   Errors::notifyError("PVRTTextureLoadFromPointer failed: glBindTexture() failed.");
	
	bool sizeSetted = false;
	
	for(unsigned int i=0; i < u32NumSurfs; i++)
	{	
		char *theTexturePtr	   =  (char*)psPVRHeader + psPVRHeader->dwHeaderSize + psPVRHeader->dwTextureDataSize * i;
		char *theTextureToLoad = 0;
		int		nMIPMapLevel;
		int		nTextureLevelsNeeded = (psPVRHeader->dwpfFlags & PVRTEX_MIPMAP)? psPVRHeader->dwMipMapCount : 0;
		unsigned int		nSizeX= psPVRHeader->dwWidth, nSizeY = psPVRHeader->dwHeight;
		unsigned int		CompressedImageSize = 0;
		
		for(nMIPMapLevel = 0; nMIPMapLevel <= nTextureLevelsNeeded; nSizeX=PVRT_MAX(nSizeX/2, (unsigned int)1), nSizeY=PVRT_MAX(nSizeY/2, (unsigned int)1), nMIPMapLevel++)
		{
			theTextureToLoad = theTexturePtr;
			
			if(!sizeSetted)
			{
				*width     = nSizeX;
				*height    = nSizeY;
				sizeSetted = true;
			}
			
			// Load the Texture
			// If the texture is PVRTC or ETC then use GLCompressedTexImage2D
			if(bIsCompressedFormat)
			{
				// Calculate how many bytes this MIP level occupies 
				if ((psPVRHeader->dwpfFlags & PVRTEX_PIXELTYPE)==OGL_PVRTC2)
					CompressedImageSize = ( PVRT_MAX(nSizeX, PVRTC2_MIN_TEXWIDTH) * PVRT_MAX(nSizeY, PVRTC2_MIN_TEXHEIGHT) * psPVRHeader->dwBitCount) / 8;
				else if ((psPVRHeader->dwpfFlags & PVRTEX_PIXELTYPE)==OGL_PVRTC4)
					CompressedImageSize = ( PVRT_MAX(nSizeX, PVRTC4_MIN_TEXWIDTH) * PVRT_MAX(nSizeY, PVRTC4_MIN_TEXHEIGHT) * psPVRHeader->dwBitCount) / 8;
				else// ETC
					CompressedImageSize = ( PVRT_MAX(nSizeX, ETC_MIN_TEXWIDTH) * PVRT_MAX(nSizeY, ETC_MIN_TEXHEIGHT) * psPVRHeader->dwBitCount) / 8;
				
				if(((signed int)nMIPMapLevel - (signed int)nLoadFromLevel) >= 0)
				{
					if(bIsCompressedFormatSupported)
					{
						if(psPVRHeader->dwpfFlags & PVRTEX_CUBEMAP)
						{
							// Load compressed texture data at selected MIP level 
							glCompressedTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X+i, nMIPMapLevel-nLoadFromLevel, eTextureFormat, nSizeX, nSizeY, 0,
												   CompressedImageSize, theTextureToLoad);
						}
						else
						{
							// Load compressed texture data at selected MIP level 
							glCompressedTexImage2D(GL_TEXTURE_2D, nMIPMapLevel-nLoadFromLevel, eTextureFormat, nSizeX, nSizeY, 0,
												   CompressedImageSize, theTextureToLoad);
							
						}
					}
					else
					{
						// Convert PVRTC to 32-bit
						PVRTuint8 *u8TempTexture = (PVRTuint8*)malloc(nSizeX*nSizeY*4);
						if ((psPVRHeader->dwpfFlags & PVRTEX_PIXELTYPE)==OGL_PVRTC2)
							PVRTDecompressPVRTC(theTextureToLoad, 1, nSizeX, nSizeY, u8TempTexture);
						else if((psPVRHeader->dwpfFlags & PVRTEX_PIXELTYPE)==OGL_PVRTC4)
							PVRTDecompressPVRTC(theTextureToLoad, 0, nSizeX, nSizeY, u8TempTexture);
						else
						{	
							Errors::notifyError("ETC decompresion is not implemented.");
							// ETC
							//PVRTDecompressETC(theTextureToLoad, nSizeX, nSizeY, u8TempTexture, 0);
						}
						
						
						if(psPVRHeader->dwpfFlags&PVRTEX_CUBEMAP)
						{// Load compressed cubemap data at selected MIP level
							// Upload the texture as 32-bits
							glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X+i,nMIPMapLevel-nLoadFromLevel,GL_RGBA,
										 nSizeX,nSizeY,0, GL_RGBA,GL_UNSIGNED_BYTE,u8TempTexture);
							FREE(u8TempTexture);
						}
						else
						{// Load compressed 2D data at selected MIP level
							// Upload the texture as 32-bits
							glTexImage2D(GL_TEXTURE_2D,nMIPMapLevel-nLoadFromLevel,GL_RGBA,
										 nSizeX,nSizeY,0, GL_RGBA,GL_UNSIGNED_BYTE,u8TempTexture);
							FREE(u8TempTexture);
						}
					}
				}
			}
			else
			{
				if(((signed int)nMIPMapLevel - (signed int)nLoadFromLevel) >= 0)
				{
					if(psPVRHeader->dwpfFlags&PVRTEX_CUBEMAP)
					{
						// Load uncompressed texture data at selected MIP level 
						glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X+i,nMIPMapLevel-nLoadFromLevel,eTextureInternalFormat,nSizeX,nSizeY,
									 0, eTextureFormat,eTextureType,theTextureToLoad);
					}
					else
					{
						// Load uncompressed texture data at selected MIP level 
						glTexImage2D(GL_TEXTURE_2D,nMIPMapLevel-nLoadFromLevel,eTextureInternalFormat,nSizeX,nSizeY,0,eTextureFormat,eTextureType,theTextureToLoad);
					}
				}
			}
			
			if(glGetError())
				Errors::notifyError("PVRTTextureLoadFromPointer failed: glTexImage2D() failed.");
			
			// offset the texture pointer by one mip-map level
			
			// PVRTC case 
			if ( bIsCompressedFormat )
			{
				theTexturePtr += CompressedImageSize;
			}
			else
			{
				// New formula that takes into account bit counts inferior to 8 (e.g. 1 bpp) 
				theTexturePtr += (nSizeX * nSizeY * psPVRHeader->dwBitCount + 7) / 8;
			}
		}
	}
	
	*texName = textureName;
	
	if(eTextureType==GL_FLOAT || eTextureType==GL_HALF_FLOAT_OES)
	{
		if(!psPVRHeader->dwMipMapCount)
		{	// Texture filter modes are limited to these for float textures
			glTexParameteri(eTarget, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
			glTexParameteri(eTarget, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		}
		else
		{
			glTexParameteri(eTarget, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
			glTexParameteri(eTarget, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		}
	}
	else
	{
		if(!psPVRHeader->dwMipMapCount)
		{
			glTexParameteri(eTarget, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(eTarget, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		}
		else
		{
			glTexParameteri(eTarget, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
			glTexParameteri(eTarget, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		}
	}
	
	if((psPVRHeader->dwWidth & (psPVRHeader->dwWidth - 1)) |
	   (psPVRHeader->dwHeight & (psPVRHeader->dwHeight - 1)))
	{
		 //NPOT textures requires the wrap mode to be set explicitly to
		 //GL_CLAMP_TO_EDGE or the texture will be inconsistent.
		 
		glTexParameteri(eTarget, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(eTarget, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	}
	else
	{
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	}
}*/

/*!***********************************************************************
 @Function		PVRTDecompressPVRTC
 @Input			pCompressedData The PVRTC texture data to decompress
 @Input			Do2bitMode Signifies whether the data is PVRTC2 or PVRTC4
 @Input			XDim X dimension of the texture
 @Input			YDim Y dimension of the texture
 @Modified		pResultImage The decompressed texture data
 @Description	Decompresses PVRTC to RGBA 8888
 *************************************************************************/

void PVRTextureLoader::PVRTDecompressPVRTC(const void *pCompressedData,
									const int Do2bitMode,
									const int XDim,
									const int YDim,
									unsigned char* pResultImage)

{
	Decompress((AMTC_BLOCK_STRUCT*)pCompressedData, Do2bitMode, XDim, YDim, 1, pResultImage);
}


#define BLK_X_2BPP	(8) // dimensions for the two formats
#define BLK_X_4BPP	(4)
#define BLK_Y_SIZE 	(4) // always 4 for all 2D block types
#define WRAP_COORD(Val, Size) ((Val) & ((Size)-1))

#define LIMIT_COORD(Val, Size, AssumeImageTiles) \
((AssumeImageTiles)? WRAP_COORD((Val), (Size)): PVRT_CLAMP((Val), 0, (Size)-1))

#define POWER_OF_2(X)   util_number_is_power_2(X)

static int DisableTwiddlingRoutine = 0;

/*!***********************************************************************
 @Function		util_number_is_power_2
 @Input		input A number
 @Returns		TRUE if the number is an integer power of two, else FALSE.
 @Description	Check that a number is an integer power of two, i.e.
 1, 2, 4, 8, ... etc.
 Returns FALSE for zero.
 *************************************************************************/

int util_number_is_power_2( unsigned  input )

{
	unsigned minus1;
	
	if( !input ) return 0;

	minus1 = input - 1;
	
	return ( (input | minus1) == (input ^ minus1) ) ? 1 : 0;
}

static U32 TwiddleUV(U32 YSize, U32 XSize, U32 YPos, U32 XPos)

{
	U32 Twiddled;
	U32 MinDimension;
	U32 MaxValue;
	U32 SrcBitPos;
	U32 DstBitPos;
	int ShiftCount;
	
	assert(YPos < YSize);
	assert(XPos < XSize);
	assert(POWER_OF_2(YSize));
	assert(POWER_OF_2(XSize));
	
	if(YSize < XSize)
	{
		MinDimension = YSize;
		MaxValue	 = XPos;
	}
	else
	{
		MinDimension = XSize;
		MaxValue	 = YPos;
	}
	
	// Nasty hack to disable twiddling
	if(DisableTwiddlingRoutine)
		return (YPos* XSize + XPos);
	
	// Step through all the bits in the "minimum" dimension
	SrcBitPos = 1;
	DstBitPos = 1;
	Twiddled  = 0;
	ShiftCount = 0;
	
	while(SrcBitPos < MinDimension)
	{
		if(YPos & SrcBitPos)
			Twiddled |= DstBitPos;
		
		if(XPos & SrcBitPos)
			Twiddled |= (DstBitPos << 1);
		
		SrcBitPos <<= 1;
		DstBitPos <<= 2;
		ShiftCount += 1;
	}
	
	// prepend any unused bits
	MaxValue >>= ShiftCount;
	Twiddled |=  (MaxValue << (2*ShiftCount));
	
	return Twiddled;
	
}

/*!***********************************************************************
 @Function		Unpack5554Colour
 @Input			pBlock
 @Input			ABColours
 @Description	Given a block, extract the colour information and convert
 to 5554 formats
 *************************************************************************/

static void Unpack5554Colour(const AMTC_BLOCK_STRUCT *pBlock,int   ABColours[2][4])
{
	U32 RawBits[2];
	int i;
	// Extract A and B
	RawBits[0] = pBlock->PackedData[1] & (0xFFFE); // 15 bits (shifted up by one)
	RawBits[1] = pBlock->PackedData[1] >> 16;	   // 16 bits
	
	// step through both colour
	for(i = 0; i < 2; i++)
	{
		// If completely opaque
		if(RawBits[i] & (1<<15))
		{
			// Extract R and G (both 5 bit)
			ABColours[i][0] = (RawBits[i] >> 10) & 0x1F;
			ABColours[i][1] = (RawBits[i] >>  5) & 0x1F;
			/*
			 The precision of Blue depends on  A or B. If A then we need to
			 replicate the top bit to get 5 bits in total
			 */
			ABColours[i][2] = RawBits[i] & 0x1F;
	
			if(i==0)
				ABColours[0][2] |= ABColours[0][2] >> 4;
			
			// set 4bit alpha fully on...
			ABColours[i][3] = 0xF;
		}
		else // Else if colour has variable translucency
		{
			/*
			 Extract R and G (both 4 bit).
			 (Leave a space on the end for the replication of bits
			 */
			ABColours[i][0] = (RawBits[i] >>  (8-1)) & 0x1E;
			ABColours[i][1] = (RawBits[i] >>  (4-1)) & 0x1E;
			
			// replicate bits to truly expand to 5 bits
			ABColours[i][0] |= ABColours[i][0] >> 4;
			ABColours[i][1] |= ABColours[i][1] >> 4;
			
			// grab the 3(+padding) or 4 bits of blue and add an extra padding bit
			ABColours[i][2] = (RawBits[i] & 0xF) << 1;
			/*
			 expand from 3 to 5 bits if this is from colour A, or 4 to 5 bits if from
			 colour B
			 */
			
			if(i==0)
				ABColours[0][2] |= ABColours[0][2] >> 3;
			else
				ABColours[0][2] |= ABColours[0][2] >> 4;
			
			// Set the alpha bits to be 3 + a zero on the end
			ABColours[i][3] = (RawBits[i] >> 11) & 0xE;
		}
	}
}


/*!***********************************************************************
 @Function		UnpackModulations
 @Input			pBlock
 @Input			Do2bitMode
 @Input			ModulationVals
 @Input			ModulationModes
 @Input			StartX
 @Input			StartY
 @Description	Given the block and the texture type and it's relative
 position in the 2x2 group of blocks, extract the bit
 patterns for the fully defined pixels.
 *************************************************************************/

static void	UnpackModulations(const AMTC_BLOCK_STRUCT *pBlock,
							  const int Do2bitMode,
							  int ModulationVals[8][16],
							  int ModulationModes[8][16],
							  int StartX,
							  int StartY)
{
	int BlockModMode;
	U32 ModulationBits;
	int x, y;
	
	BlockModMode   = pBlock->PackedData[1] & 1;
	ModulationBits = pBlock->PackedData[0];
	
	// if it's in an interpolated mode
	if(Do2bitMode && BlockModMode)
	{
		/*
		 run through all the pixels in the block. Note we can now treat all the
		 "stored" values as if they have 2bits (even when they didn't!)
		 */
		
		for(y = 0; y < BLK_Y_SIZE; y++)
		{
			for(x = 0; x < BLK_X_2BPP; x++)
			{
				ModulationModes[y+StartY][x+StartX] = BlockModMode;
				// if this is a stored value...
				if(((x^y)&1) == 0)
				{
					ModulationVals[y+StartY][x+StartX] = ModulationBits & 3;
					ModulationBits >>= 2;
				}
			}
		}
	}
	else if(Do2bitMode) // else if direct encoded 2bit mode - i.e. 1 mode bit per pixel
	{
		for(y = 0; y < BLK_Y_SIZE; y++)
		{
			for(x = 0; x < BLK_X_2BPP; x++)
			{
				ModulationModes[y+StartY][x+StartX] = BlockModMode;
				// double the bits so 0=> 00, and 1=>11
				
				if(ModulationBits & 1)
					ModulationVals[y+StartY][x+StartX] = 0x3;
				else
					ModulationVals[y+StartY][x+StartX] = 0x0;
				
				ModulationBits >>= 1;
			}
		}
	}
	else // else its the 4bpp mode so each value has 2 bits
	{
		for(y = 0; y < BLK_Y_SIZE; y++)
		{
			for(x = 0; x < BLK_X_4BPP; x++)
			{
				ModulationModes[y+StartY][x+StartX] = BlockModMode;
				ModulationVals[y+StartY][x+StartX]  = ModulationBits & 3;
				ModulationBits >>= 2;
			}
		}
	}
	// make sure nothing is left over
	
	assert(ModulationBits==0);
}

/*!***********************************************************************
 @Function		InterpolateColours
 @Input			ColourP
 @Input			ColourQ
 @Input			ColourR
 @Input			ColourS
 @Input			Do2bitMode
 @Input			x
 @Input			y
 @Modified		Result
 @Description	This performs a HW bit accurate interpolation of either the 
 A or B colours for a particular pixel.
 NOTE: It is assumed that the source colours are in ARGB 5554
 format - This means that some "preparation" of the values will
 be necessary.
*************************************************************************/

static void InterpolateColours(const int ColourP[4],
							   const int ColourQ[4],
							   const int ColourR[4],
							   const int ColourS[4],
							   const int Do2bitMode,
							   const int x,
							   const int y,
							   int Result[4])
{
	int u, v, uscale;
	int k;
	int tmp1, tmp2;
	int P[4], Q[4], R[4], S[4];
	
	// Copy the colours
	for(k = 0; k < 4; k++)
	{
		P[k] = ColourP[k];
		Q[k] = ColourQ[k];
		R[k] = ColourR[k];
		S[k] = ColourS[k];
	}
	
	// put the x and y values into the right range
	v = (y & 0x3) | ((~y & 0x2) << 1);

	if(Do2bitMode)
		u = (x & 0x7) | ((~x & 0x4) << 1);
	else
		u = (x & 0x3) | ((~x & 0x2) << 1);
	
	// get the u and v scale amounts
	v  = v - BLK_Y_SIZE/2;
	
	if(Do2bitMode)
	{
		u = u - BLK_X_2BPP/2;
		uscale = 8;
	}
	else
	{
		u = u - BLK_X_4BPP/2;
		uscale = 4;
	}
	
	for(k = 0; k < 4; k++)
	{
		tmp1 = P[k] * uscale + u * (Q[k] - P[k]);
		tmp2 = R[k] * uscale + u * (S[k] - R[k]);
		tmp1 = tmp1 * 4 + v * (tmp2 - tmp1);
		Result[k] = tmp1;
	}
	
	// Lop off the appropriate number of bits to get us to 8 bit precision
	if(Do2bitMode)
	{
		// do RGB
		for(k = 0; k < 3; k++)
			Result[k] >>= 2;
	
		Result[3] >>= 1;
	}
	else
	{
		// do RGB  (A is ok)
		for(k = 0; k < 3; k++)
			Result[k] >>= 1;
	}

	// sanity check
	for(k = 0; k < 4; k++)
		assert(Result[k] < 256);
	
	/*
	 Convert from 5554 to 8888
	 do RGB 5.3 => 8
	 */
	
	for(k = 0; k < 3; k++)
		Result[k] += Result[k] >> 5;
	
	Result[3] += Result[3] >> 4;
	
	// 2nd sanity check
	for(k = 0; k < 4; k++)
		assert(Result[k] < 256);
}

#define PT_INDEX (2)	// The Punch-through index


/*!***********************************************************************
 @Function		GetModulationValue
 @Input			x
 @Input			y
 @Input			Do2bitMode
 @Input			ModulationVals
 @Input			ModulationModes
 @Input			Mod
 @Input			DoPT
 @Description	Get the modulation value as a numerator of a fraction of 8ths
 *************************************************************************/

static void GetModulationValue(int x,
							   int y,
							   const int Do2bitMode,
							   const int ModulationVals[8][16],
							   const int ModulationModes[8][16],
							   int *Mod,
							   int *DoPT)
{
	static const int RepVals0[4] = {0, 3, 5, 8};
	static const int RepVals1[4] = {0, 4, 4, 8};
	
	int ModVal;
	// Map X and Y into the local 2x2 block
	y = (y & 0x3) | ((~y & 0x2) << 1);
	
	if(Do2bitMode)
		x = (x & 0x7) | ((~x & 0x4) << 1);
	else
		x = (x & 0x3) | ((~x & 0x2) << 1);

	// assume no PT for now
	*DoPT = 0;
	

	// extract the modulation value. If a simple encoding
	if(ModulationModes[y][x]==0)
		ModVal = RepVals0[ModulationVals[y][x]];
	else if(Do2bitMode)
	{
		// if this is a stored value
		if(((x^y)&1)==0)
			ModVal = RepVals0[ModulationVals[y][x]];
		else if(ModulationModes[y][x] == 1) // else average from the neighbours if H&V interpolation..
		{
			ModVal = (RepVals0[ModulationVals[y-1][x]] +
					  RepVals0[ModulationVals[y+1][x]] +
					  RepVals0[ModulationVals[y][x-1]] +
					  RepVals0[ModulationVals[y][x+1]] + 2) / 4;
		}
		else if(ModulationModes[y][x] == 2) // else if H-Only
		{
			ModVal = (RepVals0[ModulationVals[y][x-1]] +
					  RepVals0[ModulationVals[y][x+1]] + 1) / 2;
		}
		else // else it's V-Only
		{
			ModVal = (RepVals0[ModulationVals[y-1][x]] +
					  RepVals0[ModulationVals[y+1][x]] + 1) / 2;
		}
	}
	else // else it's 4BPP and PT encoding
	{
		ModVal = RepVals1[ModulationVals[y][x]];
		*DoPT  = ModulationVals[y][x] == PT_INDEX;
	}
	
	*Mod =ModVal;
}

/*!***********************************************************************
 @Function		Decompress
 @Input			pCompressedData The PVRTC texture data to decompress
 @Input			Do2BitMode Signifies whether the data is PVRTC2 or PVRTC4
 @Input			XDim X dimension of the texture
 @Input			YDim Y dimension of the texture
 @Input			AssumeImageTiles Assume the texture data tiles
 @Modified		pResultImage The decompressed texture data
 @Description	Decompresses PVRTC to RGBA 8888
 *************************************************************************/
void PVRTextureLoader::Decompress(AMTC_BLOCK_STRUCT *pCompressedData,
						   const int Do2bitMode,
						   const int XDim,
						   const int YDim,
						   const int AssumeImageTiles,
						   unsigned char* pResultImage)
{
	int x, y;
	int i, j;
	int BlkX, BlkY;
	int BlkXp1, BlkYp1;
	int XBlockSize;
	int BlkXDim, BlkYDim;
	int StartX, StartY;
	int ModulationVals[8][16];
	int ModulationModes[8][16];
	int Mod, DoPT;
	unsigned int uPosition;
	// local neighbourhood of blocks
	AMTC_BLOCK_STRUCT *pBlocks[2][2];
	AMTC_BLOCK_STRUCT *pPrevious[2][2] = {{NULL, NULL}, {NULL, NULL}};

	// Low precision colours extracted from the blocks
	struct
	{
		int Reps[2][4];
	}Colours5554[2][2];
	
	// Interpolated A and B colours for the pixel
	int ASig[4], BSig[4];
	int Result[4];
	
	if(Do2bitMode)
		XBlockSize = BLK_X_2BPP;
	else
		XBlockSize = BLK_X_4BPP;
	
	// For MBX don't allow the sizes to get too small
	BlkXDim = PVRT_MAX(2, XDim / XBlockSize);
	BlkYDim = PVRT_MAX(2, YDim / BLK_Y_SIZE);
	
	/*
	 Step through the pixels of the image decompressing each one in turn
	 Note that this is a hideously inefficient way to do this!
	 */
	
	for(y = 0; y < YDim; y++)
	{
		for(x = 0; x < XDim; x++)
		{
			// map this pixel to the top left neighbourhood of blocks
			BlkX = (x - XBlockSize/2);
			BlkY = (y - BLK_Y_SIZE/2);
			BlkX = LIMIT_COORD(BlkX, XDim, AssumeImageTiles);
			BlkY = LIMIT_COORD(BlkY, YDim, AssumeImageTiles);
			BlkX /= XBlockSize;
			BlkY /= BLK_Y_SIZE;
			// compute the positions of the other 3 blocks
			BlkXp1 = LIMIT_COORD(BlkX+1, BlkXDim, AssumeImageTiles);
			BlkYp1 = LIMIT_COORD(BlkY+1, BlkYDim, AssumeImageTiles);
			// Map to block memory locations
			pBlocks[0][0] = pCompressedData +TwiddleUV(BlkYDim, BlkXDim, BlkY, BlkX);
			pBlocks[0][1] = pCompressedData +TwiddleUV(BlkYDim, BlkXDim, BlkY, BlkXp1);
			pBlocks[1][0] = pCompressedData +TwiddleUV(BlkYDim, BlkXDim, BlkYp1, BlkX);
			pBlocks[1][1] = pCompressedData +TwiddleUV(BlkYDim, BlkXDim, BlkYp1, BlkXp1);
			/*
			 extract the colours and the modulation information IF the previous values
			 have changed.
			 */
			if(memcmp(pPrevious, pBlocks, 4*sizeof(void*)) != 0)
			{
				StartY = 0;
				for(i = 0; i < 2; i++)
				{
					StartX = 0;
					for(j = 0; j < 2; j++)
					{
						Unpack5554Colour(pBlocks[i][j], Colours5554[i][j].Reps);
						UnpackModulations(pBlocks[i][j],
										  Do2bitMode,
										  ModulationVals,
										  ModulationModes,
										  StartX, StartY);
						StartX += XBlockSize;
					}
					StartY += BLK_Y_SIZE;
				}

				// make a copy of the new pointers
				memcpy(pPrevious, pBlocks, 4*sizeof(void*));
			}

			// decompress the pixel.  First compute the interpolated A and B signals
			InterpolateColours(Colours5554[0][0].Reps[0],	   
							   Colours5554[0][1].Reps[0],
							   Colours5554[1][0].Reps[0],
							   Colours5554[1][1].Reps[0],
							   Do2bitMode, x, y,
							   ASig);
			
			InterpolateColours(Colours5554[0][0].Reps[1],
							   Colours5554[0][1].Reps[1],
							   Colours5554[1][0].Reps[1],
							   Colours5554[1][1].Reps[1],
							   Do2bitMode, x, y,
							   BSig);
			
			GetModulationValue(x,y, Do2bitMode, (const int (*)[16])ModulationVals, (const int (*)[16])ModulationModes, &Mod, &DoPT);
			
			// compute the modulated colour
			for(i = 0; i < 4; i++)
			{
				Result[i] = ASig[i] * 8 + Mod * (BSig[i] - ASig[i]);
				Result[i] >>= 3;
			}
			
			if(DoPT)
				Result[3] = 0;
			
			// Store the result in the output image
			uPosition = (x+y*XDim)<<2;
			pResultImage[uPosition+0] = (U8)Result[0];
			pResultImage[uPosition+1] = (U8)Result[1];
			pResultImage[uPosition+2] = (U8)Result[2];
			pResultImage[uPosition+3] = (U8)Result[3];
		}
	}
}

