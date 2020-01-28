#undef NDEBUG

#include "hashtable.h"
#include "dbg.h"

char*
_Hashtable_copy(char* s) /* make a duplicate of s */
{
  char* p;
  p = (char*)malloc(strlen(s) + 1); /* +1 for ’\0’ */
  if (p != NULL)
    strcpy(p, s);
  return p;
}

/* _Hashtable_hash: form hash value for string s */
unsigned
_Hashtable_hash(char* s)
{
  unsigned hash_value;
  for (hash_value = 0; *s != '\0'; s++)
    hash_value = *s + 31 * hash_value;
  return hash_value % HASHSIZE;
}

/* Hashtable_get: look for s in hashtab */
HashNode_t*
Hashtable_get(char* s)
{
  struct HashNode* np;

  for (np = hashtab[_Hashtable_hash(s)]; np != NULL; np = np->next)
    if (strcmp(s, np->name) == 0)
      return np; /* found */

  return NULL; /* not found */
}

HashNode_t*
Hashtable_put(char* name, char* value)
{
  struct HashNode* np;
  unsigned hash_value;
  if ((np = Hashtable_get(name)) == NULL) { /* not found */
    np = (struct HashNode*)malloc(sizeof(*np));

    if (np == NULL || (np->name = _Hashtable_copy(name)) == NULL)
      return NULL;

    hash_value = _Hashtable_hash(name);
    np->next = hashtab[hash_value];
    hashtab[hash_value] = np;
  } else {                  /* already there */
    free((void*)np->value); /*free previous value */
  }

  if ((np->value = _Hashtable_copy(value)) == NULL)
    return NULL;

  return np;
}