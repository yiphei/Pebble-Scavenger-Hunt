/* 
 * counters.c - CS50 'counters' module
 *
 * see counters.h for more information.
 *
 * Yifei Yan
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "counters.h"
#include "memory.h"

/**************** file-local global variables ****************/
/* none */

/**************** local types ****************/

typedef struct countersnode { 
  int counter;	      
  struct countersnode *next;
  int key;	      
} countersnode_t;

/**************** global types ****************/
typedef struct counters {
  struct countersnode *head;	      // head of the list of items in counters
}counters_t;  // opaque to users of the module

/**************** global functions ****************/
/* that is, visible outside this file */
/* see counters.h for comments about exported functions */

/**************** local functions ****************/
/* not visible outside this file */
static countersnode_t *countersnode_new(const int key);
static countersnode_t * counters_getnode(counters_t * ctrs ,const int key);
static countersnode_t *countersnode_newCount(const int key, int count);

/******************* counters_new() ************/  //from bag.c module
counters_t *counters_new(void){

  counters_t *ctrs = count_malloc(sizeof(counters_t));

  if (ctrs == NULL) {
    return NULL; // error allocating counters
  } else {
    // initialize contents of counters structure
    ctrs->head = NULL;
    return ctrs;
 }
}

/******************* counters_add() **************/  //from bag.c module
void counters_add(counters_t *ctrs, const int key){
  if (ctrs == NULL || key < 0) {
    return;
  }

  if (counters_get(ctrs,key) == 0) {
    // allocate a new node to be added to the list
    countersnode_t *new = countersnode_new(key);
    if (new != NULL) {
      // add it to the head of the list

      new->next = ctrs->head;
      ctrs->head = new;
    }
  } else {
  	countersnode_t * node = counters_getnode(ctrs,key);
  	node->counter += 1;
  }
}

//helper funciton that returns a node given a key
/************ counters_getnode() *****************/
static countersnode_t * counters_getnode(counters_t * ctrs ,const int key){

	countersnode_t * current_node = ctrs->head;
	while (current_node->key != key){
    ///if keys match
		if ((current_node = current_node->next) == NULL){
			return NULL; //return null if key is not found
		}
	}
	return current_node;
}

//helper function that creates a new node given a key
/*************** countersnode_new() ****************/ //from bag.c module
static countersnode_t *countersnode_new(const int key){

  countersnode_t *node = count_malloc(sizeof(countersnode_t));

  if (node == NULL) {
    // error allocating memory for node; return error
    return NULL;
  } else {

  	node->counter = 1;
    node->key = key;
    node->next = NULL;
    return node;
  }
}

static countersnode_t *countersnode_newCount(const int key, int count){

  countersnode_t *node = count_malloc(sizeof(countersnode_t));

  if (node == NULL) {
    // error allocating memory for node; return error
    return NULL;
  } else {

    node->counter = count;
    node->key = key;
    node->next = NULL;
    return node;
  }
}

/***************** counters_get() ****************/
int counters_get(counters_t *ctrs, const int key){

	if (ctrs == NULL || key < 0){
		return 0;
	}
  countersnode_t * current_node = ctrs->head;

  while (current_node != NULL) {
    //if keys match
    if (current_node->key == key) {
      return current_node->counter;
    }
    current_node = current_node->next;
  }
  return 0;
}

/******************* counters_set() ****************/
void counters_set(counters_t *ctrs, const int key, int count){
	if (key >= 0 && ctrs != NULL && count >= 0){

		countersnode_t * node = counters_getnode(ctrs,key);

    if (node != NULL){
      node->counter = count;

    }
    else{
          countersnode_t *new = countersnode_newCount(key,count);
           if (new != NULL) {
      // add it to the head of the list

            new->next = ctrs->head;
            ctrs->head = new;
          }

    }	
  }
}

/*************** counters_print() **********************/  //from bag.c module
void counters_print(counters_t *ctrs, FILE *fp){

  if (fp != NULL) {
    if (ctrs != NULL) {
      fputc('{', fp);
      for (countersnode_t *node = ctrs->head; node != NULL; node = node->next) {
	// print this node
		fprintf(fp, "[%d] = %d; ", node->key, node->counter);
      }
      fputc('}', fp);
    } else {
      fputs("(null)", fp);
    }
  }

}

/************** counters_iterate() ******************/  //from bag.c module
void counters_iterate(counters_t *ctrs, void *arg, 
                      void (*itemfunc)(void *arg, const int key, int count)){

  if (ctrs != NULL && itemfunc != NULL) {
    // call itemfunc with arg, on each item
    for (countersnode_t *node = ctrs->head; node != NULL; node = node->next) {
      (*itemfunc)(arg, node->key, node->counter); 
    }
  }

}


/********** counters_delete() *************/  //from bag.c module
void counters_delete(counters_t *ctrs){

  if (ctrs != NULL) {
    for (countersnode_t *node = ctrs->head; node != NULL; ) {
      
      countersnode_t *next = node->next;	    // remember what comes next
      count_free(node);			    // free the node
      node = next;			    // and move on to next
    }

    count_free(ctrs);
  }
}