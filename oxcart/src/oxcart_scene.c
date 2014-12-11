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

#include "oxcart_assert.h"
#include "oxcart_app.h"
#include "oxcart_atlas.h"
#include "oxcart_cube.h"
#include "oxcart_font.h"
#include "oxcart_gl.h"
#include "oxcart_math.h"
#include "oxcart_scene.h"
#include "oxcart_state.h"
#include "oxcart_text.h"

typedef struct oxcart_module_t oxcart_module_t;

struct oxcart_module_t
{
  oxcart_mat4_t ortho;
  oxcart_mat4_t persp;
  oxcart_mat4_t camera;
  oxcart_cube_t* cube;
  oxcart_text_t* text;
  oxcart_metrics_t metrics;
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
  oxcart_vec4_t color;
  oxcart_markup_t markup;
  oxcart_vec2_t pen = oxcart_vec2_zero();
  const char* str = "The quick brown fox jumps over the lazy dog. 0123456789";

  oxcart_window_rect(&x, &y, &w, &h);
  oxcart_scene_setviewport(w, h);

  glEnable(GL_CULL_FACE);
  glEnable(GL_DEPTH_TEST);
  glDepthFunc(GL_LEQUAL);
  glEnable(GL_MULTISAMPLE);
  glClearColor(0.2f, 0.2f, 0.2f, 1.0f);

  eye = oxcart_vec3_set(0.0f, 0.0f, 0.0f);
  target = oxcart_vec3_set(0.0f, 0.0f, -1.0f);
  up = oxcart_vec3_set(0.0f, 1.0f, 0.0f);
  _m.camera = oxcart_mat4_lookat(&eye, &target, &up);

  color = oxcart_vec4_set(1.0f, 1.0f, 0.0f, 1.0f);
  _m.cube = oxcart_cube_create(&color);

  markup = oxcart_markup_defaults();
  markup.size = 22;
  markup.style = OXCART_TEXT_STYLE_BOLD_ITALIC;
  _m.text = oxcart_text_create();
  oxcart_text_metrics(_m.text, &markup, str, 0, &_m.metrics);
  oxcart_text_assign(_m.text, &markup, str, 0, &pen);
}

/**
 * 
 */
void oxcart_scene_terminate()
{
  oxcart_text_destroy(_m.text);
  oxcart_cube_destroy(_m.cube);
}

/**
 * 
 */
void oxcart_scene_setviewport(int w, int h)
{
  OXCART_ASSERT(w > 0);
  OXCART_ASSERT(h > 0);

  glViewport(0, 0, w, h);

  _m.ortho = oxcart_mat4_orthographic((float)w, (float)h);
  _m.persp = oxcart_mat4_perspective(60.0f, (float)w / (float)h, 1.0f, 1000.0f);
}

/**
 * 
 */
void oxcart_scene_draw(float coeff)
{
  float angle;
  oxcart_mat4_t rotate;
  oxcart_mat4_t translate;
  oxcart_mat4_t mv;
  oxcart_mat4_t mvp;

  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  angle = (g_state.next_ang - g_state.prev_ang) * coeff;
  angle += g_state.prev_ang;
  rotate = oxcart_mat4_rotate(1.0f, 1.0f, 0.0f, angle);
  translate = oxcart_mat4_translate(0.0f, 0.0f, -5.0f);
  mv = oxcart_mat4_multiply(&translate, &rotate);
  mv = oxcart_mat4_multiply(&_m.camera, &mv);
  mvp = oxcart_mat4_multiply(&_m.persp, &mv);
  oxcart_cube_draw(_m.cube, &mvp);

  translate = oxcart_mat4_translate(-_m.metrics.bearing, -_m.metrics.ascent, 0.0f);
  mvp = oxcart_mat4_multiply(&_m.ortho, &translate);
  oxcart_text_draw(_m.text, &mvp);

  oxcart_window_swap();
}
