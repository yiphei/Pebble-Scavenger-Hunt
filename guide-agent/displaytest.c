

#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <ncurses.h>
#include <string.h>
#include <stdbool.h>
#include "../libcs50/hashtable.h"
#include "../libcs50/set.h"
#include "display.h"
#include "../common/team.h"


int
main(int argc, char *argv[])
{

  // initialize ncurses
  initialize_curses();

  refresh();
  char * string = "this is the string very longg sdasadadsa ds a da dsa d s";
  initializeWindows_I();
  updateString_I(string);


  updateTotalKrags_I(19);


  updateKragsClaimed_I(2);


  hashtable_t * teamhash = initHash();
  addFieldAgent("mark", "12345678", "one", "gameID", NULL, teamhash);
  addFieldAgent("john", "12345678", "one", "gameID", NULL, teamhash);
  addFieldAgent("yifei", "12345678", "one", "gameID", NULL, teamhash);
  updateLocation("mark", "one", 4.5, 5.6, teamhash);
  updateLocation("john", "one", 10.5, 2.6, teamhash);
  updateLocation("yifei", "one", 18.5, 14.6, teamhash);
  set_t *  set = getAllFieldAgents("one", teamhash);

  updateMap_I(set);


  updateLocation("mark", "one", 30.6, 38.7, teamhash);

  updateMap_I(set);
  
  set_t * clues = set_new();

  set_insert(clues, "a", "helloooo");
  set_insert(clues, "b", "helloooo");
  set_insert(clues, "c", "helloooo");
  set_insert(clues, "d", "helloooo");
  set_insert(clues, "e", "helloooo");
  set_insert(clues, "f", "helloooo");
  set_insert(clues, "qwe", "helloooo");
  set_insert(clues, "sdf", "helloooo");
  set_insert(clues, "sfsg", "helloooo");
  set_insert(clues, "dgdtg", "helloooo");
  set_insert(clues, "123", "helloooo");
  set_insert(clues, "432", "helloooo");
  set_insert(clues, "dfgg", "helloooo");
  set_insert(clues, "23rfew", "helloooo");
  set_insert(clues, "wefsgfd", "helloooo");
  set_insert(clues, "rhfti", "helloooo");
  set_insert(clues, "aus", "helloooo");
  set_insert(clues, "sfzg", "helloooo");
  set_insert(clues, "zrhzdr", "helloooo");
  set_insert(clues, "h4e", "helloooo");
  set_insert(clues, "wf", "helloooo");
  set_insert(clues, "zsf4", "helloooo");
  set_insert(clues, " fv", "helloooo");
  set_insert(clues, "sfg", "helloooo");
  set_insert(clues, "zsg", "helloooo");
  set_insert(clues, "zhdr3", "helloooo");
  set_insert(clues, "zfsdxvb", "helloooo");
  set_insert(clues, "yfj", "helloooo");
  set_insert(clues, "fjy", "helloooo");
  set_insert(clues, "sfdgdf", "helloooo");
  set_insert(clues, "zhdfgdhtfdr3", "helloooo");
  set_insert(clues, "zfssfdsfgsdgfdxvb", "helloooo");
  set_insert(clues, "yfjsfrgdrgds", "helloooo");
  set_insert(clues, "fj6ytjy", "helloooo");


  updateClues_I(clues);

    team_t * team = hashtable_find(teamhash, "one");

  team->claimed = 5;

  char * message = input_I();

  printf("string:%s", message);

  if ( strcmp(message, "end") == 0){

     gameOver_I(teamhash);

  }
}



//mygcc -o displaytest displaytest.c display.c display.h -lncurses


//mygcc -o displaytest displaytest.c display.c display.h ../common/team.h ../libcs50/hashtable.h ../libcs50/set.h -lncurses

