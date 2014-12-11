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

#include <ft2build.h>
#include FT_FREETYPE_H
#include FT_STROKER_H
#include <physfs.h>
#include "oxcart_assert.h"
#include "oxcart_app.h"
#include "oxcart_font.h"
#include "oxcart_util.h"
#include "oxcart_vector.h"

#define OXCART_26DOT6 64
#define OXCART_HRES 100

typedef struct oxcart_module_t oxcart_module_t;

struct oxcart_font_t
{
  char filename[OXCART_FILEPATH_LENGTH];
  int size;
  float line;
  oxcart_atlas_t* atlas;
  oxcart_vector_t* glyphs;
  float height;
  float ascent;
  float descent;
  float advance;
};

struct oxcart_glyph_t
{
  float w, h;
  float bearing_x;
  float bearing_y;
  float advance;
  float s0, t0;
  float s1, t1;
  oxcart_vector_t* kerning;
};

struct oxcart_module_t
{
  int codecnt;
  const char* codes;
};

static void _font_loadglyphs(oxcart_font_t* font);

static oxcart_module_t _m = {96, " !\"#$%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_`abcdefghijklmnopqrstuvwxyz{|}~"};

/**
 * 
 */
oxcart_font_t* oxcart_font_create(const char* filename, int size, float line, oxcart_atlas_t* atlas)
{
  oxcart_font_t* font;

  OXCART_ASSERT(filename);
  OXCART_ASSERT(size >= 8);
  OXCART_ASSERT(line >= 0.0f);
  OXCART_ASSERT(atlas);

  if (!(font = (oxcart_font_t*)malloc(sizeof(oxcart_font_t)))) {
    OXCART_ASSERT(!"malloc() failed");
  }

  strncpy_s(font->filename, OXCART_FILEPATH_LENGTH, filename, _TRUNCATE);
  font->size = size;
  font->line = line;
  font->atlas = atlas;
  font->glyphs = oxcart_vector_create(sizeof(oxcart_glyph_t), _m.codecnt);

  _font_loadglyphs(font);

  return(font);
}

/**
 * 
 */
void oxcart_font_destroy(oxcart_font_t* font)
{
  int i;
  oxcart_glyph_t* glyph;

  OXCART_ASSERT(font);

  for (i = 0; i < oxcart_vector_size(font->glyphs); i++) {
    glyph = (oxcart_glyph_t*)oxcart_vector_item(font->glyphs, i);
    oxcart_vector_destroy(glyph->kerning);
  }

  oxcart_vector_destroy(font->glyphs);
  free(font);
}

/**
 * 
 */
const char* oxcart_font_properties(oxcart_font_t* font, int* size, float* line)
{
  OXCART_ASSERT(font);
  OXCART_ASSERT(size);
  OXCART_ASSERT(line);

  *size = font->size;
  *line = font->line;

  return(font->filename);
}

/**
 * 
 */
oxcart_atlas_t* oxcart_font_atlas(oxcart_font_t* font)
{
  OXCART_ASSERT(font);

  return(font->atlas);
}

/**
 * 
 */
void oxcart_font_metrics(oxcart_font_t* font, float* height, float* ascent, float* descent, float* advance)
{
  OXCART_ASSERT(font);
  OXCART_ASSERT(height);
  OXCART_ASSERT(ascent);
  OXCART_ASSERT(descent);
  OXCART_ASSERT(advance);

  *height = font->height;
  *ascent = font->ascent;
  *descent = font->descent;
  *advance = font->advance;
}

/**
 * 
 */
oxcart_glyph_t* oxcart_font_glyph(oxcart_font_t* font, char code)
{
  OXCART_ASSERT(font);
  OXCART_ASSERT(code >= _m.codes[0]);

  return((oxcart_glyph_t*)oxcart_vector_item(font->glyphs, code - _m.codes[0]));
}

/**
 * 
 */
void oxcart_glyph_metrics(oxcart_glyph_t* glyph, float* w, float* h, float* bearing_x, float* bearing_y, float* advance)
{
  OXCART_ASSERT(glyph);
  OXCART_ASSERT(w);
  OXCART_ASSERT(h);
  OXCART_ASSERT(bearing_x);
  OXCART_ASSERT(bearing_y);
  OXCART_ASSERT(advance);

  *w = glyph->w;
  *h = glyph->h;
  *bearing_x = glyph->bearing_x;
  *bearing_y = glyph->bearing_y;
  *advance = glyph->advance;
}

