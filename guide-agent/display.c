/* 
 * display.c - 'display' module
 *
 * see display.h for more information.
 *
 * GREP
 */


#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <ncurses.h>
#include <string.h>
#include <stdbool.h>
#include "display.h"

static int NROWS;
static int NCOLS;

static const char DEADCELL = ' ';

static inline int min(int a, int b) { return a < b ? a : b; }
static inline int max(int a, int b) { return a > b ? a : b; }


/* Local function prototypes */
static char **new_board(void);
static void load_board(char **board, FILE *fp);
static void display_board(char **board);


int map_uy = 46;  //map height
int map_ux = 90;  //map width
int map_ly = 0;   //y-coordinate of map
int map_lx = 0;	  //x-coordinate of map
int stats_uy = 5;  //height of stats window
int stats_ux = 35;  //width of stats, clues, and strings window
int string_uy = 6;  //height of the string window

WINDOW * mapWin;    //window where the map will be displayed
WINDOW * stringWin;  //window where the current revealed string of the team is displayed
WINDOW * statsWin;   //window where game stats are displayed
WINDOW * cluesWin;   //window where all avaible clues are displayed
WINDOW * inputWin;   //window where the input box is displayed

//from life.c of David Kotz
void initialize_curses()
{
  // initialize the screen - which defines LINES and COLS
  initscr();

  // cache the size of the window in our global variables
  NROWS = LINES;
  NCOLS = COLS;

  cbreak(); // actually, this is the default
  noecho(); // don't show the characters users type

  // I like yellow on a black background
  start_color();
}


//source  http://www.tldp.org/HOWTO/NCURSES-Programming-HOWTO/windows.html
WINDOW * createWin_I(int height, int width, int starty, int startx)
{	
	WINDOW *local_win;
	local_win = newwin(height, width, starty, startx);    //create a new window
	box(local_win, 0 , 0);  //create outline of the window
	wrefresh(local_win);	
	return local_win;
}



void initializeWindows_I(void){
	int x,y;
	mapWin =createWin_I(map_uy, map_ux, 0, 0);  //create map window
	FILE * fp = fopen("campusmap", "r");
	char **board1 = new_board();

	load_board(board1, fp);
	display_board(board1);

	fclose(fp);


	//create game stats window
	statsWin =  createWin_I(stats_uy, stats_ux, 0, map_ux);  
	getbegyx(statsWin, y,x);
	wmove(statsWin, y, x);
	wprintw(statsWin, "GAME STATS");
	wrefresh(statsWin);

	//create current revealed string window
	stringWin = createWin_I(string_uy, stats_ux, stats_uy, map_ux);  //for curr stirng
	getbegyx(stringWin, y,x);
	wmove(stringWin, y, x);
	wprintw(stringWin, "CURRENT STRING");
	wrefresh(stringWin);

	//create clues window
	cluesWin = createWin_I(map_uy - stats_uy - string_uy, stats_ux, stats_uy + string_uy, map_ux); //list of all clues
	getbegyx(cluesWin, y,x);
	wmove(cluesWin, y, x);
	wprintw(cluesWin, "CLUES");
	wrefresh(cluesWin);

	//create input window
	inputWin = createWin_I(stats_uy, stats_ux + map_ux, map_uy, 0); //list of all clues
	getbegyx(inputWin, y,x);
	wmove(inputWin, y, x);
	wprintw(inputWin, "INPUT");
	wrefresh(inputWin);
}


void updateMap_I(set_t * fieldagents, set_t * krags){
	//erase window
	werase(mapWin);
	box(mapWin, 0 , 0);

	//display the map
	FILE * fp = fopen("campusmap", "r");
	char **board1 = new_board();
	load_board(board1, fp);
	display_board(board1);
	fclose(fp);

	addPlayers_I(fieldagents);  //display the agents
	addKrags_I(krags);  //displat the krags

}

//helper function to display agents names and their locations in different colors
static void displayAgents(void *arg, const char *key, void *item)
{

	int *y = (int *)arg;  //y coordinate where the agent name is displayed

	//if y > 7, then no more colors avaible, so restart from color 1
	if ( *y > 7){
		attron(COLOR_PAIR(*y - 7));  //turn the color on
	}
	else{
		attron(COLOR_PAIR(*y));   //turn the color on
	}
  	fieldAgent_t * fa = item;
  	double longitude = fa->longitude;
  	double latitude = fa->latitude;

 	mvprintw( *y, 1,  "%s", key);  //print agent name
 	mvprintw( latitude, longitude,  "*", key);   //print agent lcoation
	refresh();

	attroff( COLOR_PAIR(*y));  //turn the color off
	(*y)++;

}


void addPlayers_I(set_t * fieldagents){

	int y = 1; //y coordinate where the agent name is displayed

	//initialize color pairs for the different field agents
	init_pair(1,COLOR_RED, COLOR_BLACK); 
	init_pair(2,COLOR_BLUE, COLOR_BLACK);
	init_pair(3,COLOR_GREEN, COLOR_BLACK);
	init_pair(4,COLOR_YELLOW, COLOR_BLACK);
	init_pair(5,COLOR_MAGENTA, COLOR_BLACK);
	init_pair(6,COLOR_CYAN, COLOR_BLACK);
	init_pair(7,COLOR_WHITE, COLOR_BLACK);
	set_iterate(fieldagents, &y, displayAgents);   //display field agents and their locations with different colors
}

//helper function to display the krags on the map
static void displayKrags(void *arg, const char *key, void *item){

	int * color = (int *)arg;
	attron(COLOR_PAIR(* color)); //initialize color

	krag_t * krag = item;
	double longitude = krag->longitude;
  	double latitude = krag->latitude;

	mvprintw(latitude, longitude, "krag");
	refresh();
	attroff(COLOR_PAIR(* color));  //turn off color

}


