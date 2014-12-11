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

#ifndef OXCART_ATLAS_H
#define OXCART_ATLAS_H

#ifdef __cplusplus
extern "C" {
#endif

typedef struct oxcart_atlas_t oxcart_atlas_t;

oxcart_atlas_t* oxcart_atlas_create(int w, int h, int depth);
void oxcart_atlas_destroy(oxcart_atlas_t* atlas);
void oxcart_atlas_properties(oxcart_atlas_t* atlas, int* w, int* h, int* depth);
const unsigned char* oxcart_atlas_data(oxcart_atlas_t* atlas);
int oxcart_atlas_reserve(oxcart_atlas_t* atlas, int w, int h, int* x, int* y);
void oxcart_atlas_fill(oxcart_atlas_t* atlas, int x, int y, int w, int h, int stride, const unsigned char* data);
void oxcart_atlas_clear(oxcart_atlas_t* atlas);

#ifdef __cplusplus
}
#endif

#endif
