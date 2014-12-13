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

#include <stdlib.h>
#include "oxcart_assert.h"
#include "oxcart_cube.h"
#include "oxcart_gl.h"
#include "oxcart_math.h"
#include "oxcart_shader.h"
#include "oxcart_util.h"

typedef struct oxcart_shader_t oxcart_shader_t;
typedef struct oxcart_module_t oxcart_module_t;

struct oxcart_cube_t
{
  oxcart_vec4_t color;
  GLuint vao;
  GLuint vbo;
  GLuint ibo;
};

struct oxcart_shader_t
{
  GLuint program;
  GLuint camera;
  GLint model;
  GLint color;
  GLint vertex;
};

struct oxcart_module_t
{
  int init;
  oxcart_shader_t shader;
};

static void _module_initialize();
static void _module_loadshader();

static const GLfloat _vertices[] = {
  -0.25f, -0.25f, -0.25f,
  -0.25f,  0.25f, -0.25f,
   0.25f, -0.25f, -0.25f,
   0.25f,  0.25f, -0.25f,
   0.25f, -0.25f,  0.25f,
   0.25f,  0.25f,  0.25f,
  -0.25f, -0.25f,  0.25f,
  -0.25f,  0.25f,  0.25f
};

static const GLuint _indices[] = {
  0, 1, 2,
  2, 1, 3,
  2, 3, 4,
  4, 3, 5,
  4, 5, 6,
  6, 5, 7,
  6, 7, 0,
  0, 7, 1,
  2, 4, 0,
  0, 4, 6,
  5, 3, 7,
  7, 3, 1
};

static oxcart_module_t _m = {0};

/**
 * 
 */
oxcart_cube_t* oxcart_cube_create(const oxcart_vec4_t* color)
{
  oxcart_cube_t* cube;

  OXCART_ASSERT(color);

  _module_initialize();

  if (!(cube = (oxcart_cube_t*)malloc(sizeof(oxcart_cube_t)))) {
    OXCART_ASSERT(!"malloc() failed");
  }

  cube->color = *color;

  glGenVertexArrays(1, &cube->vao);
  glBindVertexArray(cube->vao);

  glGenBuffers(1, &cube->vbo);
  glBindBuffer(GL_ARRAY_BUFFER, cube->vbo);
  glBufferData(GL_ARRAY_BUFFER, sizeof(_vertices), _vertices, GL_STATIC_DRAW);
  glVertexAttribPointer(_m.shader.vertex, 3, GL_FLOAT, GL_FALSE, 0, 0);
  glEnableVertexAttribArray(_m.shader.vertex);

  glGenBuffers(1, &cube->ibo);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, cube->ibo);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(_indices), _indices, GL_STATIC_DRAW);

  return(cube);
}

/**
 * 
 */
void oxcart_cube_destroy(oxcart_cube_t* cube)
{
  OXCART_ASSERT(cube);

  glDeleteBuffers(1, &cube->ibo);
  glDeleteBuffers(1, &cube->vbo);
  glDeleteVertexArrays(1, &cube->vao);
  free(cube);
}

/**
 * 
 */
void oxcart_cube_draw(oxcart_cube_t* cube, const oxcart_mat4_t* model)
{
  OXCART_ASSERT(cube);
  OXCART_ASSERT(model);

  glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

  glUseProgram(_m.shader.program);
  glUniformMatrix4fv(_m.shader.model, 1, GL_FALSE, model->d);
  glUniform4fv(_m.shader.color, 1, cube->color.d);
  glBindVertexArray(cube->vao);
  glDrawElements(GL_TRIANGLES, OXCART_ARRAY_SIZE(_indices), GL_UNSIGNED_INT, 0);

  glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}

/**
 * 
 */
static void _module_initialize()
{
  if (_m.init) {
    return;
  }

  _m.init = 1;

  _module_loadshader();
}

/**
 * 
 */
static void _module_loadshader()
{
  GLuint shader[2];

  shader[0] = oxcart_shader_createwithfile("/shader/oxcart_cube.vert", GL_VERTEX_SHADER);
  shader[1] = oxcart_shader_createwithfile("/shader/oxcart_cube.frag", GL_FRAGMENT_SHADER);
  _m.shader.program = oxcart_shader_link(shader, OXCART_ARRAY_SIZE(shader));

  _m.shader.camera = glGetUniformBlockIndex(_m.shader.program, "camera");
  glUniformBlockBinding(_m.shader.program, _m.shader.camera, OXCART_SHADER_BINDPOINT_PERSP_CAMERA);
  _m.shader.model = glGetUniformLocation(_m.shader.program, "model");
  _m.shader.color = glGetUniformLocation(_m.shader.program, "color");
  _m.shader.vertex = glGetAttribLocation(_m.shader.program, "vertex");

  oxcart_shader_destroy(shader, OXCART_ARRAY_SIZE(shader));
}
