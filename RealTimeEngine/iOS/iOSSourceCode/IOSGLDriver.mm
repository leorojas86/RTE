//
//  IGLDriver.cpp
//  PVRTextureLoader
//
//  Created by Leonardo on 9/22/13.
//
//

#include "GLDriver.h"
#include "LogUtils.h"
#include "FileUtils.h"
#include <string>
#include <OpenGLES/ES1/gl.h>
#include <OpenGLES/ES2/glext.h>
#include <GLKit/GLKMath.h>
#include <GLKit/GLKMatrix4.h>
#include "PVRTextureLoader.h"
#include "StringUtils.h"
#include "RTDefines.h"
#include "Resources.h"
#include "PNGLoader.h"

#ifdef CHECK_GL_ERRORS

void CheckGLErrors()
{
    GLenum glError = glGetError();
	
    if(glError != GL_NO_ERROR)
		LogUtils::notifyError("GL Error = " + StringUtils::stringFromInt(glError) + " \n");//http://www.opengl.org/wiki/OpenGL_Error
}

#endif

//TEXTURE

GLuint generateTexture(int type, int filter, int filterParameter)
{
    GLuint textureId = NONE_TEXTURE;
    //glGenTextures(1, &textureId);
    glGenTextures(1, &textureId);
    //glBindTexture(textureType, textureId);
    glBindTexture(type, textureId);
    //glTexParameteri(textureType, filterType, filterParameter);
    glTexParameteri(type, filter, filterParameter);
    
    #ifdef CHECK_GL_ERRORS
        CheckGLErrors();
    #endif
    
    return textureId;
}

GLuint generateTexture2DWithBytes(void* textureBytes, Size3D size, int colorFormat, int type, int filter, int filterParameter)
{
    GLuint textureId = generateTexture(type, filter, filterParameter);
    //glTexImage2D(textureType, 0, textureColorFormat, size.w(), size.h(), 0, textureColorFormat, GL_UNSIGNED_BYTE, textureBytes);
    glTexImage2D(type, 0, colorFormat, size.w, size.h, 0, colorFormat, GL_UNSIGNED_BYTE, textureBytes);
    
    #ifdef CHECK_GL_ERRORS
        CheckGLErrors();
    #endif
    
    return textureId;
}

GLTextureData* GLDriver::loadTextureData(std::string path, int type, int filter, int filterParameter)
{
    std::string extension = FileUtils::getExtension(path);
    
    if(extension == "pvr")
    {
        PVRTextureLoader textureLoader  = PVRTextureLoader(path);
        std::vector<PVRData> imageData	= textureLoader.imageData();
        Size3D size                     = Size3D(textureLoader.header()->dwWidth,  textureLoader.header()->dwHeight);
        filterParameter                 = imageData.size() > 1 ? GL_LINEAR_MIPMAP_LINEAR : filterParameter;
        
        // load the texture up
        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);// Never have row-aligned in psPVRHeaders
        GLuint textureId = generateTexture(type, filter, filterParameter);
        
        for(std::vector<PVRData>::iterator it = imageData.begin(); it != imageData.end(); it++)
        {
            PVRData data = *it;
            
            if(data.compressed)
                glCompressedTexImage2D(data.target, data.level, data.internalformat, data.width, data.height, 0, data.length, data.data);
            else
                glTexImage2D(data.target, data.level, data.internalformat, data.width, data.height, 0, data.format , data.type, data.data);
            
            #ifdef CHECK_GL_ERRORS
                CheckGLErrors();
            #endif
        }
        
        glTexParameteri(textureLoader.filter1().textureType, textureLoader.filter1().type, textureLoader.filter1().parameter);
        glTexParameteri(textureLoader.filter2().textureType, textureLoader.filter2().type, textureLoader.filter2().parameter);
        glTexParameteri(textureLoader.filter3().textureType, textureLoader.filter3().type, textureLoader.filter3().parameter);
        glTexParameteri(textureLoader.filter4().textureType, textureLoader.filter4().type, textureLoader.filter4().parameter);
        
        #ifdef CHECK_GL_ERRORS
            CheckGLErrors();
        #endif
        
        return new GLTextureData(textureId, size, type, imageData[0].internalformat, filter, filterParameter, false);
    }
    else if(extension == "png")
    {
        PNGLoader pngLoader  = PNGLoader(path);
        GLint internalformat = pngLoader.hasAlphaChannel ? GL_RGBA : GL_RGB;
        Size3D imageSize     = Size3D(pngLoader.width, pngLoader.height);
        GLuint textureId     = generateTexture2DWithBytes(pngLoader.data, imageSize, internalformat, type, filter, filterParameter);
        
        return new GLTextureData(textureId, imageSize, type, internalformat, filter, filterParameter, true);
    }
    else
    {
        NSString* nsPath           = [NSString stringWithUTF8String:path.c_str()];
        UIImage *uiImage           = [UIImage imageWithContentsOfFile:nsPath];
        CGImageRef cgImage         = [uiImage CGImage];
        Size3D imageSize           = Size3D(CGImageGetWidth(cgImage), CGImageGetHeight(cgImage));
        CGColorSpaceRef colorSpace = CGColorSpaceCreateDeviceRGB();
        
        void* imageData        = malloc(imageSize.w * imageSize.h * 4);
        CGContextRef cgContext = CGBitmapContextCreate(imageData, imageSize.w, imageSize.h, 8, imageSize.w * 4, colorSpace, kCGImageAlphaNoneSkipLast | kCGBitmapByteOrder32Big);
        
        if(cgContext != NULL)
        {
            CGContextDrawImage(cgContext, CGRectMake(0.0f, 0.0f, imageSize.w, imageSize.h), cgImage);
            CGContextRelease(cgContext);
        }
        
        CGColorSpaceRelease(colorSpace);
        
        GLint colorFormat = GL_RGBA;
        GLuint textureId  = generateTexture2DWithBytes(imageData, imageSize, colorFormat, type, filter, filterParameter);
        
        free(imageData);
        
        #ifdef CHECK_GL_ERRORS
            CheckGLErrors();
        #endif
        
        return new GLTextureData(textureId, imageSize, type, colorFormat, filter, filterParameter, false);
    }
}

