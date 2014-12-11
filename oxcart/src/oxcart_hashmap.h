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

#ifndef OXCART_HASHMAP_H
#define OXCART_HASHMAP_H

#include <string.h>
#include "oxcart_assert.h"
#include "oxcart_util.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct oxcart_hashmap_t oxcart_hashmap_t;

oxcart_hashmap_t* oxcart_hashmap_create(size_t (*hash)(const void*), int (*compare)(const void*, const void*), int capacity);
void oxcart_hashmap_destroy(oxcart_hashmap_t* hashmap);
int oxcart_hashmap_capacity(oxcart_hashmap_t* hashmap);
int oxcart_hashmap_empty(oxcart_hashmap_t* hashmap);
int oxcart_hashmap_size(oxcart_hashmap_t* hashmap);
int oxcart_hashmap_collisions(oxcart_hashmap_t* hashmap);
void* oxcart_hashmap_item(oxcart_hashmap_t* hashmap, const void* key);
void* oxcart_hashmap_insert(oxcart_hashmap_t* hashmap, const void* key, void* item);
void* oxcart_hashmap_erase(oxcart_hashmap_t* hashmap, const void* key);
int oxcart_hashmap_contains(oxcart_hashmap_t* hashmap, const void* key);
void oxcart_hashmap_foreach(oxcart_hashmap_t* hashmap, int (*callback)(const void* key, void* item, void* userdata), void* userdata);

/**
 * FNV-1a hash function
 */
OXCART_INLINE size_t oxcart_hashkey(const unsigned char* key, size_t size)
{
  size_t i;
  size_t hash = 2166136261u;

  OXCART_ASSERT(key);
  OXCART_ASSERT(size > 0);

  for (i = 0; i < size; i++) {
    hash ^= (size_t)(key[i]);
    hash *= 16777619u;
  }

  return(hash);
}

/**
 * 
 */
OXCART_INLINE size_t oxcart_hashint(const void* key)
{
  OXCART_ASSERT(key);

  return(*((int*)key));
}

/**
 * 
 */
OXCART_INLINE size_t oxcart_hashstr(const void* key)
{
  OXCART_ASSERT(key);

  return(oxcart_hashkey(key, strlen(key)));
}

/**
 * 
 */
OXCART_INLINE int oxcart_compareint(const void* key1, const void* key2)
{
  int val1;
  int val2;

  OXCART_ASSERT(key1);
  OXCART_ASSERT(key2);

  val1 = *((int*)key1);
  val2 = *((int*)key2);

  if (val1 < val2) {
    return(-1);
  }

  if (val1 > val2) {
    return(1);
  }

  return(0);
}

/**
 * 
 */
OXCART_INLINE int oxcart_comparestr(const void* key1, const void* key2)
{
  OXCART_ASSERT(key1);
  OXCART_ASSERT(key2);

  return(strcmp(key1, key2));
}

#ifdef __cplusplus
}
#endif

#endif
