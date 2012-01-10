/*
 *  render_engine.h
 *  riot
 *
 *  Created by Kyle Weicht on 10/19/11.
 *  Copyright (c) 2011 Kyle Weicht. All rights reserved.
 */
#ifndef riot_render_engine_h_
#define riot_render_engine_h_

/*******************************************************************\
 External constants and types
\*******************************************************************/
enum RenderStatus
{
    kOk = 0,
    kError,
    kUninitialized,
};

enum GraphicsDeviceType
{
    kNull = 0,
    kOpenGL,
    kDirect3D,
};

namespace RenderEngine
{

typedef int shader_id_t;
typedef int material_id_t;
/*******************************************************************\
 Variables
\*******************************************************************/
extern RenderStatus g_status;

/*******************************************************************\
 External functions
\*******************************************************************/
void Initialize(void);
void CreateDevice(void* window, GraphicsDeviceType type);
void Shutdown(void);
void Frame(void);
shader_id_t CreateVertexShader(const char* source);
shader_id_t CreatePixelShader(const char* source);

} // namespace RenderEngine

#endif /* include guard */
