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
_copy(char* s);

unsigned
_hash(char* s);

struct HashNode*
get(char* s);

struct HashNode*
put(char* name, char* value);

#endif