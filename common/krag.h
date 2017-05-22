/* 
 * krag.h - header file for 'krag' module
 * 
 *
 *
 * GREP
 */

#ifndef __KRAG_H
#define __KRAG_H

#include <stdio.h>
#include <stdbool.h>
#include "team.h"



/**************** global types ****************/
typedef struct krag krag_t;  // opaque to users of the module
/**************** functions ****************/

/*
* This functions reads a kragfile and creates a hashtable where the kragID is the key and the item
* is a krag struct. Caller is rensposible for freeing the pointer
*/
hashtable_t * readKrag(char * filename);

/*
* This functions reads a secretfile and returns the secret string. 
* Caller is rensposible for freeing the pointer
*/
char * getSecretString(char * filename);

/*
* This functions returns the lenght of the secret string
*/
int getSecretStringLen(char * string);


/*
* This function reveal more characters to a team's current string. It takes the teamname whose
* string is to be releaved, the secret string, the hashtable of the teams, and the hashtable of the krags.
* If a team calls the function for the first time, then this will set the current string of the team to be
* a series of '_' and set the number of claimed krags to zero. The function normally retuns 0. It
* only returns 1 when the team has claimed all krags. 
*/
int revealCharacters(char * teamname, char * secret, hashtable_t * teamhash, hashtable_t * kraghash);

/*
* This function computes the total number of krags. It takes the hashtable of krags as paramenter. 
* If kraghash is NULL, then 0 is returned.
*/
int totalKrags(hashtable_t * kraghash);

/*
* This funtion deletes the krag struct. It shuold be called with hashtable_delete(kraghash, deleteKrag)
*/
void deleteKrag(void *item);


/*
* This function prints the kraghash with all its components.
*/
void printKrags(hashtable_t * kraghash);

/*
*
*/
char * getClue(hashtable_t * ht, char * kragID);


#endif // __KRAG_H