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
#include <string.h>
#include "oxcart_assert.h"
#include "oxcart_atlas.h"
#include "oxcart_font.h"
#include "oxcart_gl.h"
#include "oxcart_hashmap.h"
#include "oxcart_lua.h"
#include "oxcart_math.h"
#include "oxcart_shader.h"
#include "oxcart_text.h"
#include "oxcart_util.h"
#include "oxcart_vector.h"

#define OXCART_ATLAS_SIZE 1024
#define OXCART_STYLE_LENGTH 4

typedef struct oxcart_config_t oxcart_config_t;
typedef struct oxcart_shader_t oxcart_shader_t;
typedef struct oxcart_vertex_t oxcart_vertex_t;
typedef struct oxcart_module_t oxcart_module_t;

struct oxcart_config_t
{
  oxcart_hashmap_t* fontmap;
};

struct oxcart_shader_t
{
  GLuint program;
  GLuint camera;
  GLint model;
  GLint sampler;
};

struct oxcart_text_t
{
  int changed;
  oxcart_atlas_t* atlas;
  oxcart_vector_t* fonts;
  int vcapacity;
  oxcart_vector_t* vertices;
  int icapacity;
  oxcart_vector_t* indices;
  GLuint vao;
  GLuint vbo;
  GLuint ibo;
  GLuint tex;
};

struct oxcart_vertex_t
{
  oxcart_vec2_t vertex;
  oxcart_vec2_t texcoord;
  oxcart_vec4_t color;
};

struct oxcart_module_t
{
  int init;
  oxcart_config_t config;
  oxcart_shader_t shader;
};

static oxcart_font_t* _text_font(oxcart_text_t* text, const oxcart_markup_t* markup);
static void _module_initialize();
static void _module_loadconfig();
static void _module_loadshader();

static oxcart_module_t _m = {0};

/**
 * 
 */
oxcart_text_t* oxcart_text_create()
{
  oxcart_text_t* text;

  _module_initialize();

  if (!(text = (oxcart_text_t*)malloc(sizeof(oxcart_text_t)))) {
    OXCART_ASSERT(!"malloc() failed");
  }

  text->changed = 0;
  text->atlas = oxcart_atlas_create(OXCART_ATLAS_SIZE, OXCART_ATLAS_SIZE, 1);
  text->fonts = oxcart_vector_create(sizeof(oxcart_font_t*), 2);
  text->vcapacity = 128 * 4; /* default capacity: 128 characters, 4 vertices each (only 64 characters if outlined) */
  text->vertices = oxcart_vector_create(sizeof(oxcart_vertex_t), text->vcapacity);
  text->icapacity = 128 * 6; /* default capacity: 128 characters, 6 indices each (only 64 characters if outlined) */
  text->indices = oxcart_vector_create(sizeof(GLuint), text->icapacity);

  glGenVertexArrays(1, &text->vao);
  glBindVertexArray(text->vao);

  glGenBuffers(1, &text->vbo);
  glBindBuffer(GL_ARRAY_BUFFER, text->vbo);
  glBufferData(GL_ARRAY_BUFFER, text->vcapacity * sizeof(oxcart_vertex_t), 0, GL_DYNAMIC_DRAW);
  glVertexAttribPointer(OXCART_SHADER_ATTRIBLOC_VERTEX, 2, GL_FLOAT, GL_FALSE, sizeof(oxcart_vertex_t), OXCART_OFFSET_PTR(oxcart_vertex_t, vertex));
  glEnableVertexAttribArray(OXCART_SHADER_ATTRIBLOC_VERTEX);
  glVertexAttribPointer(OXCART_SHADER_ATTRIBLOC_TEXCOORD, 2, GL_FLOAT, GL_FALSE, sizeof(oxcart_vertex_t), OXCART_OFFSET_PTR(oxcart_vertex_t, texcoord));
  glEnableVertexAttribArray(OXCART_SHADER_ATTRIBLOC_TEXCOORD);
  glVertexAttribPointer(OXCART_SHADER_ATTRIBLOC_COLOR, 4, GL_FLOAT, GL_FALSE, sizeof(oxcart_vertex_t), OXCART_OFFSET_PTR(oxcart_vertex_t, color));
  glEnableVertexAttribArray(OXCART_SHADER_ATTRIBLOC_COLOR);

  glGenBuffers(1, &text->ibo);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, text->ibo);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, text->icapacity * sizeof(GLuint), 0, GL_DYNAMIC_DRAW);

  glGenTextures(1, &text->tex);
  glBindTexture(GL_TEXTURE_2D, text->tex);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexStorage2D(GL_TEXTURE_2D, 1, GL_R8, OXCART_ATLAS_SIZE, OXCART_ATLAS_SIZE);

  return(text);
}

