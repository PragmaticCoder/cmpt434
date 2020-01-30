/* This is inherited from my previous project used in CMPT 332 assignment */

#ifndef _asn_01_MAP_H
#define _asn_01_MAP_H

#include <string.h>
#include "dbg.h"

struct Map_node_t;
typedef struct Map_node_t Map_node_t;

typedef struct {
  Map_node_t **buckets;
  unsigned nbuckets, nnodes;
} Map_base_t;

typedef struct {
  unsigned bucketidx;
  Map_node_t *node;
} Map_iter_t;


#define Map_t(T)\
  struct { Map_base_t base; T *ref; T tmp; }


#define Map_init(m)\
  memset(m, 0, sizeof(*(m)))


#define Map_deinit(m)\
  Map_deinit_(&(m)->base)


#define Map_get(m, key)\
  ( (m)->ref = Map_get_(&(m)->base, key) )


#define Map_set(m, key, value)\
  ( (m)->tmp = (value),\
    Map_set_(&(m)->base, key, &(m)->tmp, sizeof((m)->tmp)) )


#define Map_remove(m, key)\
  Map_remove_(&(m)->base, key)


#define Map_iter(m)\
  Map_iter_()


#define Map_next(m, iter)\
  Map_next_(&(m)->base, iter)


void Map_deinit_(Map_base_t *m);
void *Map_get_(Map_base_t *m, const char *key);
int Map_set_(Map_base_t *m, const char *key, void *value, int vsize);
void Map_remove_(Map_base_t *m, const char *key);
Map_iter_t Map_iter_(void);
const char *Map_next_(Map_base_t *m, Map_iter_t *iter);


typedef Map_t(void*) Map_void_t;
typedef Map_t(char*) Map_str_t;
typedef Map_t(int) Map_int_t;
typedef Map_t(char) Map_char_t;
typedef Map_t(float) Map_float_t;
typedef Map_t(double) Map_double_t;

#endif