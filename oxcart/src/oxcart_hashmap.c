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
#include "oxcart_hashmap.h"

typedef struct oxcart_node_t oxcart_node_t;

struct oxcart_node_t
{
  size_t hash;
  const void* key;
  void* item;
  oxcart_node_t* next;
};

struct oxcart_hashmap_t
{
  size_t (*hash)(const void*);
  int (*compare)(const void*, const void*);
  oxcart_node_t** buckets;
  int buckcnt;
  int size;
};

static void _hashmap_expand(oxcart_hashmap_t* hashmap);
static int _hashmap_index(oxcart_hashmap_t* hashmap, size_t hash);
static int _hashmap_compare(oxcart_hashmap_t* hashmap, const void* k1, size_t h1, const void* k2, size_t h2);

/**
 * 
 */
oxcart_hashmap_t* oxcart_hashmap_create(size_t (*hash)(const void*), int (*compare)(const void*, const void*), int capacity)
{
  int buckcnt = 1;
  oxcart_hashmap_t* hashmap;

  OXCART_ASSERT(hash);
  OXCART_ASSERT(compare);
  OXCART_ASSERT(capacity > 0);

  if (!(hashmap = (oxcart_hashmap_t*)malloc(sizeof(oxcart_hashmap_t)))) {
    OXCART_ASSERT(!"malloc() failed");
  }

  /* 0.75 load factor; buckcnt must be a power of 2 */
  capacity = capacity * 4 / 3;
  while (buckcnt <= capacity) {
    buckcnt <<= 1;
  }

  if (!(hashmap->buckets = (oxcart_node_t**)calloc(buckcnt, sizeof(oxcart_node_t*)))) {
    OXCART_ASSERT(!"calloc() failed");
  }

  hashmap->hash = hash;
  hashmap->compare = compare;
  hashmap->buckcnt = buckcnt;
  hashmap->size = 0;

  return(hashmap);
}

/**
 * 
 */
void oxcart_hashmap_destroy(oxcart_hashmap_t* hashmap)
{
  int i;
  oxcart_node_t* node;
  oxcart_node_t* next;

  OXCART_ASSERT(hashmap);
  OXCART_ASSERT(hashmap->buckets);

  for (i = 0; i < hashmap->buckcnt; i++) {
    node = hashmap->buckets[i];

    while (node) {
      next = node->next;
      free(node);
      node = next;
    }
  }

  free(hashmap->buckets);
  free(hashmap);
}

/**
 * 
 */
int oxcart_hashmap_capacity(oxcart_hashmap_t* hashmap)
{
  OXCART_ASSERT(hashmap);

  return(hashmap->buckcnt * 3 / 4);
}

/**
 * 
 */
int oxcart_hashmap_empty(oxcart_hashmap_t* hashmap)
{
  OXCART_ASSERT(hashmap);

  return(!!hashmap->size);
}

/**
 * 
 */
int oxcart_hashmap_size(oxcart_hashmap_t* hashmap)
{
  OXCART_ASSERT(hashmap);

  return(hashmap->size);
}

/**
 * 
 */
int oxcart_hashmap_collisions(oxcart_hashmap_t* hashmap)
{
  int i;
  int collisions = 0;
  oxcart_node_t* node;

  OXCART_ASSERT(hashmap);

  for (i = 0; i < hashmap->buckcnt; i++) {
    node = hashmap->buckets[i];

    while (node) {
      if (node->next) {
        collisions++;
      }

      node = node->next;
    }
  }

  return(collisions);
}

/**
 * 
 */
void* oxcart_hashmap_item(oxcart_hashmap_t* hashmap, const void* key)
{
  size_t hash;
  int index;
  oxcart_node_t* node;

  OXCART_ASSERT(hashmap);
  OXCART_ASSERT(key);

  hash = hashmap->hash(key);
  index = _hashmap_index(hashmap, hash);
  node = hashmap->buckets[index];

  while (node) {
    if (_hashmap_compare(hashmap, key, hash, node->key, node->hash)) {
      return(node->item);
    }

    node = node->next;
  }

  return(0);
}

/**
 * 
 */
