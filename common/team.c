/* 
 * team.c -  'team' module
 *
 * see team.h for more information.
 *
 * GREP
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "krag.h"
#include "../libcs50/hashtable.h"
#include "../libcs50/set.h"
#include "../libcs50/file.h"
#include "team.h"

char * getRevealedString(char * teamname, hashtable_t * teamhash){
	team_t * team = hashtable_find(teamhash, teamname); 
	return team->revealedString;
}

hashtable_t * init(void){
	hashtable_t * teamhash = hashtable_new(50);  //initialize a new hashtable of teams
	return teamhash;
}

void addFieldAgent(char * name, char * teamname, char * gameID, hashtable_t * teamhash){

	//create the team if it does not exist
	if (hashtable_find(teamhash, teamname) == NULL){
		team_t * newteam = newTeam();
		hashtable_insert(teamhash, teamname, newteam);
	}

	team_t * newteam = hashtable_find(teamhash, teamname); 
	fieldAgent_t * newFA = newFieldAgent(gameID);  //initialize a new field agent
	set_insert(newteam->FAset, name, newFA);
}

int addGuideAgent(char * guideID, char * teamname, char * name, char * gameID, hashtable_t * teamhash){

	//create the team if it does not exists
	if (hashtable_find(teamhash, teamname) == NULL){
		team_t * newteam = newTeam();
		hashtable_insert(teamhash, teamname, newteam);
	}

	//if the funciton already has a guide agent
	if ( ((team_t *)(hashtable_find(teamhash, teamname)))->guideAgent != NULL){
		return 1;
	}

	//create a new guide agent and assign it to the team
	team_t * newteam = hashtable_find(teamhash, teamname);
	guideAgent_t * newGA = newGuideAgent(guideID, name , gameID);
	newteam->guideAgent = newGA;

	return 0;
}

void addKrag(char * teamname, char * kragID, krag_t * krag, hashtable_t * teamhash ){

	team_t * team = hashtable_find(teamhash, teamname);
	set_insert(team->krags, kragID, krag);
}


set_t * getKrags(char * teamname, hashtable_t * teamhash){

	team_t * team = hashtable_find(teamhash, teamname);
	return team->krags;
}

set_t * getClues(char * teamname, hashtable_t * teamhash){

	team_t * team = hashtable_find(teamhash, teamname);
	return team->clues;
}

char * getClueOne(char * teamname, hashtable_t * teamhash){

	team_t * team = hashtable_find(teamhash, teamname);
	return team->recentClues[0];
}

char * getClueTwo(char * teamname, hashtable_t * teamhash){

	team_t * team = hashtable_find(teamhash, teamname);
	return team->recentClues[1];
}



void updateLocation(char * name, char * teamname, double longitude, double latitude, hashtable_t * teamhash){

	team_t * team = hashtable_find(teamhash, teamname);
	//process only if team and field agent exist
	if (team != NULL && set_find(team->FAset, name) != NULL){
		fieldAgent_t * FA = set_find(team->FAset, name);
		FA->latitude = latitude;
		FA->longitude = longitude;
	}
}

//Helper function to free field agents
static void deleteFA(void * item){
	if (item){

		free(((fieldAgent_t *) item)->gameID);
	}
}

//helper functino to free krags
static void deleteKrags(void * item){
	if (item){
		free(item);
	}
}

static void deleteTeam(void *item)
{
  if ((team_t *)item != NULL) {
  	free(((team_t *)item)->revealedString);
  	free((((team_t *)item)->guideAgent)->guideID);
  	free((((team_t *)item)->guideAgent)->name);
  	free((((team_t *)item)->guideAgent)->gameID);
  	free(((team_t *)item)->guideAgent);
  	set_delete(((team_t *)item)->FAset, deleteFA);
  	set_delete(((team_t *)item)->krags, deleteKrags);
  	set_delete(((team_t *)item)->clues, deleteKrags);

  				//free the array
	for (int i=0; i<2; i++) {
		if (((team_t *)item)->recentClues[i] != NULL) {
			free(((team_t *)item)->recentClues[i]);
		}
	}
  	free((team_t *)item);
  }
}

void deleteTeamHash(hashtable_t * teamhash){

	if (teamhash != NULL){
		hashtable_delete(teamhash, deleteTeam);
	}
}


fieldAgent_t * newFieldAgent(char * gameID){
	fieldAgent_t * fa = malloc(sizeof(fieldAgent_t));
  	if (fa == NULL) {
    	return NULL; // error allocating field agent
  	} 
  	else {
  	fa->gameID = malloc(strlen(gameID)+1);
  	strcpy(fa->gameID,gameID);

    return fa;
 }
}


guideAgent_t * newGuideAgent(char * guideID, char * name, char * gameID){

	guideAgent_t * ga = malloc(sizeof(guideAgent_t));
  	if (ga == NULL) {
    	return NULL; // error allocating guide agent
  	} else {

    // initialize contents of guide agent structure
  	ga->name = malloc(strlen(name)+1);
  	ga->guideID = malloc(strlen(guideID)+1);
  	ga->gameID = malloc(strlen(gameID)+1);  
  	strcpy(ga->name,name);
  	strcpy(ga->guideID,guideID);
  	strcpy(ga->gameID,gameID);

    return ga;
 }
}


team_t * newTeam(void){

  team_t * team = malloc(sizeof(team_t));
  if (team == NULL) {
    return NULL; // error allocating team
  } else {
    // initialize contents of team structure
    team->revealedString = NULL;
  	team->FAset = set_new();
  	team->guideAgent = NULL;
  	team->krags = set_new();
  	team->clues = set_new();

  	for (int i =0; i<2; i++) {
		team->recentClues[i] = NULL;
	}
    return team;
 }
}

static void FAPrint(FILE *fp, const char *key, void *item)
{
  fieldAgent_t * fa = item;
  if (fa != NULL){
    fprintf(fp, "Name: %s\n", key);
   	}
}

static void teamPrint(FILE *fp, const char *key, void *item)
{
  team_t * team = item;
  if (team != NULL){
    fprintf(fp, "Team name: %s\n", key);
    fprintf(fp, "Guide agent name: %s    guideID: %s\n", (team->guideAgent)->name, (team->guideAgent)->guideID);
    fprintf(fp, "Field agents:");
    set_print(team->FAset, stdout, FAPrint);
   	}
}


void printTeams(hashtable_t * teamhash){

	hashtable_print(teamhash, stdout, teamPrint);
}


