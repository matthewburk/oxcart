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
#include "oxcart_vector.h"

struct oxcart_vector_t
{
  void* items;
  int itemsize;
  int capacity;
  int size;
};

/**
 * 
 */
oxcart_vector_t* oxcart_vector_create(int itemsize, int capacity)
{
  oxcart_vector_t* vector;

  OXCART_ASSERT(itemsize > 0);
  OXCART_ASSERT(capacity > 0);

  if (!(vector = (oxcart_vector_t*)malloc(sizeof(oxcart_vector_t)))) {
    OXCART_ASSERT(!"malloc() failed");
  }

  if (!(vector->items = (void*)calloc(capacity, itemsize))) {
    OXCART_ASSERT(!"calloc() failed");
  }

  vector->itemsize = itemsize;
  vector->capacity = capacity;
  vector->size = 0;

  return(vector);
}

/**
 * 
 */
void oxcart_vector_destroy(oxcart_vector_t* vector)
{
  OXCART_ASSERT(vector);
  OXCART_ASSERT(vector->items);

  free(vector->items);
  free(vector);
}

/**
 * 
 */
int oxcart_vector_capacity(oxcart_vector_t* vector)
{
  OXCART_ASSERT(vector);

  return(vector->capacity);
}

/**
 * 
 */
int oxcart_vector_empty(oxcart_vector_t* vector)
{
  OXCART_ASSERT(vector);

  return(!!vector->size);
}

/**
 * 
 */
int oxcart_vector_size(oxcart_vector_t* vector)
{
  OXCART_ASSERT(vector);

  return(vector->size);
}

/**
 * 
 */
void oxcart_vector_reserve(oxcart_vector_t* vector, int capacity)
{
  OXCART_ASSERT(vector);
  OXCART_ASSERT(capacity > 0);

  if (capacity > vector->capacity) {
    if (!(vector->items = (void*)realloc(vector->items, capacity * vector->itemsize))) {
      OXCART_ASSERT(!"realloc() failed");
    }

    vector->capacity = capacity;
  }
}

/**
 * 
 */
void oxcart_vector_resize(oxcart_vector_t* vector, int capacity)
{
  OXCART_ASSERT(vector);
  OXCART_ASSERT(capacity > 0);

  if (capacity > vector->capacity) {
    oxcart_vector_reserve(vector, capacity);
    vector->size = vector->capacity;
  }
  else {
    vector->size = capacity;
  }
}

/**
 * 
 */
void oxcart_vector_shrink(oxcart_vector_t* vector)
{
  OXCART_ASSERT(vector);
  OXCART_ASSERT(vector->size > 0);

  if (vector->capacity > vector->size) {
    if (!(vector->items = (void*)realloc(vector->items, vector->size * vector->itemsize))) {
      OXCART_ASSERT(!"realloc() failed");
    }
  }

  vector->capacity = vector->size;
}

/**
 * 
 */
void oxcart_vector_clear(oxcart_vector_t* vector)
{
  OXCART_ASSERT(vector);

  vector->size = 0;
}

/**
 *
 */
void* oxcart_vector_item(oxcart_vector_t* vector, int index)
{
  OXCART_ASSERT(vector);
  OXCART_ASSERT(vector->size > 0);
  OXCART_ASSERT(index >= 0);
  OXCART_ASSERT(index < vector->size);

  return((unsigned char*)(vector->items) + (index * vector->itemsize));
}

/**
 * 
 */
void* oxcart_vector_front(oxcart_vector_t* vector)
{
  OXCART_ASSERT(vector);
  OXCART_ASSERT(vector->size > 0);

  return(oxcart_vector_item(vector, 0));
}

/**
 * 
 */
void* oxcart_vector_back(oxcart_vector_t* vector)
{
  OXCART_ASSERT(vector);
  OXCART_ASSERT(vector->size > 0);

  return(oxcart_vector_item(vector, vector->size - 1));
}

/**
 * 
 */
void* oxcart_vector_pushback(oxcart_vector_t* vector, const void* item)
{
  OXCART_ASSERT(vector);
  OXCART_ASSERT(item);

  return(oxcart_vector_insert(vector, vector->size, item));
}

/**
 * 
 */
void* oxcart_vector_pushbackv(oxcart_vector_t* vector, const void* items, int count)
{
  void* dst;

  OXCART_ASSERT(vector);
  OXCART_ASSERT(items);
  OXCART_ASSERT(count > 0);

  if ((vector->size + count) > vector->capacity) {
    oxcart_vector_reserve(vector, vector->size + count);
  }

  dst = ((unsigned char*)(vector->items) + (vector->size * vector->itemsize));
  memcpy(dst, items, count * vector->itemsize);

  vector->size += count;

  return(dst);
}