void addKrags_I(set_t * krags){
	int y = 8; //number of the color
	init_pair(y,COLOR_WHITE, COLOR_YELLOW);  //initialize color
	set_iterate(krags, &y, displayKrags); 
}


static char**
new_board(void)
{
	//columns is vertical
	//row is horizontals

  // allocate a 2-dimensional array of NROWS x NCOLS
  char **board = calloc(map_uy, sizeof(char*));
  char *contents = calloc(map_uy * map_ux, sizeof(char));
  if (board == NULL || contents == NULL) {
    fprintf(stderr, "cannot allocate memory for board\r\n");
    exit(1);
  }

  // set up the array of pointers, one for each row
  for (int y = 0; y < map_uy; y++) {
    board[y] = contents + y * map_ux;
  }

  // fill the board with empty cells
  for (int y = 0; y < map_uy; y++) {
    for (int x = 0; x < map_ux; x++) {
      board[y][x] = DEADCELL;
    }
  }
  return board;
}



static void
load_board(char **board, FILE *fp)
{
  const int size = map_ux+2;  // include room for \n\0
  char line[size];	      // a line of input
  int y = 0;

  // read each line and copy it to the board
  while ( fgets(line, size, fp) != NULL && y < map_uy) {
    int len = strlen(line);
    if (line[len-1] == '\n') {
      // normal line
      len--; // don't copy the newline
    } else {
      // overly wide line
      len = map_ux;
      for (char c = 0; c != '\n' && c != EOF; c = getc(fp))
	; // scan off the excess part of the line
    }
    strncpy(board[y++], line, len);
  }

  if (!feof(fp)) {
    fprintf(stderr, "board too big for screen; truncated to %d lines\r\n", y);
  }
}

static void
display_board(char **board)
{
  for (int y = 0; y < map_uy; y++) {
    for (int x = 0; x < map_ux; x++) {
      move(y,x);	      // CURSES
      addch(board[y][x]);     // CURSES
    }
  }
  refresh();				      // CURSES
}


void updateString_I(char * revealedString){

	//erase previous string
	werase(stringWin);
	box(stringWin, 0 , 0);
	wprintw(stringWin, "CURRENT STRING");

	//print out new string
	mvwprintw(stringWin, 2, 1,  "%s", revealedString);
	wrefresh(stringWin);
}



//helper function to print out all the clues
static void printClues(void *arg, const char *key, void *item){

	int *ly = (int *)arg;  ////y coordinate where the clues is displayed
	char * clue = item;

	if (clue != NULL){

	 	int x, y;
		getbegyx(cluesWin, y, x);
		int max = y + (map_uy - stats_uy - string_uy) - 3;  //max y before exiting the clues window

		//make sure clues dont go out the window boundaries
		if ( *ly < max){
			mvprintw( *ly + 2, x + 1,  "%s\n", clue);  //print the clue
			refresh();
		}
		(*ly)++;  //increment y coordinate
	}
}

void updateClues_I(set_t * clues){

	//erase previosu clues
	werase(cluesWin);
	box(cluesWin, 0 , 0);
	wprintw(cluesWin, "CLUES");

	int lx, ly;
	getbegyx(cluesWin, ly, lx);  //get upper left coordinates
	set_iterate(clues, &ly, printClues);  //print all clues

	lx++; //not needed, but just used to get rid of compiler warning

}

void updateTotalKrags_I(int totalKrags){

	//display total krags
	mvwprintw(statsWin, 2, 1,  "Total krags in game: %d\n", totalKrags);
	wrefresh(statsWin);
}


void updateKragsClaimed_I(int claimed){

	//display krags claimed
	mvwprintw(statsWin, 3, 1,  "Total krags claimed: %d\n", claimed);
	wrefresh(statsWin);
}


//from curse.c of David Kotz 
char * input_I(void){

	//erase previous input
	werase(inputWin);
	box(inputWin, 0 , 0);
	wprintw(inputWin, "INPUT");

	int c; //the character
	char * string = malloc(141);  //the hint

  	wmove(inputWin,1,1); //move cursor
  	int pos = 0; //pos in the string

  	//read input until user presses return
  	while ((c = wgetch(inputWin)) != '\n') {
		if (isprint(c)) { 
			waddch(inputWin, c);  //add characters to the screen
  			string[pos++] = (char) c;  //add characters to the hint
		}
    wrefresh(inputWin);
  }

  wrefresh(inputWin);
  string[pos] = '\0';  //add terminate charcters
  refresh();
  return string;
}


static void printGameOver(void *arg, const char *key, void *item){

	int *y = (int *)arg;
	(*y)++;
	team_t * team = item;

	mvprintw( *y , 30,"\n", key);
	(*y)++;
	mvprintw( *y , 30,"Team: %s\n", key);
	(*y)++;
	mvprintw(*y, 30, "Players: %d\n", team->numPlayers);
		(*y)++;
	mvprintw(*y, 30, "Krasgs claimed: %d\n", team->claimed);

	refresh();

}


void gameOver_I(hashtable_t * teamhash){

	//erase everything in all windows
	werase(mapWin);
	werase(cluesWin);
	werase(statsWin);
	werase(inputWin);
	werase(stringWin);
	erase();
	refresh();

	//delete all windows
	delwin(mapWin);
	delwin(cluesWin);
	delwin(statsWin);
	delwin(inputWin);
	delwin(stringWin);

	//cordinates where the statements will be displayed
	int x = 30;
	int y = 5;
	mvprintw( y, x,"GAME OVER\n");
	hashtable_iterate(teamhash, &y, printGameOver);  //print team stats

	mvprintw( y+ 3, x,"Press any key to exit\n");
	getch();
  	endwin();

}












