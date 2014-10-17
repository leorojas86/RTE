#include "GLtexture.h"
#include "LogUtils.h"
#include "FileUtils.h"
#include "PVRTextureLoader.h"
#include "Resources.h"

bool   GLTexture::_isInitialized       = false;
bool   GLTexture::_isEnabled           = false;

GLTexture::GLTexture()
{
    initializeVariables();
}

GLTexture::GLTexture(std::string path)
{
    initializeVariables();
    this->texturePath(path);
}

void GLTexture::initializeVariables()
{
    _textureData = NULL;
}

void GLTexture::releaseTextureData()
{
    if(_textureData != NULL)
    {
        GLDriver::unloadTexture(_textureData);
        _textureData = NULL;
    }
}

GLTexture::~GLTexture()
{
    releaseTextureData();
}

std::string GLTexture::texturePath() { return _textureResourcePath; }
void GLTexture::texturePath(std::string value)
{
    if(_textureResourcePath != value)
    {
        _textureResourcePath = value;
        std::string fullPath = Resources::getFullPath(value);

        if(FileUtils::exist(fullPath))
        {
            releaseTextureData();
            _textureData = GLDriver::loadTextureData(fullPath, GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        }
        else
            LogUtils::notifyError("File = '" + fullPath + "' doesn't exist");
    }
}

GLTextureData* GLTexture::textureData() { return _textureData; }






