/**
 * 
 */
float oxcart_glyph_kerning(oxcart_glyph_t* glyph, char code)
{
  OXCART_ASSERT(glyph);
  OXCART_ASSERT(code >= _m.codes[0]);

  return(*(float*)oxcart_vector_item(glyph->kerning, code - _m.codes[0]));
}

/**
 * 
 */
void oxcart_glyph_texcoords(oxcart_glyph_t* glyph, float* s0, float* t0, float* s1, float* t1)
{
  OXCART_ASSERT(glyph);
  OXCART_ASSERT(s0);
  OXCART_ASSERT(t0);
  OXCART_ASSERT(s1);
  OXCART_ASSERT(t1);

  *s0 = glyph->s0;
  *t0 = glyph->t0;
  *s1 = glyph->s1;
  *t1 = glyph->t1;
}

/**
 * 
 */
static void _font_loadglyphs(oxcart_font_t* font)
{
  int i, k;
  int dpi_x, dpi_y;
  int atl_w, atl_h, atl_d;
  int x, y, w, h;
  float kerning;
  unsigned char* buffer;
  oxcart_glyph_t glyph;
  PHYSFS_File* file;
  PHYSFS_sint64 size;
  FT_Library ft_library;
  FT_Face ft_face;
  FT_Matrix ft_matrix;
  FT_UInt ft_curr, ft_prev;
  FT_Int32 ft_flags;
  FT_Stroker ft_stroker;
  FT_Glyph ft_glyph;
  FT_Bitmap ft_bitmap;
  FT_Int ft_left, ft_top;
  FT_Vector ft_vector;

  OXCART_ASSERT(font);

  oxcart_atlas_properties(font->atlas, &atl_w, &atl_h, &atl_d);
  OXCART_ASSERT(atl_d == 1);

  if (!(file = PHYSFS_openRead(font->filename))) {
    OXCART_ASSERT(!"PHYSFS_openRead() failed");
  }

  size = PHYSFS_fileLength(file);
  OXCART_ASSERT(size > 0);

  if (!(buffer = (unsigned char*)malloc((size_t)size))) {
    OXCART_ASSERT(!"malloc() failed");
  }

  if (size > PHYSFS_read(file, buffer, 1, (size_t)size)) {
    OXCART_ASSERT(!"PHYSFS_read() failed");
  }

  if (FT_Init_FreeType(&ft_library)) {
    OXCART_ASSERT(!"FT_Init_FreeType() failed");
  }

  if (FT_New_Memory_Face(ft_library, buffer, (FT_Long)size, 0, &ft_face)) {
    OXCART_ASSERT(!"FT_New_Face() failed");
  }

  if (FT_Select_Charmap(ft_face, FT_ENCODING_UNICODE)) {
    OXCART_ASSERT(!"FT_Select_Charmap() failed");
  }

  oxcart_monitor_dpi(&dpi_x, &dpi_y);

  if (FT_Set_Char_Size(ft_face, 0, font->size * OXCART_26DOT6, dpi_x * OXCART_HRES, dpi_y)) {
    OXCART_ASSERT(!"FT_Set_Char_Size() failed");
  }

  ft_matrix.xx = (int)((1.0f / OXCART_HRES) * 0x10000L);
  ft_matrix.xy = (int)((0.0f) * 0x10000L);
  ft_matrix.yx = (int)((0.0f) * 0x10000L);
  ft_matrix.yy = (int)((1.0f) * 0x10000L);
  FT_Set_Transform(ft_face, &ft_matrix, 0);

  ft_flags = (font->line > 0.0f) ? FT_LOAD_NO_BITMAP : FT_LOAD_RENDER;

  for (i = 0; i < _m.codecnt; i++) {
    ft_curr = FT_Get_Char_Index(ft_face, _m.codes[i]);

    if (FT_Load_Glyph(ft_face, ft_curr, ft_flags | FT_LOAD_FORCE_AUTOHINT)) {
      OXCART_ASSERT(!"FT_Load_Glyph() failed");
    }

    if (font->line > 0.0f) {
      if (FT_Stroker_New(ft_library, &ft_stroker)) {
        OXCART_ASSERT(!"FT_Stroker_New() failed");
      }

      FT_Stroker_Set(ft_stroker, (FT_Fixed)(font->line * OXCART_26DOT6), FT_STROKER_LINECAP_ROUND, FT_STROKER_LINEJOIN_ROUND, 0);

      if (FT_Get_Glyph(ft_face->glyph, &ft_glyph)) {
        OXCART_ASSERT(!"FT_Get_Glyph() failed");
      }

      if (FT_Glyph_Stroke(&ft_glyph, ft_stroker, 1)) {
        OXCART_ASSERT(!"FT_Glyph_Stroke() failed");
      }

      if (FT_Glyph_To_Bitmap(&ft_glyph, FT_RENDER_MODE_LIGHT, 0, 1)) {
        OXCART_ASSERT(!"FT_Glyph_To_Bitmap() failed");
      }

      ft_bitmap = ((FT_BitmapGlyph)ft_glyph)->bitmap;
      ft_left = ((FT_BitmapGlyph)ft_glyph)->left;
      ft_top = ((FT_BitmapGlyph)ft_glyph)->top;

      FT_Stroker_Done(ft_stroker);
    }
    else {
      ft_bitmap = ft_face->glyph->bitmap;
      ft_left = ft_face->glyph->bitmap_left;
      ft_top = ft_face->glyph->bitmap_top;
    }

    x = 0;
    y = 0;
    w = ft_bitmap.width;
    h = ft_bitmap.rows;

    if ((w > 0) && (h > 0) && (ft_bitmap.buffer)) {
      /* add one to w and h to separate each glyph in the atlas */
      if (!oxcart_atlas_reserve(font->atlas, w + 1, h + 1, &x, &y)) {
        OXCART_ASSERT(!"oxcart_atlas_reserve() failed");
      }

      oxcart_atlas_fill(font->atlas, x, y, w, h, ft_bitmap.pitch, ft_bitmap.buffer);
    }

    glyph.w = (float)w;
    glyph.h = (float)h;
    glyph.bearing_x = (float)ft_left;
    glyph.bearing_y = -(float)ft_top;
    glyph.s0 = x / (float)atl_w;
    glyph.t0 = y / (float)atl_h;
    glyph.s1 = (x + w) / (float)atl_w;
    glyph.t1 = (y + h) / (float)atl_h;
    glyph.kerning = oxcart_vector_create(sizeof(float), _m.codecnt);

    /* discard hinting before retrieving advance */
    if (FT_Load_Glyph(ft_face, ft_curr, FT_LOAD_RENDER | FT_LOAD_NO_HINTING)) {
      OXCART_ASSERT(!"FT_Load_Glyph() failed");
    }

    glyph.advance = ft_face->glyph->advance.x / (float)OXCART_26DOT6;

    for (k = 0; k < _m.codecnt; k++) {
      ft_prev = FT_Get_Char_Index(ft_face, _m.codes[k]);

      if (FT_Get_Kerning(ft_face, ft_prev, ft_curr, FT_KERNING_UNFITTED, &ft_vector)) {
        OXCART_ASSERT(!"FT_Get_Kerning() failed");
      }

      kerning = ft_vector.x / (float)(OXCART_26DOT6 * OXCART_26DOT6);
      oxcart_vector_pushback(glyph.kerning, &kerning);
    }

    oxcart_vector_pushback(font->glyphs, &glyph);

    if (font->line > 0.0f) {
      FT_Done_Glyph(ft_glyph);
    }
  }

  font->height = (float)(ft_face->size->metrics.height >> 6);
  font->ascent = -(float)(ft_face->size->metrics.ascender >> 6);
  font->descent = -(float)(ft_face->size->metrics.descender >> 6);
  font->advance = (float)(ft_face->size->metrics.max_advance >> 6) / OXCART_26DOT6;

  FT_Done_Face(ft_face);
  FT_Done_FreeType(ft_library);
  free(buffer);
  PHYSFS_close(file);
}
