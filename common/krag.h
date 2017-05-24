/* 
 * krag.h - header file for 'krag' module
 * 
 * The krag module contains all the logic for saving krags to a hashtable,
 * getting the secret string, revealing characters, and providing clues to krags.
 * Given a filename, the module will read the file and store the krags in a hashtable
 * where the key is the kragID and the item is a krag struct. 
 *
 *
 * GREP
 */

#ifndef __KRAG_H
#define __KRAG_H

#include <stdio.h>
#include <stdbool.h>
#include "../libcs50/hashtable.h"
#include "team.h"



/**************** global types ****************/
typedef struct krag {  
  double latitude;   //latitude of the krag location
  double longitude;  //longitude of the krag location
  char clue[141];	//a string representing the clue to the krag
  int n;  //number according to the order in the kragfile       
} krag_t;
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
* This function reveal more characters to a team's current string. It takes the kragID for the krag found,
* teamname whose string is to be releaved, the secret string, the hashtable of the teams, and the hashtable of the krags.
* If a team calls the function for the first time, then this will set the current string of the team to be
* a series of '_' and set the number of claimed krags to zero. The function normally retuns 0. It
* only returns 1 when the team has claimed all krags. 
*/
int revealCharacters(char * kragID, char * teamname, char * secret, hashtable_t * teamhash, hashtable_t * kraghash);

/*
* This function computes the total number of krags. It takes the hashtable of krags as paramenter. 
* If kraghash is NULL, then 0 is returned.
*/
int totalKrags(hashtable_t * kraghash);

/*
* This funtion deletes the krag struct
*/
void deleteKragHash(hashtable_t * kraghash);

/*
* This function prints the kraghash with all its components.
*/
void printKrags(hashtable_t * kraghash);

/*
* This function store a random first clue in a given team. This function should be called
* at the start of the game where a team is given one clue for a random krag.
*/
void firstClue(char * teamname, hashtable_t * kraghash, hashtable_t * teamhash);

/*
* THis function stores two clues for two random krags that the team has not found yet.
* THis funciton should be called when a team find a krag.
*/
void randomClue(char * teamname, hashtable_t * kraghash, hashtable_t * teamhash);


#endif // __KRAG_H