/**
 * 
 */
void oxcart_text_destroy(oxcart_text_t* text)
{
  int i;
  oxcart_font_t* font;

  OXCART_ASSERT(text);

  glDeleteTextures(1, &text->tex);
  glDeleteBuffers(1, &text->ibo);
  glDeleteBuffers(1, &text->vbo);
  glDeleteVertexArrays(1, &text->vao);
  oxcart_vector_destroy(text->indices);
  oxcart_vector_destroy(text->vertices);

  for (i = 0; i < oxcart_vector_size(text->fonts); i++) {
    font = *(oxcart_font_t**)oxcart_vector_item(text->fonts, i);
    oxcart_font_destroy(font);
  }

  oxcart_vector_destroy(text->fonts);
  oxcart_atlas_destroy(text->atlas);
  free(text);
}

/**
 * 
 */
void oxcart_text_metrics(oxcart_text_t* text, const oxcart_markup_t* markup, const char* str, int length, oxcart_metrics_t* metrics)
{
  int i;
  float w, h;
  float ascent;
  float descent;
  float bearing_x;
  float bearing_y;
  float advance;
  oxcart_font_t* font;
  oxcart_glyph_t* glyph;

  OXCART_ASSERT(text);
  OXCART_ASSERT(markup);
  OXCART_ASSERT(str);
  OXCART_ASSERT(length >= 0);
  OXCART_ASSERT(length <= SHRT_MAX);
  OXCART_ASSERT(metrics);

  if (!length) {
    if (!(length = strnlen_s(str, SHRT_MAX))) {
      OXCART_ASSERT(!"Invalid string length");
    }
  }

  font = _text_font(text, markup);
  oxcart_font_metrics(font, &h, &ascent, &descent, &advance);
  metrics->baseline = h; /* per freetype, use height for baseline-to-baseline value */

  glyph = oxcart_font_glyph(font, str[0]);
  oxcart_glyph_metrics(glyph, &w, &h, &bearing_x, &bearing_y, &advance);
  metrics->bearing = bearing_x;
  metrics->ascent = bearing_y;
  metrics->descent = bearing_y + h;
  metrics->advance = advance;

  for (i = 1; i < length; i++) {
    glyph = oxcart_font_glyph(font, str[i]);
    oxcart_glyph_metrics(glyph, &w, &h, &bearing_x, &bearing_y, &advance);

    if (bearing_y < metrics->ascent) {
      metrics->ascent = bearing_y;
    }

    if ((bearing_y + h) > metrics->descent) {
      metrics->descent = bearing_y + h;
    }

    if (markup->style & OXCART_TEXT_STYLE_KERNING) {
      metrics->advance += oxcart_glyph_kerning(glyph, str[i - 1]);
    }

    metrics->advance += advance;
  }

  metrics->w = (metrics->advance - advance + bearing_x + w) - metrics->bearing;
  metrics->h = -metrics->ascent + metrics->descent;
}

/**
 * 
 */
void oxcart_text_assign(oxcart_text_t* text, const oxcart_markup_t* markup, const char* str, int length, oxcart_vec2_t* pen)
{
  OXCART_ASSERT(text);
  OXCART_ASSERT(markup);
  OXCART_ASSERT(str);
  OXCART_ASSERT(length >= 0);
  OXCART_ASSERT(length <= SHRT_MAX);
  OXCART_ASSERT(pen);

  oxcart_text_clear(text);
  oxcart_text_append(text, markup, str, length, pen);
}

/**
 * 
 */