/**
 * 
 */
void* oxcart_vector_insert(oxcart_vector_t* vector, int index, const void* item)
{
  OXCART_ASSERT(vector);
  OXCART_ASSERT(index >= 0);
  OXCART_ASSERT(index <= vector->size);
  OXCART_ASSERT(item);

  if (vector->size >= vector->capacity) {
    oxcart_vector_reserve(vector, vector->capacity * 2);
  }

  if (index < vector->size) {
    memmove((unsigned char*)(vector->items) + ((index + 1) * vector->itemsize),
            (unsigned char*)(vector->items) + ((index + 0) * vector->itemsize),
            (vector->size - index) * vector->itemsize);
  }

  vector->size++;

  return(oxcart_vector_replace(vector, index, item));
}

/**
 * 
 */
void* oxcart_vector_insertv(oxcart_vector_t* vector, int index, const void* items, int count)
{
  void* dst;

  OXCART_ASSERT(vector);
  OXCART_ASSERT(index >= 0);
  OXCART_ASSERT(index <= vector->size);
  OXCART_ASSERT(items);
  OXCART_ASSERT(count > 0);

  if ((vector->size + count) > vector->capacity) {
    oxcart_vector_reserve(vector, vector->size + count);
  }

  if (index < vector->size) {
    memmove((unsigned char*)(vector->items) + ((index + count) * vector->itemsize),
            (unsigned char*)(vector->items) + ((index + 0) * vector->itemsize),
            (vector->size - index) * vector->itemsize);
  }

  dst = ((unsigned char*)(vector->items) + (index * vector->itemsize));
  memcpy(dst, items, count * vector->itemsize);

  vector->size += count;

  return(dst);
}

/**
 * 
 */
void* oxcart_vector_replace(oxcart_vector_t* vector, int index, const void* item)
{
  void* dst;

  OXCART_ASSERT(vector);
  OXCART_ASSERT(vector->size > 0);
  OXCART_ASSERT(index >= 0);
  OXCART_ASSERT(index < vector->size);
  OXCART_ASSERT(item);

  dst = ((unsigned char*)(vector->items) + (index * vector->itemsize));
  memcpy(dst, item, vector->itemsize);

  return(dst);
}

/**
 * 
 */
void oxcart_vector_popback(oxcart_vector_t* vector)
{
  OXCART_ASSERT(vector);
  OXCART_ASSERT(vector->size > 0);

  vector->size--;
}

/**
 * 
 */
void oxcart_vector_erase(oxcart_vector_t* vector, int index)
{
  OXCART_ASSERT(vector);
  OXCART_ASSERT(vector->size > 0);
  OXCART_ASSERT(index >= 0);
  OXCART_ASSERT(index < vector->size);

  oxcart_vector_eraserange(vector, index, index + 1);
}

/**
 * 
 */
void oxcart_vector_eraserange(oxcart_vector_t* vector, int first, int last)
{
  OXCART_ASSERT(vector);
  OXCART_ASSERT(vector->size > 0);
  OXCART_ASSERT(first >= 0);
  OXCART_ASSERT(first < vector->size);
  OXCART_ASSERT(last > first);
  OXCART_ASSERT(last <= vector->size);

  memmove((unsigned char*)(vector->items) + (first * vector->itemsize),
          (unsigned char*)(vector->items) + (last * vector->itemsize),
          (vector->size - last) * vector->itemsize);

  vector->size -= (last - first);
}

/**
 * 
 */
int oxcart_vector_contains(oxcart_vector_t* vector, const void* item, int (*compare)(const void*, const void*))
{
  int i;

  OXCART_ASSERT(vector);
  OXCART_ASSERT(item);
  OXCART_ASSERT(compare);

  for (i = 0; i < vector->size; i++) {
    if (0 == compare(item, oxcart_vector_item(vector, i))) {
      return(1);
    }
  }

  return(0);
}

/**
 * 
 */
void oxcart_vector_sort(oxcart_vector_t* vector, int (*compare)(const void*, const void*))
{
  OXCART_ASSERT(vector);
  OXCART_ASSERT(vector->size > 0);
  OXCART_ASSERT(compare);

  qsort(vector->items, vector->size, vector->itemsize, compare);
}