void* oxcart_hashmap_insert(oxcart_hashmap_t* hashmap, const void* key, void* item)
{
  size_t hash;
  int index;
  void* old;
  oxcart_node_t** list;

  OXCART_ASSERT(hashmap);
  OXCART_ASSERT(key);
  OXCART_ASSERT(item);

  hash = hashmap->hash(key);
  index = _hashmap_index(hashmap, hash);
  list = &(hashmap->buckets[index]);

  while (*list) {
    if (_hashmap_compare(hashmap, key, hash, (*list)->key, (*list)->hash)) {
      old = (*list)->item;
      (*list)->item = item;

      return(old);
    }

    list = &(*list)->next;
  }

  if (!(*list = (oxcart_node_t*)malloc(sizeof(oxcart_node_t)))) {
    OXCART_ASSERT(!"malloc() failed");
  }

  (*list)->hash = hash;
  (*list)->key = key;
  (*list)->item = item;
  (*list)->next = 0;

  hashmap->size++;
  _hashmap_expand(hashmap);

  return(0);
}

/**
 * 
 */
void* oxcart_hashmap_erase(oxcart_hashmap_t* hashmap, const void* key)
{
  size_t hash;
  int index;
  void* item;
  oxcart_node_t** list;
  oxcart_node_t* node;

  OXCART_ASSERT(hashmap);
  OXCART_ASSERT(key);

  hash = hashmap->hash(key);
  index = _hashmap_index(hashmap, hash);
  list = &(hashmap->buckets[index]);

  while (*list) {
    if (_hashmap_compare(hashmap, key, hash, (*list)->key, (*list)->hash)) {
      node = *list;
      item = node->item;
      *list = node->next;
      free(node);
      hashmap->size--;

      return(item);
    }

    list = &(*list)->next;
  }

  return(0);
}

/**
 * 
 */
int oxcart_hashmap_contains(oxcart_hashmap_t* hashmap, const void* key)
{
  size_t hash;
  int index;
  oxcart_node_t* node;

  OXCART_ASSERT(hashmap);
  OXCART_ASSERT(key);

  hash = hashmap->hash(key);
  index = _hashmap_index(hashmap, hash);
  node = hashmap->buckets[index];

  while (node) {
    if (_hashmap_compare(hashmap, key, hash, node->key, node->hash)) {
      return(1);
    }

    node = node->next;
  }

  return(0);
}

/**
 * 
 */
void oxcart_hashmap_foreach(oxcart_hashmap_t* hashmap, int (*callback)(const void* key, void* item, void* userdata), void* userdata)
{
  int i;
  oxcart_node_t* node;

  OXCART_ASSERT(hashmap);
  OXCART_ASSERT(callback);

  for (i = 0; i < hashmap->buckcnt; i++) {
    node = hashmap->buckets[i];

    while (node) {
      if (!callback(node->key, node->item, userdata)) {
        return;
      }

      node = node->next;
    }
  }
}

/**
 * 
 */
static void _hashmap_expand(oxcart_hashmap_t* hashmap)
{
  int i;
  int index;
  int buckcnt;
  oxcart_node_t** buckets;
  oxcart_node_t* node;
  oxcart_node_t* next;

  OXCART_ASSERT(hashmap);

  /* if the load factor exceeds 0.75 */
  if (hashmap->size > (hashmap->buckcnt * 3 / 4)) {
    buckcnt = hashmap->buckcnt << 1;

    if (!(buckets = (oxcart_node_t**)calloc(buckcnt, sizeof(oxcart_node_t*)))) {
      OXCART_ASSERT(!"calloc() failed");
    }

    /* for each node, calculate new index and move to new bucket list */
    for (i = 0; i < hashmap->buckcnt; i++) {
      node = hashmap->buckets[i];

      while (node) {
        next = node->next;
        index = (buckcnt - 1) & node->hash;
        node->next = buckets[index];
        buckets[index] = node;
        node = next;
      }
    }

    free(hashmap->buckets);
    hashmap->buckets = buckets;
    hashmap->buckcnt = buckcnt;
  }
}

/**
 * 
 */
static int _hashmap_index(oxcart_hashmap_t* hashmap, size_t hash)
{
  int index;

  OXCART_ASSERT(hashmap);

  index = hash & (hashmap->buckcnt - 1);

  if (index >= hashmap->buckcnt) {
    index -= ((hashmap->buckcnt - 1) >> 1) + 1;
  }

  return(index);
}

/**
 * 
 */
static int _hashmap_compare(oxcart_hashmap_t* hashmap, const void* k1, size_t h1, const void* k2, size_t h2)
{
  OXCART_ASSERT(hashmap);
  OXCART_ASSERT(k1);
  OXCART_ASSERT(k2);

  if (k1 == k2) {
    return(1);
  }

  if (h1 != h2) {
    return(0);
  }

  if (0 == hashmap->compare(k1, k2)) {
    return(1);
  }

  return(0);
}
