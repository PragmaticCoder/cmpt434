/**
 * Name: Alvi Akbar
 * NSID: ala273
 * Student Number: 11118887
 */
#ifndef __dictionary_h__
#define __dictionary_h__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "dbg.h"

typedef struct _dictionary {
  char *key;
  char *value;
  struct _dictionary *next;
  struct _dictionary *prev;
}dictionary_t;

int Dictionary_add(char *key, char *value);
int Dictionary_getValue(char *key, char* value);
int Dictionary_getAll(char *key, char* value);
int Dictionary_remove(char* key);

int Command_interface(int argc, char *argv[], char *retBuffer);

#endif
