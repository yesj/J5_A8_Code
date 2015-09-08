/*****************************************************************************
 * common.h
 *
 * bc-cat module unit test - common header
 *
 * Copyright (C) 2010 Texas Instruments Incorporated - http://www.ti.com/
 *
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 *   Redistributions of source code must retain the above copyright
 *   notice, this list of conditions and the following disclaimer.
 *   
 *   Redistributions in binary form must reproduce the above copyright
 *   notice, this list of conditions and the following disclaimer in the
 *   documentation and/or other materials provided with the
 *   distribution.
 *   
 *   Neither the name of Texas Instruments Incorporated nor the names of
 *   its contributors may be used to endorse or promote products derived
 *   from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 ****************************************************************************/
#ifndef __COMMON_H__
#define __COMMON_H__

#ifdef GLES_20
#include <GLES2/gl2.h>
#include <EGL/egl.h>
#include <GLES2/gl2ext.h>
#else
#include <GLES/gl.h>
#include <GLES/egl.h>
#include <GLES/glext.h>
#endif

#define TRUE            1
#define FALSE           0

typedef struct tex_buffer_info {
    int n;
    int w;
    int h;
    int fmt;
} tex_buffer_info_t;


extern int gQuit;
extern int profiling;

//liuxu, 06/19/2014, no need to use GPU for rending DSP output anymore. #define TI_DSP_PROCESSING//liuxu, 02/12/2014. 

//#define DIRECT_SHOW//liuxu, 02/16/2014.
//liuxu, 06/02/2014, for side view by GPU w/o Cube. #define TINY_CUBE//liuxu, 02/18/2014.

//#define SIDE_VIEW//liuxu, 06/02/2014. 

//liuxu, 06/19/2014, disable the DIRECT_SHOW and SIDE_VIEW at the same time. 

#ifndef DIRECT_SHOW
extern GLfloat cube_vertices[16][3];
extern GLfloat cube_normals[16][3];
extern GLfloat cube_tex_coords[16][2];
#else

#ifdef SIDE_VIEW
extern GLfloat cube_vertices[12][3];//liuxu, 02/16/2014, for demo tour.
extern GLfloat cube_tex_coords[12][2];
#else
extern GLfloat cube_vertices[6][3];//liuxu, 02/16/2014, for demo tour.
extern GLfloat cube_tex_coords[6][2];
#endif

extern GLfloat cube_normals[6][3];

#ifdef TINY_CUBE
extern GLfloat cube_vertices_tiny[16][3];
extern GLfloat cube_tex_coords_tiny[16][2];
extern float modelview_tiny[];
#endif

#endif

extern PFNGLTEXBINDSTREAMIMGPROC glTexBindStreamIMG;
extern EGLDisplay dpy;
extern EGLSurface surface;

#ifdef GLES_20
extern GLuint *ptex_objs;
extern GLint model_view_idx[], proj_idx[];
extern int program[];
extern float projection[];
extern float modelview[];
#endif

void signalHandler(int signum);
int get_disp_resolution(int *w, int *h);
int initTexExt(int bcdev_id, tex_buffer_info_t *binfo);
int initEGL(int n_buf);
void deInitEGL(int n_buf);

void pattern_uyvy(int fr_idx, void *p, int w, int h);

int reset_shaders_idx0(int bcdev_id);

#ifdef X11
#include "X11/Xlib.h"
#include "X11/Xutil.h"

extern Display* x11Display;
extern Window   x11Window;
extern Colormap x11Colormap;

int initX11(int *w, int *h);
void deInitX11(void);
int doX11Events(void);
#endif

#endif /* __COMMON_H__ */
