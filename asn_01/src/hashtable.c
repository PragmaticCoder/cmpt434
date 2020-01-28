#undef NDEBUG

#include <dbg.h>
#include <stdlib.h>

struct HashNode
{
  struct HashNode* next;
  char* name;
  char* value;
};

#define HASHSIZE 101
static struct HashNode* hashtab[HASHSIZE]; /* pointer table */

char*
_copy(char* s) /* make a duplicate of s */
{
  char* p;
  p = (char*)malloc(strlen(s) + 1); /* +1 for ’\0’ */
  if (p != NULL)
    strcpy(p, s);
  return p;
}

/* _hash: form hash value for string s */
unsigned
_hash(char* s)
{
  unsigned hash_value;
  for (hash_value = 0; *s != '\0'; s++)
    hash_value = *s + 31 * hash_value;
  return hash_value % HASHSIZE;
}

/* get: look for s in hashtab */
struct HashNode*
get(char* s)
{
  struct HashNode* np;

  for (np = hashtab[_hash(s)]; np != NULL; np = np->next)
    if (strcmp(s, np->name) == 0)
      return np; /* found */

  return NULL; /* not found */
}

struct HashNode*
put(char* name, char* value)
{
  struct HashNode* np;
  unsigned hash_value;
  if ((np = get(name)) == NULL) { /* not found */
    np = (struct HashNode*)malloc(sizeof(*np));

    if (np == NULL || (np->name = _copy(name)) == NULL)
      return NULL;

    hash_value = _hash(name);
    np->next = hashtab[hash_value];
    hashtab[hash_value] = np;
  } else {                  /* already there */
    free((void*)np->value); /*free previous value */
  }

  if ((np->value = _copy(value)) == NULL)
    return NULL;

  return np;
}