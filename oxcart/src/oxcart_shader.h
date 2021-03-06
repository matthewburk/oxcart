/**
 * oxcart is licensed under the terms of the MIT license reproduced below.
 * 
 * Copyright © 2014 Thomas J. Schaefer
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy of 
 * this software and associated documentation files (the "Software"), to deal in 
 * the Software without restriction, including without limitation the rights to 
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
 * the Software, and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR 
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR 
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER 
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN 
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

#ifndef OXCART_SHADER_H
#define OXCART_SHADER_H

#include "oxcart_gl.h"

#ifdef __cplusplus
extern "C" {
#endif

#define OXCART_SHADER_ATTRIB_LOCATION_VERTEX   0
#define OXCART_SHADER_ATTRIB_LOCATION_NORMAL   1
#define OXCART_SHADER_ATTRIB_LOCATION_TEXCOORD 2
#define OXCART_SHADER_ATTRIB_LOCATION_COLOR    3
#define OXCART_SHADER_BINDPOINT_CAMERA_ORTHO   0
#define OXCART_SHADER_BINDPOINT_CAMERA_PERSP   1

GLuint oxcart_shader_createwithfile(const GLchar* filename, GLenum type);
GLuint oxcart_shader_createwithstr(const GLchar* str, GLenum type);
void oxcart_shader_destroy(const GLuint* shader, GLsizei count);

GLuint oxcart_program_create(const GLuint* shader, GLsizei count);
void oxcart_program_destroy(GLuint program);
void oxcart_program_link(GLuint program);

#ifdef __cplusplus
}
#endif

#endif