void GLDriver::unloadTexture(GLTextureData* textureData)
{
    glDeleteTextures(1, &textureData->identifier);
}

//TRANSFORMATIONS
void GLDriver::pushMatrix()
{
     glPushMatrix();
    
    #ifdef CHECK_GL_ERRORS
        CheckGLErrors();
    #endif
}

void GLDriver::applyTransformations(Transformations& transformations)
{
    if(!transformations.translation().isEmpty())
        glTranslatef(transformations.translation().x, transformations.translation().y, transformations.translation().z);
    
    if(!transformations.rotation().isEmpty())
        glRotatef(transformations.rotation().degrees(), 0, 0, 1);
    
    if(transformations.scale().w != 1 || transformations.scale().h != 1 || transformations.scale().d != 1)
        glScalef(transformations.scale().w, transformations.scale().h, transformations.scale().d);
    
    if(!transformations.color().isFull())
        glColor4f(transformations.color().r(), transformations.color().g(), transformations.color().b(), transformations.color().a());
    
    #ifdef CHECK_GL_ERRORS
        CheckGLErrors();
    #endif
}

void GLDriver::color4f(float r, float g, float b, float a)
{
    glColor4f(r, g, b, a);
    
    #ifdef CHECK_GL_ERRORS
        CheckGLErrors();
    #endif
}

void GLDriver::getTransformationsState(GLTransformationsState* transformationsState)
{
    glGetFloatv(GL_MODELVIEW_MATRIX, transformationsState->modelViewMatrix);
    glGetFloatv(GL_PROJECTION_MATRIX, transformationsState->projMatrix);
    glGetIntegerv(GL_VIEWPORT, transformationsState->viewport);
    
    #ifdef CHECK_GL_ERRORS
        CheckGLErrors();
    #endif
}

Point3D GLDriver::screenPositionToWorldPosition(Point3D& position, GLTransformationsState* transformationsState)
{
    GLKMatrix4 modelViewMatrix4 = GLKMatrix4MakeWithArray(transformationsState->modelViewMatrix);
    GLKMatrix4 projMatrix4      = GLKMatrix4MakeWithArray(transformationsState->projMatrix);
    
    GLKVector3 vector3       = GLKVector3Make(position.x, position.y, position.z);
    bool success             = false;
    GLKVector3 worldPosition = GLKMathUnproject(vector3, modelViewMatrix4, projMatrix4, transformationsState->viewport, &success);
    
    if(!success)
        LogUtils::notifyWarning(std::string("Errors occurred unprojecting point"));
    
    return Point3D(worldPosition.x, worldPosition.y, worldPosition.z);
    
#ifdef CHECK_GL_ERRORS
    CheckGLErrors();
#endif
}

void GLDriver::popMatrix()
{
    glPopMatrix();
    
    #ifdef CHECK_GL_ERRORS
        CheckGLErrors();
    #endif
}

//CAMERA
void GLDriver::clearBufferColor(Color& color)
{
    glClearColor(color.r(), color.g(), color.b(), color.a());
    glClear(GL_COLOR_BUFFER_BIT);
    
    #ifdef CHECK_GL_ERRORS
        CheckGLErrors();
    #endif
}

void GLDriver::setUpOrthographicProjection(Size3D& screenSize)
{
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
#ifdef CHECK_GL_ERRORS
    CheckGLErrors();
#endif
    
    glMatrixMode(GL_PROJECTION);
    
#ifdef CHECK_GL_ERRORS
    CheckGLErrors();
#endif
    
    glLoadIdentity();
    
#ifdef CHECK_GL_ERRORS
    CheckGLErrors();
#endif
    
    glViewport(0, 0, screenSize.w, screenSize.h);
    glOrthof(-screenSize.w / 2, screenSize.w / 2, -screenSize.h / 2, screenSize.h / 2, -1000 , 1000);
    
#ifdef CHECK_GL_ERRORS
    CheckGLErrors();
#endif
    
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    
    #ifdef CHECK_GL_ERRORS
        CheckGLErrors();
    #endif
}


//RENDERING
void GLDriver::renderTexture2DTriangleStrip(unsigned int textureId, float* vertices, float* textureCoordinates, int pointsNumber, int coordinatesNumber)
{
    //Enable states
    glEnable(GL_TEXTURE_2D);
    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);
    glBindTexture(GL_TEXTURE_2D, textureId);
    
    //Set vertex and texture coordinates
    glVertexPointer(coordinatesNumber, GL_FLOAT, 0, vertices);
    glTexCoordPointer(coordinatesNumber, GL_FLOAT, 0, textureCoordinates);
    
    //Set filters
    //GL_NEAREST, GL_LINEAR, GL_LINEAR_MIPMAP_LINEAR, GL_NEAREST_MIPMAP_NEAREST
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    
    //GL_NEAREST, GL_LINEAR
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	
    //if (_anisotropySupported)
    //glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, 1.0f);
	
    //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    
    //Draw triangles
    glDrawArrays(GL_TRIANGLE_STRIP, 0, pointsNumber);
}