/*
 *  renderEngine.h
 *  renderEngine.lib
 *
 *  Created by Kyle Weicht on 1/10/12.
 *  Copyright (c) 2012 Kyle Weicht. All rights reserved.
 */
#ifndef __renderEngineLib_h__
#define __renderEngineLib_h__

#include <stdint.h>
#include <stddef.h>

//#include <c++headers>

//#include "external_headers.h"

//#include "internal_headers.h"

namespace Render
{

/*******************************************************************\
 External constants and types
\*******************************************************************/
enum shader_type_e
{
    kVertexShader,
    kPixelShader,
};
enum vertex_layout_e
{
    kPosition,
};
typedef int shader_t;
typedef int mesh_t;

/*******************************************************************\
 Variables
\*******************************************************************/
extern const size_t kRenderEngineSize;

/*******************************************************************\
 External functions
\*******************************************************************/
void Initialize(void* window, void* memoryBuffer, size_t bufferSize);
void Shutdown(void);
void Frame(void);

shader_t CreateShader(const char* shaderSource, shader_type_e type);
mesh_t CreateMesh(  vertex_layout_e layout, 
                    int indexCount, 
                    int vertexCount, 
                    size_t vertexSize, 
                    size_t indexSize, 
                    const void* vertices, 
                    const void* indices);

} // namespace Render

#endif /* include guard */
