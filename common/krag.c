/* 
 * krag.c -  'krag' module
 *
 * see krag.h for more information.
 *
 * GREP
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <time.h>
#include "krag.h"
#include "../libcs50/hashtable.h"
#include "../libcs50/set.h"
#include "../libcs50/file.h"
#include "team.h"

#define HASH_SIZE 50
#define ARRAY_SIZE 20


// typedef struct krag {  
//   double latitude;   //latitude of the krag location
//   double longitude;  //longitude of the krag location
//   char clue[141];	//a string representing the clue to the krag       
// } krag_t;

unsigned int MAX_HEX;
char * array [ARRAY_SIZE];


hashtable_t * readKrag(char * filename){

	FILE *fp;
 	fp = fopen(filename, "r");

 	hashtable_t * kraghash = hashtable_new(50);

 	//unsigned int curr_hex; //current hex code
 	int x = 0;


 	for (int i =0; i<ARRAY_SIZE; i++) {
		array[i] = NULL;
	}

 	//while is not end of file
 	while (!feof(fp)){
 		char * string = readlinep(fp);  //read a line
 		char * tok;
 		char kragID[5]; //kragID of the krag

 		krag_t *krag = malloc(sizeof(krag_t));

 		//pull of the latitude from the string and assign it to the krag struct
 		tok = strtok(string, "|");
    	tok = tok + strlen("latitude=");
    	krag->latitude = atof(tok);

    	//pull of the longitude from the string and assign it to the krag struct
    	tok = strtok(NULL, "|");
    	tok = tok + strlen("longitude=");
    	krag->longitude = atof(tok);

    	//pull of the kragID from the string and copy it to the kragID
    	tok = strtok (NULL, "|");
    	tok = tok + strlen("kragId=");
		strcpy(kragID, tok);
		array[x] = malloc(5);
		strcpy(array[x],tok);

		//pull of the clue from the string and assign it to the krag struct
    	tok = strtok(NULL, "|");
    	tok = tok + strlen("clue=");
    	strcpy(krag->clue,tok);

    	//insert the krag in the hashtable
    	hashtable_insert(kraghash, kragID, krag);

    	free(string);
    	x++;
 	}


 	fclose(fp);
 	return kraghash;
}


char * getSecretString(char * filename){

	FILE *fp;
 	fp = fopen(filename, "r");

 	char * secret = readlinep(fp);
 	fclose(fp);
 	return secret;
}


int getSecretStringLen(char * string){
	 return strlen(string);
}


int revealCharacters(char * teamname, char * secret, hashtable_t * teamhash, hashtable_t * kraghash){

	team_t * team = hashtable_find(teamhash, teamname);

	//if the team calls this function for the first time
	if (!(team->revealedString)){

		team->revealedString = realloc(team->revealedString, strlen(secret) + 1);
		char c = '_'; 
		char * string = malloc(strlen(secret) + 1);
		int x = 0;

		//create a string with all undescores '_' and with the size of the secret string
		for (x = 0; x < strlen(secret); x++){
			string[x] = c;
		}

		string[x] = '\0';  //add null character

		//assign it to the revealedString (AKA current string) of the team
		strcpy(team->revealedString,string);
		team->claimed = 0;  //set the number of claimed krags to zero

		free(string);
	}
	else{
		int nkrags = totalKrags(kraghash); //total number of krags in the game

		//reveal characters
		for (int x = 0; x < strlen(secret); x++){
			if (x % nkrags == (team->claimed)){
				(team->revealedString)[x] = secret[x];
			}
		}

		team->claimed = team->claimed + 1;

		//return 1 if team has claimed all krags
		if (team->claimed == nkrags){
			return 1;
		}
	}
	return 0;
}

//helper funciton to compute the total number of krags
static void computeSize(void *arg, const char *key, void *item){
	int *size = (int *)arg;
	(*size)++;
}

int totalKrags(hashtable_t * kraghash){

	if (kraghash != NULL){
		int size=0; 
		hashtable_iterate(kraghash, &size, computeSize);
		return size;
	}
	return 0;
}

static void deleteKrag(void *item){
  if (item) {
    free(item);
  }
}

void deleteKragHash(hashtable_t * kraghash){

	hashtable_delete(kraghash, deleteKrag);
			//free the array
		for (int i=0; i<ARRAY_SIZE; i++) {
			if (array[i] != NULL) {
				free(array[i]);
			}
		}
}

//helper function to print the krags
static void kragPrint(FILE *fp, const char *key, void *item)
{
  krag_t * krag = item;
  if (krag != NULL){
    fprintf(fp, "latitude=%f|longitude=%f|kragId=%s|clue=%s\n", krag->latitude, krag->longitude, key, krag->clue);
   	}
}

void printKrags(hashtable_t * kraghash){
	hashtable_print(kraghash, stdout, kragPrint);
}


void firstClue(hashtable_t * kraghash){

	srand(time(NULL)); 
	int r = rand() % totalKrags(kraghash);
	char kragID[5];

	for (int i = 0; i < ARRAY_SIZE; i++){
		if (i == r){
			strcpy(kragID, array[i]);
			break;

		}
	}
	printf("kragID: %s", kragID);

	krag_t * krag = hashtable_find(kraghash, kragID);

	return krag->clue;
}

void randomClue(char * teamname, hashtable_t * kraghash, hashtable_t * teamhash){


	getClue(teamname, kraghash, teamhash);
	getClue(teamname, kraghash, teamhash);

}


void getClueOne(char * teamname, hashtable_t * kraghash, hashtable_t * teamhash){

	srand(time(NULL)); 
	int r = rand() % totalKrags(kraghash);
	char kragID[5];

	set_t * krags = getKrags(teamname, teamhash);
	set_t * clues = getClues(teamname, teamhash);

	for (int i = 0; i < ARRAY_SIZE; i++){
		if (i == r && set_find(krags, array[i] == NULL)){
			strcpy(kragID, array[i]);
			break;
		}
	}

	krag_t * krag = hashtable_find(kraghash, kragID);


	set_insert(clues, kragID, krag->clue);

	team_t * team = hashtable_find(teamhash, teamname);

	team->recentClues


}







