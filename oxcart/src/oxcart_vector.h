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

#ifndef OXCART_VECTOR_H
#define OXCART_VECTOR_H

#ifdef __cplusplus
extern "C" {
#endif

typedef struct oxcart_vector_t oxcart_vector_t;

oxcart_vector_t* oxcart_vector_create(int itemsize, int capacity);
void oxcart_vector_destroy(oxcart_vector_t* vector);
int oxcart_vector_capacity(oxcart_vector_t* vector);
int oxcart_vector_empty(oxcart_vector_t* vector);
int oxcart_vector_size(oxcart_vector_t* vector);
void oxcart_vector_reserve(oxcart_vector_t* vector, int capacity);
void oxcart_vector_resize(oxcart_vector_t* vector, int capacity);
void oxcart_vector_shrink(oxcart_vector_t* vector);
void oxcart_vector_clear(oxcart_vector_t* vector);
void* oxcart_vector_item(oxcart_vector_t* vector, int index);
void* oxcart_vector_front(oxcart_vector_t* vector);
void* oxcart_vector_back(oxcart_vector_t* vector);
void* oxcart_vector_pushback(oxcart_vector_t* vector, const void* item);
void* oxcart_vector_pushbackv(oxcart_vector_t* vector, const void* items, int count);
void* oxcart_vector_insert(oxcart_vector_t* vector, int index, const void* item);
void* oxcart_vector_insertv(oxcart_vector_t* vector, int index, const void* items, int count);
void* oxcart_vector_replace(oxcart_vector_t* vector, int index, const void* item);
void oxcart_vector_popback(oxcart_vector_t* vector);
void oxcart_vector_erase(oxcart_vector_t* vector, int index);
void oxcart_vector_eraserange(oxcart_vector_t* vector, int first, int last);
int oxcart_vector_contains(oxcart_vector_t* vector, const void* item, int (*compare)(const void*, const void*));
void oxcart_vector_sort(oxcart_vector_t* vector, int (*compare)(const void*, const void*));

#ifdef __cplusplus
}
#endif

#endif