void oxcart_text_append(oxcart_text_t* text, const oxcart_markup_t* markup, const char* str, int length, oxcart_vec2_t* pen)
{
  int i;
  int index;
  float w, h;
  float bearing_x, bearing_y;
  float advance;
  float s0, t0, s1, t1;
  float x0, y0, x1, y1;
  oxcart_font_t* font;
  oxcart_glyph_t* glyph;
  oxcart_vertex_t vertices[4];
  GLuint indices[6];

  OXCART_ASSERT(text);
  OXCART_ASSERT(markup);
  OXCART_ASSERT(str);
  OXCART_ASSERT(length >= 0);
  OXCART_ASSERT(length <= SHRT_MAX);
  OXCART_ASSERT(pen);

  if (!length) {
    if (!(length = strnlen_s(str, SHRT_MAX))) {
      OXCART_ASSERT(!"Invalid string length");
    }
  }

  font = _text_font(text, markup);

  for (i = 0; i < length; i++) {
    glyph = oxcart_font_glyph(font, str[i]);
    oxcart_glyph_metrics(glyph, &w, &h, &bearing_x, &bearing_y, &advance);
    oxcart_glyph_texcoords(glyph, &s0, &t0, &s1, &t1);

    if ((markup->style & OXCART_TEXT_STYLE_KERNING) && (i > 0)) {
      OXCART_VEC_X(*pen) += oxcart_glyph_kerning(glyph, str[i - 1]);
    }

    x0 = OXCART_VEC_X(*pen) + bearing_x;
    y0 = OXCART_VEC_Y(*pen) + bearing_y;
    x1 = x0 + w;
    y1 = y0 + h;

    vertices[0].vertex = oxcart_vec2_set(x0, y0);
    vertices[0].texcoord = oxcart_vec2_set(s0, t0);
    vertices[0].color = markup->color;
    vertices[1].vertex = oxcart_vec2_set(x0, y1);
    vertices[1].texcoord = oxcart_vec2_set(s0, t1);
    vertices[1].color = markup->color;
    vertices[2].vertex = oxcart_vec2_set(x1, y1);
    vertices[2].texcoord = oxcart_vec2_set(s1, t1);
    vertices[2].color = markup->color;
    vertices[3].vertex = oxcart_vec2_set(x1, y0);
    vertices[3].texcoord = oxcart_vec2_set(s1, t0);
    vertices[3].color = markup->color;

    index = oxcart_vector_size(text->vertices);
    indices[0] = index + 0;
    indices[1] = index + 1;
    indices[2] = index + 2;
    indices[3] = index + 0;
    indices[4] = index + 2;
    indices[5] = index + 3;

    oxcart_vector_pushbackv(text->vertices, vertices, OXCART_ARRAY_SIZE(vertices));
    oxcart_vector_pushbackv(text->indices, indices, OXCART_ARRAY_SIZE(indices));

    OXCART_VEC_X(*pen) += advance;
  }

  text->changed = 1;
}

/**
 * 
 */
void oxcart_text_clear(oxcart_text_t* text)
{
  OXCART_ASSERT(text);

  text->changed = 0;
  oxcart_vector_clear(text->indices);
  oxcart_vector_clear(text->vertices);
}

/**
 * 
 */
void oxcart_text_draw(oxcart_text_t* text, const oxcart_mat4_t* model)
{
  OXCART_ASSERT(text);
  OXCART_ASSERT(model);

  if (text->changed) {
    glBindBuffer(GL_ARRAY_BUFFER, text->vbo);

    if (text->vcapacity != oxcart_vector_capacity(text->vertices)) {
      text->vcapacity = oxcart_vector_capacity(text->vertices);
      glBufferData(GL_ARRAY_BUFFER, text->vcapacity * sizeof(oxcart_vertex_t), oxcart_vector_front(text->vertices), GL_DYNAMIC_DRAW);
    }
    else {
      glBufferSubData(GL_ARRAY_BUFFER, 0, oxcart_vector_size(text->vertices) * sizeof(oxcart_vertex_t), oxcart_vector_front(text->vertices));
    }

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, text->ibo);

    if (text->icapacity != oxcart_vector_capacity(text->indices)) {
      text->icapacity = oxcart_vector_capacity(text->indices);
      glBufferData(GL_ELEMENT_ARRAY_BUFFER, text->icapacity * sizeof(GLuint), oxcart_vector_front(text->indices), GL_DYNAMIC_DRAW);
    }
    else {
      glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, oxcart_vector_size(text->indices) * sizeof(GLuint), oxcart_vector_front(text->indices));
    }

    text->changed = 0;
  }

  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glBlendColor(1.0f, 1.0f, 1.0f, 1.0f);

  glUseProgram(_m.shader.program);
  glUniformMatrix4fv(_m.shader.model, 1, GL_FALSE, model->d);
  glBindVertexArray(text->vao);
  glBindTexture(GL_TEXTURE_2D, text->tex);
  glDrawElements(GL_TRIANGLES, oxcart_vector_size(text->indices), GL_UNSIGNED_INT, 0);
}

/**
 * 
 */
