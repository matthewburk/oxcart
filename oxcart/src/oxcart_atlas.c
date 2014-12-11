/**
 * Freetype GL - A C OpenGL Freetype engine
 * Platform:    Any
 * WWW:         http://code.google.com/p/freetype-gl/
 * ----------------------------------------------------------------------------
 * Copyright 2011,2012 Nicolas P. Rougier. All rights reserved.
 * 
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 * 
 *  1. Redistributions of source code must retain the above copyright notice,
 *     this list of conditions and the following disclaimer.
 *
 *  2. Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 * 
 * THIS SOFTWARE IS PROVIDED BY NICOLAS P. ROUGIER ''AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO
 * EVENT SHALL NICOLAS P. ROUGIER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
 * INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * The views and conclusions contained in the software and documentation are
 * those of the authors and should not be interpreted as representing official
 * policies, either expressed or implied, of Nicolas P. Rougier.
 * 
 * ============================================================================
 * 
 * This source is based on the article by Jukka Jylanki :
 * "A Thousand Ways to Pack the Bin - A Practical Approach to
 * Two-Dimensional Rectangle Bin Packing", February 27, 2010.
 *
 * More precisely, this is an implementation of the Skyline Bottom-Left
 * algorithm based on C++ sources provided by Jukka Jylanki at:
 * http://clb.demon.fi/files/RectangleBinPack/
 *
 * ============================================================================
 *
 * This source is derived from Freetype GL.  Its license, found above, has been
 * carried forward.  It has been refactored, with several bug fixes, to conform
 * with the oxcart framework.
 */

#include <stdlib.h>
#include <string.h>
#include "oxcart_assert.h"
#include "oxcart_atlas.h"
#include "oxcart_vector.h"

typedef struct oxcart_region_t oxcart_region_t;

struct oxcart_region_t
{
  int x, y, w, h;
};

struct oxcart_atlas_t
{
  unsigned char* data;
  int w, h, depth;
  oxcart_vector_t* nodes;
};

int _atlas_fit(oxcart_atlas_t* atlas, int index, int w, int h, int* y);
void _atlas_merge(oxcart_atlas_t* atlas);

/**
 * 
 */
oxcart_atlas_t* oxcart_atlas_create(int w, int h, int depth)
{
  oxcart_atlas_t* atlas;
  oxcart_region_t node;

  OXCART_ASSERT(w >= 4);
  OXCART_ASSERT(h >= 4);
  OXCART_ASSERT(depth > 0);

  if (!(atlas = (oxcart_atlas_t*)malloc(sizeof(oxcart_atlas_t)))) {
    OXCART_ASSERT(!"malloc() failed");
  }

  if (!(atlas->data = (unsigned char*)calloc(w * h * depth, sizeof(unsigned char)))) {
    OXCART_ASSERT(!"calloc() failed");
  }

  atlas->w = w;
  atlas->h = h;
  atlas->depth = depth;
  atlas->nodes = oxcart_vector_create(sizeof(oxcart_region_t), 1);

  /* leave space for a one pixel border */
  node.x = 1;
  node.y = 1;
  node.w = (w - 2);
  node.h = 0;
  oxcart_vector_pushback(atlas->nodes, &node);

  return(atlas);
}

/**
 * 
 */
void oxcart_atlas_destroy(oxcart_atlas_t* atlas)
{
  OXCART_ASSERT(atlas);
  OXCART_ASSERT(atlas->data);

  oxcart_vector_destroy(atlas->nodes);
  free(atlas->data);
  free(atlas);
}

/**
 * 
 */
void oxcart_atlas_properties(oxcart_atlas_t* atlas, int* w, int* h, int* depth)
{
  OXCART_ASSERT(atlas);
  OXCART_ASSERT(w);
  OXCART_ASSERT(h);
  OXCART_ASSERT(depth);

  *w = atlas->w;
  *h = atlas->h;
  *depth = atlas->depth;
}

/**
 * 
 */
const unsigned char* oxcart_atlas_data(oxcart_atlas_t* atlas)
{
  OXCART_ASSERT(atlas);
  OXCART_ASSERT(atlas->data);

  return(atlas->data);
}

/**
 * 
 */
