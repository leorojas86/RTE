#ifndef GL_TEXTURE_H
#define GL_TEXTURE_H

#include "Transformations.h"
#include "GLDriver.h"
#include <string>

class GLTexture
{
public:

    GLTexture();
    GLTexture(std::string path);
    ~GLTexture();

	std::string texturePath();
	void texturePath(std::string value);
    
    virtual GLTextureData* textureData();
	
private:
	
    void initializeVariables();
    void releaseTextureData();
	
    GLTextureData* _textureData;
	std::string _textureResourcePath;

	static bool    _isInitialized;
	static bool    _isEnabled;
};

#endif // GLTEXTURE_H
