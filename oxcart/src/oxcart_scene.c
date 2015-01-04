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
#include "oxcart_assert.h"
#include "oxcart_app.h"
#include "oxcart_atlas.h"
#include "oxcart_cube.h"
#include "oxcart_font.h"
#include "oxcart_gl.h"
#include "oxcart_math.h"
#include "oxcart_scene.h"
#include "oxcart_shader.h"
#include "oxcart_state.h"
#include "oxcart_text.h"
#include "oxcart_util.h"

#define OXCART_UBO_CAMERA_ORTHO 0
#define OXCART_UBO_CAMERA_PERSP 1

typedef struct oxcart_camera_t oxcart_camera_t;
typedef struct oxcart_module_t oxcart_module_t;

struct oxcart_camera_t
{
  oxcart_mat4_t projection;
  oxcart_mat4_t view;
};

struct oxcart_module_t
{
  size_t past;
  size_t osec;
  size_t fps;
  size_t frames;
  GLuint ubo[2];
  oxcart_camera_t ortho;
  oxcart_camera_t persp;
  oxcart_cube_t* cube;
  oxcart_text_t* text;
};

static oxcart_module_t _m = {0};

/**
 * 
 */
void oxcart_scene_initialize()
{
  int x, y, w, h;
  oxcart_vec3_t eye;
  oxcart_vec3_t target;
  oxcart_vec3_t up;

  glEnable(GL_CULL_FACE);
  glEnable(GL_DEPTH_TEST);
  glDepthFunc(GL_LEQUAL);
  glEnable(GL_MULTISAMPLE);
  glClearColor(0.2f, 0.2f, 0.2f, 1.0f);

  /* setup orthographic uniform buffer */
  _m.ortho.view = oxcart_mat4_identity();
  glGenBuffers(1, &_m.ubo[OXCART_UBO_CAMERA_ORTHO]);
  glBindBuffer(GL_UNIFORM_BUFFER, _m.ubo[OXCART_UBO_CAMERA_ORTHO]);
  glBufferData(GL_UNIFORM_BUFFER, sizeof(oxcart_camera_t), &_m.ortho, GL_DYNAMIC_DRAW);
  glBindBufferBase(GL_UNIFORM_BUFFER, OXCART_SHADER_BINDPOINT_CAMERA_ORTHO, _m.ubo[OXCART_UBO_CAMERA_ORTHO]);

  /* setup perspective uniform buffer */
  eye = oxcart_vec3_set(0.0f, 0.0f, 0.0f);
  target = oxcart_vec3_set(0.0f, 0.0f, -1.0f);
  up = oxcart_vec3_set(0.0f, 1.0f, 0.0f);
  _m.persp.view = oxcart_mat4_lookat(&eye, &target, &up);
  glGenBuffers(1, &_m.ubo[OXCART_UBO_CAMERA_PERSP]);
  glBindBuffer(GL_UNIFORM_BUFFER, _m.ubo[OXCART_UBO_CAMERA_PERSP]);
  glBufferData(GL_UNIFORM_BUFFER, sizeof(oxcart_camera_t), &_m.persp, GL_DYNAMIC_DRAW);
  glBindBufferBase(GL_UNIFORM_BUFFER, OXCART_SHADER_BINDPOINT_CAMERA_PERSP, _m.ubo[OXCART_UBO_CAMERA_PERSP]);

  /* initialize the viewport */
  oxcart_window_rect(&x, &y, &w, &h);
  oxcart_scene_setviewport(w, h);

  /* create cube */
  _m.cube = oxcart_cube_create();

  /* create text */
  _m.text = oxcart_text_create();
}

/**
 * 
 */
void oxcart_scene_terminate()
{
  oxcart_text_destroy(_m.text);
  oxcart_cube_destroy(_m.cube);
  glDeleteBuffers(1, &_m.ubo[OXCART_UBO_CAMERA_PERSP]);
  glDeleteBuffers(1, &_m.ubo[OXCART_UBO_CAMERA_ORTHO]);
}

/**
 * 
 */
void oxcart_scene_setviewport(int w, int h)
{
  OXCART_ASSERT(w > 0);
  OXCART_ASSERT(h > 0);

  glViewport(0, 0, w, h);

  _m.ortho.projection = oxcart_mat4_orthographic((float)w, (float)h);
  glBindBuffer(GL_UNIFORM_BUFFER, _m.ubo[OXCART_UBO_CAMERA_ORTHO]);
  glBufferSubData(GL_UNIFORM_BUFFER, OXCART_OFFSET(oxcart_camera_t, projection), sizeof(oxcart_mat4_t), _m.ortho.projection.d);

  _m.persp.projection = oxcart_mat4_perspective(45.0f, (float)w / (float)h, 0.1f, 1000.0f);
  glBindBuffer(GL_UNIFORM_BUFFER, _m.ubo[OXCART_UBO_CAMERA_PERSP]);
  glBufferSubData(GL_UNIFORM_BUFFER, OXCART_OFFSET(oxcart_camera_t, projection), sizeof(oxcart_mat4_t), _m.persp.projection.d);
}

/**
 * 
 */
void oxcart_scene_draw(float coeff)
{
  float angle;
  size_t now;
  size_t elapsed;
  oxcart_mat4_t rotate;
  oxcart_mat4_t translate;
  oxcart_mat4_t model;
  oxcart_vec2_t pen;
  oxcart_markup_t markup;
  oxcart_metrics_t metrics;
  char buffer[16];

  now = oxcart_time_tick();
  elapsed = now - _m.past;
  _m.past = now;
  _m.frames++;

  if ((now - _m.osec) >= 1000) {
    _m.osec = now;
    _m.fps = _m.frames;
    _m.frames = 0;
  }

  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  angle = (g_state.next_ang - g_state.prev_ang) * coeff;
  angle += g_state.prev_ang;
  rotate = oxcart_mat4_rotate(0.5f, 1.0f, 0.0f, angle);
  translate = oxcart_mat4_translate(0.0f, 0.0f, -20.0f);
  model = oxcart_mat4_multiply(&translate, &rotate);
  oxcart_cube_draw(_m.cube, &model);

  markup = oxcart_markup_defaults();
  sprintf_s(buffer, OXCART_ARRAY_SIZE(buffer), " dt: %ums", elapsed);
  pen = oxcart_vec2_set(0.0f, 0.0f);
  oxcart_text_assign(_m.text, &markup, buffer, 0, &pen);
  oxcart_text_metrics(_m.text, &markup, buffer, 0, &metrics);
  sprintf_s(buffer, OXCART_ARRAY_SIZE(buffer), "fps: %u", _m.fps);
  pen = oxcart_vec2_set(0.0f, metrics.baseline);
  oxcart_text_append(_m.text, &markup, buffer, 0, &pen);
  model = oxcart_mat4_translate(-metrics.bearing, -metrics.ascent, 0.0f);
  oxcart_text_draw(_m.text, &model);

  oxcart_window_swap();
}