static oxcart_font_t* _text_font(oxcart_text_t* text, const oxcart_markup_t* markup)
{
  int i;
  int size;
  float line;
  const char* filename;
  const char* filepath;
  oxcart_vector_t* filepaths;
  oxcart_font_t* font;

  OXCART_ASSERT(text);
  OXCART_ASSERT(markup);
  OXCART_ASSERT(markup->name);

  if (!(filepaths = (oxcart_vector_t*)oxcart_hashmap_item(_m.config.fontmap, markup->name))) {
    OXCART_ASSERT(!"Invalid markup name");
  }

  filepath = (const char*)oxcart_vector_item(filepaths, markup->style & 0x0F);

  for (i = 0; i < oxcart_vector_size(text->fonts); i++) {
    font = *(oxcart_font_t**)oxcart_vector_item(text->fonts, i);
    filename = oxcart_font_properties(font, &size, &line);

    if (!strncmp(filepath, filename, OXCART_FILEPATH_LENGTH) && (markup->size == size) && OXCART_ISEQUALF(markup->line, line, 0.001f)) {
      return(font);
    }
  }

  font = oxcart_font_create(filepath, markup->size, markup->line, text->atlas);

  /* upload modified atlas data containing new font */
  glBindTexture(GL_TEXTURE_2D, text->tex);
  glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, OXCART_ATLAS_SIZE, OXCART_ATLAS_SIZE, GL_RED, GL_UNSIGNED_BYTE, oxcart_atlas_data(text->atlas));

  return(*(oxcart_font_t**)oxcart_vector_pushback(text->fonts, &font));
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

  _module_loadconfig();
  _module_loadshader();
}

/**
 * 
 */
static void _module_loadconfig()
{
  int t, i;
  lua_State* L;
  const char* key;
  oxcart_vector_t* filepaths;
  char filepath[OXCART_FILEPATH_LENGTH];

  _m.config.fontmap = oxcart_hashmap_create(oxcart_hashstr, oxcart_comparestr, 8);

  L = oxcart_lua_newstate();

  if (OXCART_LUA_DOFILE(L, "/config/oxcart_text.lua")) {
    OXCART_ASSERT(!"OXCART_LUA_DOFILE() failed");
  }

  lua_getglobal(L, "oxcart_text");
  if (!lua_istable(L, -1)) {
    OXCART_ASSERT(!"lua_istable() failed");
  }

  t = lua_gettop(L);
  lua_pushnil(L);

  /* for-each font */
  while (lua_next(L, t)) {
    if (!lua_istable(L, -1)) {
      OXCART_ASSERT(!"lua_istable() failed");
    }

    if (OXCART_STYLE_LENGTH != lua_objlen(L, -1)) {
      OXCART_ASSERT(!"Invalid number of filepaths");
    }

    filepaths = oxcart_vector_create(OXCART_FILEPATH_LENGTH, OXCART_STYLE_LENGTH);

    /* for-each filepath */
    for (i = 1; i <= OXCART_STYLE_LENGTH; i++) {
      lua_rawgeti(L, -1, i);

      if (!lua_isstring(L, -1)) {
        OXCART_ASSERT(!"lua_isstring() failed");
      }

      strncpy_s(filepath, OXCART_FILEPATH_LENGTH, lua_tostring(L, -1), _TRUNCATE);
      oxcart_vector_pushback(filepaths, filepath);

      lua_pop(L, 1);
    }

    /* use the font name as the key */
    if (!(key = _strdup(lua_tostring(L, -2)))) {
      OXCART_ASSERT(!"_strdup() failed");
    }

    if (oxcart_hashmap_insert(_m.config.fontmap, key, filepaths)) {
      OXCART_ASSERT(!"Duplicate font");
    }

    lua_pop(L, 1);
  }

  lua_close(L);
}

/**
 * 
 */
static void _module_loadshader()
{
  GLuint shader[2];

  shader[0] = oxcart_shader_createwithfile("/shader/oxcart_text.vert", GL_VERTEX_SHADER);
  shader[1] = oxcart_shader_createwithfile("/shader/oxcart_text.frag", GL_FRAGMENT_SHADER);
  _m.shader.program = oxcart_program_create(shader, OXCART_ARRAY_SIZE(shader));
  oxcart_shader_destroy(shader, OXCART_ARRAY_SIZE(shader));

  glBindAttribLocation(_m.shader.program, OXCART_SHADER_ATTRIBLOC_VERTEX, "vertex");
  glBindAttribLocation(_m.shader.program, OXCART_SHADER_ATTRIBLOC_TEXCOORD, "texcoord");
  glBindAttribLocation(_m.shader.program, OXCART_SHADER_ATTRIBLOC_COLOR, "color");

  oxcart_program_link(_m.shader.program);

  _m.shader.camera = glGetUniformBlockIndex(_m.shader.program, "camera");
  _m.shader.model = glGetUniformLocation(_m.shader.program, "model");
  _m.shader.sampler = glGetUniformLocation(_m.shader.program, "sampler");

  glUniformBlockBinding(_m.shader.program, _m.shader.camera, OXCART_SHADER_BINDPOINT_CAMERA_ORTHO);
  glUniform1i(_m.shader.sampler, 0);
}
