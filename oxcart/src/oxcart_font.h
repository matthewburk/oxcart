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

#ifndef OXCART_FONT_H
#define OXCART_FONT_H

#include "oxcart_atlas.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct oxcart_font_t oxcart_font_t;
typedef struct oxcart_glyph_t oxcart_glyph_t;

OXCART_API oxcart_font_t* oxcart_font_create(const char* filename, int size, float line, oxcart_atlas_t* atlas);
OXCART_API void oxcart_font_destroy(oxcart_font_t* font);
OXCART_API const char* oxcart_font_properties(oxcart_font_t* font, int* size, float* line);
OXCART_API oxcart_atlas_t* oxcart_font_atlas(oxcart_font_t* font);
OXCART_API void oxcart_font_metrics(oxcart_font_t* font, float* height, float* ascent, float* descent, float* advance);
OXCART_API oxcart_glyph_t* oxcart_font_glyph(oxcart_font_t* font, char code);

OXCART_API void oxcart_glyph_metrics(oxcart_glyph_t* glyph, float* w, float* h, float* bearing_x, float* bearing_y, float* advance);
OXCART_API float oxcart_glyph_kerning(oxcart_glyph_t* glyph, char code);
OXCART_API void oxcart_glyph_texcoords(oxcart_glyph_t* glyph, float* s0, float* t0, float* s1, float* t1);

#ifdef __cplusplus
}
#endif

#endif
