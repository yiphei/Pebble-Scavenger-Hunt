

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


  updateTotalKrags_I(5);


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

  
  set_t * clues = set_new();

  set_insert(clues, "a", "helloooo");
  set_insert(clues, "b", "helloooo");
  set_insert(clues, "c", "helloooo");
  set_insert(clues, "d", "helloooo");
  set_insert(clues, "e", "helloooo");
  set_insert(clues, "f", "helloooo");
  set_insert(clues, "g", "helloooo");





  updateClues_I(clues);



  char * message = input_I();

  printf("string:%s", message);

	getch();


  endwin();
}



//mygcc -o displaytest displaytest.c display.c display.h -lncurses


//mygcc -o displaytest displaytest.c display.c display.h ../common/team.h ../libcs50/hashtable.h ../libcs50/set.h -lncurses

