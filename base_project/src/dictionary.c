#include "dictionary.h"

dictionary_t *head = NULL;
dictionary_t *tail = NULL;
dictionary_t *current = NULL;

char *strdup(char *str)
{	
	char *dup = (char *)malloc(sizeof(char) * (strlen(str) + 1));
	memset(dup, 0, (strlen(str) + 1));
	strcpy(dup, str);
	return dup;
}

dictionary_t* 
Dictionary_initialize()
{
    dictionary_t* dictionary = (dictionary_t*)malloc(sizeof(dictionary_t));
    if(dictionary == NULL) {
      log_err("Failed to Allocate Memory\n\r");
      return NULL;
    }

    memset(dictionary, 0, sizeof(dictionary_t));
    return dictionary;
}

dictionary_t* 
dictionary_search(char *key)
{
    dictionary_t* dictionary;
  
    for(dictionary = tail; dictionary; dictionary = dictionary->next) {
      if(!strcmp(dictionary->key, key)){
        return dictionary;
        }
    }
    return NULL;
}

void 
Dictionary_freeMemory(dictionary_t* dictionary)
{
    if(dictionary == NULL)
      return;
    free(dictionary->key);
    free(dictionary->value);
    free(dictionary);
}

int 
Dictionary_add(char *key, char *value)
{  
    if(dictionary_search(key) != NULL) {
      return 0;
    }

    dictionary_t* dictionary = Dictionary_initialize();
    if(dictionary == NULL) {
      return 0;    
    }  

    dictionary->key = strdup(key);
    dictionary->value = strdup(value);
  
    dictionary->prev = head;
    dictionary->next = NULL;
  
    if(head != NULL){
      head->next = dictionary;    
    }
    else {
      tail	= dictionary;
      current = tail;
    }

    head = dictionary;
    return 1;
}

int 
Dictionary_getValue(char *key, char* value)
{  
    dictionary_t* dictionary = dictionary_search(key);
    if(dictionary != NULL) {
        strcpy(value, dictionary->value);
        return 1;
    }
    return 0;
}

dictionary_t* 
Dictionary_iter() 
{
    dictionary_t *dictionary = current;
    if(tail == NULL) {
      return NULL;
    }
    if(current == NULL){
      current = tail;
    }
    else {
      current = current->next;
    }
    return dictionary;
}

int 
Dictionary_getAll(char *key, char* value)
{
    dictionary_t *dictionary;
    if((dictionary = Dictionary_iter()) != NULL) {
      strcpy(key, dictionary->key);
      strcpy(value, dictionary->value);
      return 1;
    }
    return  0;
}

int 
Dictionary_remove(char* key)
{
    dictionary_t* dictionary = dictionary_search(key);
    if(dictionary != NULL) {
      if((dictionary == tail) && (dictionary == head)) {
        head = NULL;
        tail = NULL;
        current = NULL;
      }	
      else if(dictionary == tail){
        if(tail == current) {
          current = dictionary->next;
        }
        tail = dictionary->next;
        dictionary->next->prev = NULL;
      }
      else if(dictionary == head){
        if(head == current) {
          current = dictionary->prev;
        }
        head = dictionary->prev;
        dictionary->prev->next = NULL;
      }
      else {	
        dictionary->next->prev = dictionary->prev;
        dictionary->prev->next = dictionary->next;
      }
		
      Dictionary_freeMemory(dictionary);
      return 1;
    }
    return 0;
}


