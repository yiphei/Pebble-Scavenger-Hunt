/* 
 * set.c - CS50 'set' module
 *
 * see set.h for more information.
 *
 * Yifei Yan
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "set.h"
#include "memory.h"


/**************** file-local global variables ****************/
/* none */

/**************** local types ****************/

typedef struct setnode {  
  void *item;		      
  struct setnode *next;
  char* key;	      
} setnode_t;

/**************** global types ****************/
typedef struct set {
  struct setnode *head;	      // head of the list of items in set
}set_t;  // opaque to users of the module


/**************** global functions ****************/
/* that is, visible outside this file */
/* see set.h for comments about exported functions */

/**************** local functions ****************/
/* not visible outside this file */

static setnode_t *setnode_new(const char  *key, void *item);

/**************** set_new() ****************/  //from bag.c module
set_t *set_new(void){
  set_t *set = count_malloc(sizeof(set_t)); //allocate new memory

  if (set == NULL) {
    return NULL; // error allocating set
  } else {
    // initialize contents of set structure
    set->head = NULL;
    return set;
 }
}

/**************** set_insert() ****************/  //from bag.c module
bool set_insert(set_t *set,const char *key, void *item){
  if (set != NULL && item != NULL && key!= NULL && set_find(set,key) == NULL) {
    // allocate a new node to be added to the list
    setnode_t *new = setnode_new(key,item);
    if (new != NULL) {
      // add it to the head of the list
      new->next = set->head;
      set->head = new;
      return true;
    }
  }
  return false;

}

//helper function to create a new node in set
/*************** setnode_new() ****************/ //from bag.c module
static setnode_t *setnode_new(const char *key,void *item){

  setnode_t *node = count_malloc(sizeof(setnode_t));

  if (node == NULL) {
    // error allocating memory for node; return error
    return NULL;
  } else {
    node->key = count_malloc(strlen(key)+1); 
    if (node->key == NULL){
      return NULL;
    }
    node->item = item;
    strcpy(node->key,key);
    node->next = NULL;
    return node;
  }
}
/**************** set_find() ****************/
void *set_find(set_t *set, const char *key){

	if (set == NULL || key == NULL || set->head == NULL){
		return NULL;
	}
	setnode_t * current_node = set->head;

  while (current_node != NULL) {
    //if keys match
    if (strcmp(current_node->key, key) == 0) {
      return current_node->item;
    }
    current_node = current_node->next;
  }
  return NULL;
}


/**************** set_print() ****************/   //from bag.c module
void set_print(set_t *set, FILE *fp, 
               void (*itemprint)(FILE *fp, const char *key, void *item) ){
  if (fp != NULL) {
    if (set != NULL) {
      fputc('{', fp);
      for (setnode_t *node = set->head; node != NULL; node = node->next) {
	// print this node
		if (itemprint != NULL) {  // print the node's item 
	 	 (*itemprint)(fp, node->key, node->item); 
		  fputc(',', fp);
		}
      }
      fputc('}', fp);
    } else {
      fputs("(null)", fp);
    }
  }
}

/**************** set_iterate() ****************/   //from bag.c module
void set_iterate(set_t *set, void *arg,
                 void (*itemfunc)(void *arg, const char *key, void *item) ){

  if (set != NULL && itemfunc != NULL) {
    // call itemfunc with arg, on each item
    for (setnode_t *node = set->head; node != NULL; node = node->next) {
      (*itemfunc)(arg, node->key, node->item); 
    }
  }

}

/**************** set_delete() ****************/  //from bag.c module
void set_delete(set_t *set, void (*itemdelete)(void *item) ){

  if (set != NULL) {
    for (setnode_t *node = set->head; node != NULL; ) {
      if (itemdelete != NULL) {		    // if possible...
        (*itemdelete)(node->item);	    // delete node's item
      }
      setnode_t *next = node->next;	    // remember what comes next
      count_free(node->key);
      count_free(node);			    // free the node
      node = next;			    // and move on to next
    }

    count_free(set);
  }

}
