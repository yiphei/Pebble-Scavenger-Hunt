

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
#include "../common/krag.h"




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
  updateLocation("mark", "one", -72.288408, 43.704490, teamhash);
  updateLocation("john", "one", -72.289408, 43.705390, teamhash);
  updateLocation("yifei", "one", -72.290408, 43.706090, teamhash);
  set_t *  set = getAllFieldAgents("one", teamhash);



  krag_t * k1 = kragNew(-72.288408, 43.706190);
  krag_t * k2 = kragNew(-72.288408, 43.703590);

  set_t * kragset = set_new();

  set_insert(kragset, "AA", k1);
  set_insert(kragset, "BB", k2);


  updateMap_I(set, kragset);
  
  set_t * clues = set_new();
    set_insert(clues, "afsrgsefsrgr", "helloooo");
  set_insert(clues, "afsrgefsfsrgr", "helloooo");
  set_insert(clues, "afsrsdfgsrgr", "helloooo");
  set_insert(clues, "afsrgdfessrgr", "helloooo");
  set_insert(clues, "afsrgssrgr", "helloooo");
  set_insert(clues, "afsrgsdrgr", "helloooo");

  set_insert(clues, "afsrgsrgr", "helloooo");
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
    set_insert(clues, "asd", "asdd fr rgnd gdrkj gdrng dkrgnrdjkg drdrjgn drgkdrjgn dkjrgn drng dlg rl sf s flksflkrsn rsgsr grr g");
  set_insert(clues, "zhdr3", "helloooo");
  set_insert(clues, "zfsdxvb", "helloooo");
  set_insert(clues, "yfj", "helloooo");
  set_insert(clues, "fjy", "helloooo");
  set_insert(clues, "sfdgdf", "helloooo");
  set_insert(clues, "zhdfgdhtfdr3", "helloooo");
  set_insert(clues, "zfssfdsfgsdgfdxvb", "helloooo");
  set_insert(clues, "yfjsfrgdrgds", "helloooo");
  set_insert(clues, "fj6ytjy", "helloooo");
  set_insert(clues, "sdsf", "helloooo");
  set_insert(clues, "yfjsfsdfrgdrgds", "helloooo");
  set_insert(clues, "fj6fdsytjy", "feffefsefse asdas asd ger gdrg drognrosf   efseofie fosef ef sefsef sef");


  updateClues_I(clues);

  team_t * team = hashtable_find(teamhash, "one");

  team->claimed = 5;

  char * message = "hello";

  while (strcmp(message, "end") != 0 ){

    message = input_I();
  }

  gameOver_I(teamhash);

  //free(message);
  //deleteTeamHash(teamhash);
  // set_delete(kragset,kragdelete);
   //set_delete(clues,setcluedelete);
}



