/* 
 * hashtable.c - CS50 'hashtable' module
 *
 * see hashtable.h for more information.
 *
 * Yifei Yan
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "hashtable.h"
#include "set.h"
#include "jhash.h"
#include "memory.h"

/**************** file-local global variables ****************/
/* none */

/**************** local types ****************/

/**************** global types ****************/
typedef struct hashtable {
  int num_slots;
  set_t **array;         //array of set pointers
}hashtable_t;  // opaque to users of the module


/**************** global functions ****************/
/* that is, visible outside this file */
/* see hashtable.h for comments about exported functions */

/**************** local functions ****************/
/* not visible outside this file */

/**************** hashtable_new() ****************/  //from bag.c module
hashtable_t *hashtable_new(const int num_slots){

  hashtable_t *hashtable = count_malloc(sizeof(hashtable_t));

  if (hashtable == NULL) {
    return NULL; // error allocating hashtable
  } else {
    // initialize contents of hashtable structure

  	hashtable->array = count_malloc(num_slots* sizeof(set_t *));
    for (int i=0; i<num_slots;i++) {
      hashtable->array[i] = NULL;
    }
    hashtable->num_slots = num_slots;
    return hashtable;
 }
}

/**************** hashtable_insert() ****************/  
bool hashtable_insert(hashtable_t *ht, const char *key, void *item){
  if (ht != NULL && item != NULL && key != NULL) {
    // allocate a new node to be added to the list

   int hashkey = JenkinsHash(key, ht->num_slots);
   bool success;    //if item was successfully inserted
   if (ht->array[hashkey] == NULL){
     set_t *new_set = set_new();
      if (new_set == NULL) {
      return false; // error allocating set
       }
     ht->array[hashkey] = new_set;
     success = set_insert(new_set,key,item);
   }
   else{
     success = set_insert(ht->array[hashkey],key,item);
   }

    return success;
  }
  return false;
}

/**************** hashtable_find() ****************/
void *hashtable_find(hashtable_t *ht, const char *key){
  if (ht!= NULL) {
    int hashkey = JenkinsHash(key, ht->num_slots);
    return set_find(ht->array[hashkey],key);
  }
  return NULL;
}

/**************** hashtable_print() ****************/   //from bag.c module
void hashtable_print(hashtable_t *ht, FILE *fp, 
                     void (*itemprint)(FILE *fp, const char *key, void *item)){

  if (fp != NULL) {
    if (ht != NULL) {
      fputc('{', fp);

      for (int x = 0; x < sizeof(ht->array); x++){
        set_t * set = ht->array[x];
        set_print(set, fp, itemprint);
     }
      fputc('}', fp);
    } else {
      fputs("(null)", fp);
    }
  }
}

/**************** hashtable_iterate() ****************/   //from bag.c module
void hashtable_iterate(hashtable_t *ht, void *arg,
               void (*itemfunc)(void *arg, const char *key, void *item) ){

  if (ht != NULL && itemfunc != NULL) {
    // call itemfunc with arg, on each item
    for (int x = 0; x < sizeof(ht->array); x++){
      set_t * set = ht->array[x];
      set_iterate(set, arg, itemfunc);
    }
  }
}

/**************** hashtable_delete() ****************/  //from bag.c module
void hashtable_delete(hashtable_t *ht, void (*itemdelete)(void *item) ){

  if (ht != NULL) {
    for (int x = 0; x < ht->num_slots; x++){
      if (ht->array[x] != NULL) {
        set_delete(ht->array[x], itemdelete);
      }
    }
    count_free(ht->array);
    count_free(ht);
  }
}

