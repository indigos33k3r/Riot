/*
 *  graphicsDevice_opengl.cpp
 *  renderEngine
 *
 *  Created by Kyle Weicht on 1/10/12.
 *  Copyright (c) 2012 Kyle Weicht. All rights reserved.
 */
#include "graphicsDevice.h"
#include "renderEngineInternal.h"

/*
 * Platform detection
 */
#define BUILD_PLATFORM_WINDOWS 1
#define BUILD_PLATFORM_MACOS   2
#define BUILD_PLATFORM_LINUX   3
#define BUILD_PLATFORM_IOS     4
#define BUILD_PLATFORM_ANDROID 5

#if defined( WIN32 )
    #define BUILD_PLATFORM_ID BUILD_PLATFORM_WINDOWS
    #ifndef WIN32_LEAN_AND_MEAN
        #define WIN32_LEAN_AND_MEAN
    #endif
#elif defined( __MACH__ )
    #include <TargetConditionals.h>
    #if( TARGET_OS_IPHONE )
        #define BUILD_PLATFORM_ID BUILD_PLATFORM_IOS
    #else
        #define BUILD_PLATFORM_ID BUILD_PLATFORM_MACOS
    #endif
#elif defined( __ANDROID__ )
    #define BUILD_PLATFORM_ID BUILD_PLATFORM_ANDROID
#elif defined( __linux__ )
    #define BUILD_PLATFORM_ID BUILD_PLATFORM_LINUX
#else
    #error No valid target found
#endif


#include "glew/gl/glew.h"
#include "glew/gl/wglew.h"
#include <stdio.h>

//#include <c++headers>

//#include "external_headers.h"

//#include "internal_headers.h"

namespace
{

/*******************************************************************\
 Internal constants and types
\*******************************************************************/

/*******************************************************************\
 Internal variables
\*******************************************************************/
HDC     s_hDC         = nullptr;
HGLRC   s_hGLRC       = nullptr;
int     s_pixelFormat = -1;

GLuint  s_vao = 0;

/*******************************************************************\
 Internal functions
\*******************************************************************/
GLuint CompileShader(GLenum shaderType, const char* shaderSource)
{   
    GLchar  statusBuffer[1024] = {0};
    GLint   status = GL_TRUE;
    GLuint  shader = glCreateShader(shaderType);
    glShaderSource(shader, 1, &shaderSource, NULL);
    glCompileShader(shader);
    
    glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
    if(status == GL_FALSE)
    {
        glGetShaderInfoLog(shader, sizeof(statusBuffer), NULL, statusBuffer);
        printf("Error:\t%s\n", statusBuffer);
        assert(0);
    }
    
    return shader;
}

} // namespace

namespace GraphicsDevice
{

/*******************************************************************\
 External variables
\*******************************************************************/

/*******************************************************************\
 External functions
\*******************************************************************/
#if BUILD_PLATFORM_ID == BUILD_PLATFORM_WINDOWS
void Initialize(void* window)
{
    HDC     hDC = NULL;
    HGLRC   hGLRC = NULL;
    HWND    hWnd = static_cast<HWND>(window);
    int     pixelFormat = 0;
    
    //
    // Create context to load extensions
    //
    hDC = GetDC(hWnd);
    
	PIXELFORMATDESCRIPTOR pfd;
	memset(&pfd, 0, sizeof(PIXELFORMATDESCRIPTOR));
	pfd.nSize  = sizeof(PIXELFORMATDESCRIPTOR);
	pfd.nVersion   = 1;
	pfd.dwFlags    = PFD_DOUBLEBUFFER | PFD_SUPPORT_OPENGL | PFD_DRAW_TO_WINDOW;
	pfd.iPixelType = PFD_TYPE_RGBA;
	pfd.cColorBits = 32;
	pfd.cDepthBits = 32;
	pfd.iLayerType = PFD_MAIN_PLANE;

    pixelFormat = ChoosePixelFormat(hDC, &pfd);
    assert(pixelFormat != 0);

    BOOL bSuccess = SetPixelFormat(hDC, pixelFormat, &pfd);
    assert(bSuccess);

    hGLRC = wglCreateContext(hDC);
    wglMakeCurrent(hDC, hGLRC);

    // Initialize Glew
    GLenum error = glewInit();
    if(GLEW_OK != error)
    {
        /* Problem: glewInit failed, something is seriously wrong. */
        printf("Error: %s\n", glewGetErrorString(error));
        assert(0);
    }

    //
    // Create new OpenGL context
    //

    assert(wglewIsSupported("WGL_ARB_create_context") == 1);

    GLint pCreationAttributes[] =
    {
        WGL_CONTEXT_MAJOR_VERSION_ARB, 3,
        WGL_CONTEXT_MINOR_VERSION_ARB, 2,
        0,
    };
    HGLRC newContext = wglCreateContextAttribsARB( hDC, 0, pCreationAttributes );
    assert(hGLRC);

    wglMakeCurrent(nullptr, nullptr);
    wglDeleteContext(hGLRC);
    wglMakeCurrent(hDC, newContext);
    hGLRC = newContext;

    s_hDC   = hDC;
    s_hGLRC = hGLRC;
    s_pixelFormat = pixelFormat;

    const char* majorVersion = (const char*)glGetString(GL_VERSION);

    //
    // Perform OpenGL initialization
    //
    glGenVertexArrays(1, &s_vao);
    glBindVertexArray(s_vao);
}
void Shutdown(void)
{
    wglMakeCurrent(NULL, NULL);
    wglDeleteContext(s_hGLRC);
    s_hGLRC = nullptr;
}
#else
    #error Need OpenGL
#endif // BUILD_PLATFORM_ID

// Pipeline control
void SetClearColor(float r, float g, float b, float a)
{
    glClearColor(r,g,b,a);
}
void SetClearDepth(float d)
{
    glClearDepth(d);
}

// Frame controls
void Clear(void)
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}
void Present(void)
{
#if BUILD_PLATFORM_ID == BUILD_PLATFORM_WINDOWS
    SwapBuffers(s_hDC);
#else
    #error Need OpenGL
#endif // BUILD_PLATFORM_ID
}


// Render object creation
shader_t CreateVertexShader(const char* shaderSource)
{
    GLuint shaderId = CompileShader(GL_VERTEX_SHADER, shaderSource);
    shader_t shader;
    shader.intShader = shaderId;

    return shader;
}
shader_t CreatePixelShader(const char* shaderSource)
{
    GLuint shaderId = CompileShader(GL_FRAGMENT_SHADER, shaderSource);
    shader_t shader;
    shader.intShader = shaderId;

    return shader;
}
buffer_t CreateVertexBuffer(size_t size, const void* data)
{
    GLuint bufferId = 0;
    glGenBuffers(1, &bufferId);
    glBindBuffer(GL_ARRAY_BUFFER, bufferId);
    glBufferData(GL_ARRAY_BUFFER, (GLsizeiptr)size, data, GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    
    buffer_t buffer;
    buffer.intBuffer = bufferId;

    return buffer;
}
buffer_t CreateIndexBuffer(size_t size, const void* data)
{
    GLuint bufferId = 0;
    glGenBuffers(1, &bufferId);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, bufferId);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, (GLsizeiptr)size, data, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    
    buffer_t buffer;
    buffer.intBuffer = bufferId;

    return buffer;
}

} // namespace GraphicsDevice
