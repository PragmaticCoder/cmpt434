#include "map.h"
#include <stdlib.h>
#include <string.h>

struct Map_node_t
{
  unsigned hash;
  void* value;
  Map_node_t* next;
};

static unsigned
Map_hash(const char* str)
{
  unsigned hash = 5381;
  while (*str)
    hash = ((hash << 5) + hash) ^ *str++;

  return hash;
}

static Map_node_t*
Map_newnode(const char* key, void* value, int vsize)
{
  Map_node_t* node;
  int ksize = strlen(key) + 1;
  int voffset = ksize + ((sizeof(void*) - ksize) % sizeof(void*));
  node = malloc(sizeof(*node) + voffset + vsize);
  if (!node)
    return NULL;
  memcpy(node + 1, key, ksize);
  node->hash = Map_hash(key);
  node->value = ((char*)(node + 1)) + voffset;
  memcpy(node->value, value, vsize);
  return node;
}

static int
Map_bucketidx(Map_base_t* m, unsigned hash)
{
  /* If the implementation is changed to allow a non-power-of-2 bucket count,
   * the line below should be changed to use mod instead of AND */
  return hash & (m->nbuckets - 1);
}

static void
Map_addnode(Map_base_t* m, Map_node_t* node)
{
  int n = Map_bucketidx(m, node->hash);
  node->next = m->buckets[n];
  m->buckets[n] = node;
}

static int
Map_resize(Map_base_t* m, int nbuckets)
{
  Map_node_t *nodes, *node, *next;
  Map_node_t** buckets;
  int i;
  /* Chain all nodes together */
  nodes = NULL;
  i = m->nbuckets;
  while (i--) {
    node = (m->buckets)[i];
    while (node) {
      next = node->next;
      node->next = nodes;
      nodes = node;
      node = next;
    }
  }
  /* Reset buckets */
  buckets = realloc(m->buckets, sizeof(*m->buckets) * nbuckets);
  if (buckets != NULL) {
    m->buckets = buckets;
    m->nbuckets = nbuckets;
  }
  if (m->buckets) {
    memset(m->buckets, 0, sizeof(*m->buckets) * m->nbuckets);
    /* Re-add nodes to buckets */
    node = nodes;
    while (node) {
      next = node->next;
      Map_addnode(m, node);
      node = next;
    }
  }
  /* Return error code if realloc() failed */
  return (buckets == NULL) ? -1 : 0;
}

static Map_node_t**
Map_getref(Map_base_t* m, const char* key)
{
  unsigned hash = Map_hash(key);
  Map_node_t** next;
  if (m->nbuckets > 0) {
    next = &m->buckets[Map_bucketidx(m, hash)];
    while (*next) {
      if ((*next)->hash == hash && !strcmp((char*)(*next + 1), key)) {
        return next;
      }
      next = &(*next)->next;
    }
  }
  return NULL;
}

void
Map_deinit_(Map_base_t* m)
{
  Map_node_t *next, *node;
  int i;
  i = m->nbuckets;
  while (i--) {
    node = m->buckets[i];
    while (node) {
      next = node->next;
      free(node);
      node = next;
    }
  }
  free(m->buckets);
}

void*
Map_get_(Map_base_t* m, const char* key)
{
  Map_node_t** next = Map_getref(m, key);
  return next ? (*next)->value : NULL;
}

int
Map_set_(Map_base_t* m, const char* key, void* value, int vsize)
{
  int n, err;
  Map_node_t **next, *node;
  /* Find & replace existing node */
  next = Map_getref(m, key);
  if (next) {
    memcpy((*next)->value, value, vsize);
    return 0;
  }
  /* Add new node */
  node = Map_newnode(key, value, vsize);
  if (node == NULL)
    goto fail;
  if (m->nnodes >= m->nbuckets) {
    n = (m->nbuckets > 0) ? (m->nbuckets << 1) : 1;
    err = Map_resize(m, n);
    if (err)
      goto fail;
  }
  Map_addnode(m, node);
  m->nnodes++;
  return 0;
fail:
  if (node)
    free(node);
  return -1;
}

void
Map_remove_(Map_base_t* m, const char* key)
{
  Map_node_t* node;
  Map_node_t** next = Map_getref(m, key);
  if (next) {
    node = *next;
    *next = (*next)->next;
    free(node);
    m->nnodes--;
  }
}

Map_iter_t
Map_iter_(void)
{
  Map_iter_t iter;
  iter.bucketidx = -1;
  iter.node = NULL;
  return iter;
}

const char*
Map_next_(Map_base_t* m, Map_iter_t* iter)
{
  if (iter->node) {
    iter->node = iter->node->next;
    if (iter->node == NULL)
      goto nextBucket;
  } else {
  nextBucket:
    do {
      if (++iter->bucketidx >= m->nbuckets) {
        return NULL;
      }
      iter->node = m->buckets[iter->bucketidx];
    } while (iter->node == NULL);
  }
  return (char*)(iter->node + 1);
}