#ifndef _asn_01_Hashtable_h
#define _asn_01_Hashtable_h

#include <stdint.h>
#include <stdlib.h>

#define HASHSIZE 100

static struct HashNode* hashtab[HASHSIZE]; /* pointer table */

struct HashNode
{
  struct HashNode* next;
  char* name;
  char* value;
};

struct HashNode*
Hashtable_get(char* s);

struct HashNode*
Hashtable_put(char* name, char* value);

#endif