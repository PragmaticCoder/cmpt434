#ifndef _asn_01_Hashtable_h
#define _asn_01_Hashtable_h

#include <stdint.h>
#include <stdlib.h>

#define HASHSIZE 101

struct HashNode
{
  struct HashNode* next;
  char* name;
  char* value;
};

char*
_Hashtable_copy(char* s);

unsigned
_Hashtable_hash(char* s);

struct HashNode*
_Hashtable_get(char* s);

struct HashNode*
_Hashtable_put(char* name, char* value);

#endif