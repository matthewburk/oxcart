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

#ifndef OXCART_TEXT_H
#define OXCART_TEXT_H

#include "oxcart_math.h"
#include "oxcart_util.h"

#ifdef __cplusplus
extern "C" {
#endif

#define OXCART_TEXT_STYLE_REGULAR     0x00
#define OXCART_TEXT_STYLE_BOLD        0x01
#define OXCART_TEXT_STYLE_ITALIC      0x02
#define OXCART_TEXT_STYLE_BOLD_ITALIC 0x03
#define OXCART_TEXT_STYLE_KERNING     0x10

typedef struct oxcart_markup_t oxcart_markup_t;
typedef struct oxcart_metrics_t oxcart_metrics_t;
typedef struct oxcart_text_t oxcart_text_t;

struct oxcart_markup_t
{
  char* name;
  int size;
  int style;
  float line;
  oxcart_vec4_t color;
};

struct oxcart_metrics_t
{
  float w, h;
  float bearing;
  float ascent;
  float descent;
  float advance;
  float baseline;
};

oxcart_text_t* oxcart_text_create();
void oxcart_text_destroy(oxcart_text_t* text);
void oxcart_text_metrics(oxcart_text_t* text, const oxcart_markup_t* markup, const char* str, int length, oxcart_metrics_t* metrics);
void oxcart_text_assign(oxcart_text_t* text, const oxcart_markup_t* markup, const char* str, int length, oxcart_vec2_t* pen);
void oxcart_text_append(oxcart_text_t* text, const oxcart_markup_t* markup, const char* str, int length, oxcart_vec2_t* pen);
void oxcart_text_clear(oxcart_text_t* text);
void oxcart_text_draw(oxcart_text_t* text, const oxcart_mat4_t* mvp);

/**
 * 
 */
OXCART_INLINE oxcart_markup_t oxcart_markup_defaults()
{
  oxcart_markup_t markup;

  markup.name = "firasans";
  markup.size = 16;
  markup.style = OXCART_TEXT_STYLE_REGULAR;
  markup.line = 0.0f;
  markup.color = oxcart_vec4_set(1.0f, 1.0f, 1.0f, 1.0f);

  return(markup);
}

#ifdef __cplusplus
}
#endif

#endif
