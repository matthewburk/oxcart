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

#include <stdio.h>
#include <stdlib.h>
#include <physfs.h>
#include "oxcart_assert.h"
#include "oxcart_shader.h"

#define OXCART_INFOLOG_LENGTH 1024

/**
 * 
 */
GLuint oxcart_shader_createwithfile(const GLchar* filename, GLenum type)
{
  PHYSFS_File* file;
  PHYSFS_sint64 size;
  GLchar* buffer;
  GLuint shader;

  OXCART_ASSERT(filename);

  if (!(file = PHYSFS_openRead(filename))) {
    OXCART_ASSERT(!"PHYSFS_openRead() failed");
  }

  size = PHYSFS_fileLength(file);
  OXCART_ASSERT(size > 0);

  if (!(buffer = (GLchar*)malloc((size_t)size + 1))) {
    OXCART_ASSERT(!"malloc() failed");
  }

  if (size > PHYSFS_read(file, buffer, 1, (size_t)size)) {
    OXCART_ASSERT(!"PHYSFS_read() failed");
  }

  buffer[size] = '\0';

  shader = oxcart_shader_createwithstr(buffer, type);

  free(buffer);
  PHYSFS_close(file);

  return(shader);
}

/**
 * 
 */
GLuint oxcart_shader_createwithstr(const GLchar* str, GLenum type)
{
  GLuint shader;
  GLint status;

  OXCART_ASSERT(str);

  if (!(shader = glCreateShader(type))) {
    OXCART_ASSERT(!"glCreateShader() failed");
  }

  glShaderSource(shader, 1, &str, 0);
  glCompileShader(shader);
  glGetShaderiv(shader, GL_COMPILE_STATUS, &status);

  if (!status) {
    #ifdef _DEBUG
      GLchar log[OXCART_INFOLOG_LENGTH];
      glGetShaderInfoLog(shader, OXCART_INFOLOG_LENGTH, 0, log);
      fprintf_s(stderr, "%s\n", log);
    #endif

    OXCART_ASSERT(!"GL_COMPILE_STATUS failed");
  }

  return(shader);
}

/**
 *
 */
void oxcart_shader_destroy(const GLuint* shader, GLsizei count)
{
  GLsizei i;

  OXCART_ASSERT(shader);
  OXCART_ASSERT(count > 0);

  for (i = 0; i < count; i++) {
    glDeleteShader(shader[i]);
  }
}

/**
 * 
 */
GLuint oxcart_program_create(const GLuint* shader, GLsizei count)
{
  GLsizei i;
  GLuint program;

  OXCART_ASSERT(shader);
  OXCART_ASSERT(count > 0);

  if (!(program = glCreateProgram())) {
    OXCART_ASSERT(!"glCreateProgram() failed");
  }

  for (i = 0; i < count; i++) {
    glAttachShader(program, shader[i]);
  }

  return(program);
}

/**
 * 
 */
void oxcart_program_destroy(GLuint program)
{
  glDeleteProgram(program);
}

/**
 * 
 */
void oxcart_program_link(GLuint program)
{
  GLint status;

  glLinkProgram(program);
  glGetProgramiv(program, GL_LINK_STATUS, &status);

  if (!status) {
    #ifdef _DEBUG
      GLchar log[OXCART_INFOLOG_LENGTH];
      glGetProgramInfoLog(program, OXCART_INFOLOG_LENGTH, 0, log);
      fprintf_s(stderr, "%s\n", log);
    #endif

    OXCART_ASSERT(!"GL_LINK_STATUS failed");
  }
}