int oxcart_atlas_reserve(oxcart_atlas_t* atlas, int w, int h, int* x, int* y)
{
  int i;
  int y1;
  int shrink;
  int best_i = -1;
  int best_w = INT_MAX;
  int best_h = INT_MAX;
  oxcart_region_t item;
  oxcart_region_t* node;
  oxcart_region_t* prev;

  OXCART_ASSERT(atlas);
  OXCART_ASSERT(w > 0);
  OXCART_ASSERT(w <= (atlas->w - 2));
  OXCART_ASSERT(h > 0);
  OXCART_ASSERT(h <= (atlas->h - 2));
  OXCART_ASSERT(x);
  OXCART_ASSERT(y);

  for (i = 0; i < oxcart_vector_size(atlas->nodes); i++) {
    node = (oxcart_region_t*)oxcart_vector_item(atlas->nodes, i);

    if ((node->x + w) > (atlas->w - 1)) {
      continue;
    }

    if (_atlas_fit(atlas, i, w, h, &y1)) {
      if (((y1 + h) < best_h) || (((y1 + h) == best_h) && (node->w < best_w))) {
        best_i = i;
        best_w = node->w;
        best_h = (y1 + h);
        *x = node->x;
        *y = y1;
      }
    }
  }

  if (best_i == -1) {
    return(0);
  }

  item.x = *x;
  item.y = (*y + h);
  item.w = w;
  item.h = 0;
  oxcart_vector_insert(atlas->nodes, best_i, &item);

  best_i++;

  for (i = best_i; i < oxcart_vector_size(atlas->nodes); i++) {
    node = (oxcart_region_t*)oxcart_vector_item(atlas->nodes, i);
    prev = (oxcart_region_t*)oxcart_vector_item(atlas->nodes, i - 1);

    if (node->x < (prev->x + prev->w)) {
      shrink = (prev->x + prev->w) - node->x;
      node->x += shrink;
      node->w -= shrink;

      if (node->w <= 0) {
        oxcart_vector_erase(atlas->nodes, i);
        i--;
      }
      else {
        break;
      }
    }
    else {
      break;
    }
  }

  _atlas_merge(atlas);

  return(1);
}

/**
 * 
 */
void oxcart_atlas_fill(oxcart_atlas_t* atlas, int x, int y, int w, int h, int stride, const unsigned char* data)
{
  int i;
  int csize = sizeof(unsigned char);

  OXCART_ASSERT(atlas);
  OXCART_ASSERT(x > 0);
  OXCART_ASSERT(x < (atlas->w - 1));
  OXCART_ASSERT(y > 0);
  OXCART_ASSERT(y < (atlas->h - 1));
  OXCART_ASSERT(w > 0);
  OXCART_ASSERT((x + w) <= (atlas->w - 1));
  OXCART_ASSERT(h > 0);
  OXCART_ASSERT((y + h) <= (atlas->h - 1));
  OXCART_ASSERT(stride >= 0);
  OXCART_ASSERT(data);

  for (i = 0; i < h; i++) {
    memcpy(atlas->data + ((((y + i) * atlas->w) + x) * atlas->depth * csize), data + ((i * stride) * csize), w * atlas->depth * csize);
  }
}

/**
 * 
 */
void oxcart_atlas_clear(oxcart_atlas_t* atlas)
{
  oxcart_region_t node;

  OXCART_ASSERT(atlas);
  OXCART_ASSERT(atlas->data);

  oxcart_vector_clear(atlas->nodes);

  /* leave space for a one pixel border */
  node.x = 1;
  node.y = 1;
  node.w = (atlas->w - 2);
  node.h = 0;
  oxcart_vector_pushback(atlas->nodes, &node);

  memset(atlas->data, 0, atlas->w * atlas->h * atlas->depth);
}

/**
 * 
 */
int _atlas_fit(oxcart_atlas_t* atlas, int index, int w, int h, int* y)
{
  int remainder;
  oxcart_region_t* node;

  OXCART_ASSERT(atlas);
  OXCART_ASSERT(y);

  *y = -1;
  remainder = w;

  while (remainder > 0) {
    node = (oxcart_region_t*)oxcart_vector_item(atlas->nodes, index);

    if (node->y > *y) {
      *y = node->y;
    }

    if ((*y + h) > (atlas->h - 1)) {
      return(0);
    }

    remainder -= node->w;
    index++;
  }

  return(1);
}

/**
 * 
 */
void _atlas_merge(oxcart_atlas_t* atlas)
{
  int i;
  oxcart_region_t* node;
  oxcart_region_t* next;

  OXCART_ASSERT(atlas);

  for (i = 0; i < (oxcart_vector_size(atlas->nodes) - 1); i++) {
    node = (oxcart_region_t*)oxcart_vector_item(atlas->nodes, i);
    next = (oxcart_region_t*)oxcart_vector_item(atlas->nodes, i + 1);

    if (node->y == next->y) {
      node->w += next->w;
      oxcart_vector_erase(atlas->nodes, i + 1);
      i--;
    }
  }
}
