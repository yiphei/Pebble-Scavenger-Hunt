# Implementation
_Tony DiPadova, Michael Perezous, Paolo Takagi, Yifei Yan_ 

## Guide Agent

The implementation of the Guide Agent is based on the previously made
design spec (DESIGN.md).

### Components

The Guide Agent contains two major components.

#### guideagent.c

This component contains and handles the game logic, handling and sending 
messages based on the status of the game. This component abstracts displaying
the status to the display.c module, which will be explained later, by sending
specific components of the game to update the user interface. It leverages the 
common directories `network.h`, `message.h`, and `log.h`. 

##### Main Functions

```
int main(int argc, char **argv) 
```
This function parses and validates the command line arguments based on the 
requirement spec. Command line argument types are modified as needed and then
passed onto the game function to be handled and used in the context of the game
logic. 

```
int game(char *guideId, char *team, char *player, char *host, int port);
```
This function handles the connecting to the server, receiving and sending
messages when appropriate. Its return value is an integer that represents the exit 
status of the program; if it successfully runs until the game ends, it will be 0. 
Else, it will be greater than 0. It loops until a _GAME\_OVER_ opCode is received
from the Game Server, so the while loop is structured to continue until a 
break statement is called at that point. Within the while loop, _receiveMessage_ 
is used to listen for a message from the server at the beginning of the loop. 
Once received, _parseMessage_ is leveraged to put this string into a message 
struct so that its components are easily accessible (largely based on what we
know about the components of a specific opCode). _game_ then leverages a 
function dispatch table, `opCodes`, to call a specific function based on
the opCode received. There is one dispatch function to call if the Guide Agent
should not be handling this opCode, _badOpCode_, and a specific function for 
each message the Guide Agent can handle with names based on the opCode.

```
bool sendGA_STATUS(char *gameId, char *guideId, char *team, char *player, char *statusReq, connection_t *connection, FILE *file);
```
This function handles sending the main output of the Guide Agent to the Game
Server: the GA\_STATUS. It leverages the _sendMessage_ and _logMessage_ 
functions to send the message to the Game Server and log the message to the 
opened log file. A string message is constructed from the `message_t` input
to pass to both the _sendMessage_ and _logMessage_ functions to have a more
uniform message structure. Its return value is a bool to ensure that the 
message was successfully sent and that the memory for the message that is 
constructed is successfully allocated.

```
bool sendGA_HINT(char *gameId, char *guideId, char *team, char *player, char *pebbleId, char *hint, connection_t *connection, FILE *file);
```
This function handles sending custom textual hints to the Game Server, which
will then be forwarded to a specified Field Agent. _game_ leverages this 
function whenever is receives input from the user through the interface input
area. It uses the _sendMessage_ and _logMessage_ common functions to send the
message to the Game Server and log the activity to the already opened log file.
Like _sendGA\_STATUS_, it returns a bool to ensure successful delivery and
allocation of memory of the string to be sent.

```
int handleMessage(char *messagep, team_t *teamp, connection_t *connection, char *filePath, hashtable_t *teams);
```
_handleMessage_ is called when select reads input from the socket. This function
just parses the message into a message\_t struct and passes the message and
other parameters onto the function dispatch table, which handles the message 
according to its type.

```
void handleHint(char *gameId, char *guideId, char *team, char *player, char *hint, connection_t *connection, char *filePath, team_t *teamp);
```
_handleHint_ is called when select reads input from stdin (the user). This 
function parses that input to look for a specified recipient (denoted by the
first word of the message) and uses _sendGAHINT_ to send the user input hint
to the specified recipient(s).

##### Data Structures

`Op Code Handler Function Table`

This function table is used to easily loop over based on a given opCode in a
message and make a call to the correct function using its function pointer,
thus handling the message type in the correct way and updating the game status
accordingly.
```
static const struct {

	const char *opCode;
	void (*func)(char *messagep, message_t *message, team_t *teamp, connection_t *connection, FILE *log);

} opCodes[] = {
	{"FA_CLAIM", badOpCodeHandler},
	{"FA_LOG", badOpCodeHandler},
	{"GA_STATUS", badOpCodeHandler},
	{"GA_HINT", badOpCodeHandler},
	{"FA_LOCATION", badOpCodeHandler},
	{"GAME_STATUS", gameStatusHandler},
	{"GS_AGENT", GSAgentHandler},
	{"GS_CLUE", GSClueHandler},
	{"GS_SECRET", GSSecretHandler},
	{"GS_RESPONSE", GSResponseHandler},
	{"TEAM_RECORD", teamRecordHandler},
	{"GAME_OVER", gameOverHandler},
	{NULL, NULL}
};
```

`set`

The guideagent.c component will leverage a _set_ to store the field agents on
its team, which themselves will be stored in a _fieldAgent_ struct that is 
defined in `team.h`.

`connection`

To be able to coontinuously send and receive messages from the Game Server, the
Guide Agent will also leverage a _connection_ struct defined in `network.h`.
This data structure just stores the socket address and the port number so that
both are accessible from the return value of _openSocket_.

`message`

When a message is received, a _message_ struct is also leveraged. This is
returned by _parseMessage_ in the `message.h` module, storing all components
of a specific message into the structure with their respective types. This
can be used to easily extract opCodes and dispatch functions, then easily parse
and use specific components of a message in handling that message without
having to parse each message type in a different way.

`team`

A _team_ struct will also be used to store the guide agent's own information,
along with that of his own team's field agents. This implementation allows
for the abstraction of the modification of this data as well, by having 
functions to update the agents and access the agents' information without
hardcoding it. Due to common implementation across modules, a _hashtable_
is used to store this team (that way, the Game Server can also use these
functions when handling multiple teams at a time).

`time`

Mainly for the log file entries, the Guide Agent will also use a _time_ struct
from the `time.h` library in order to create the timestamps for the log file
entries.

##### Pseudocode

1. Parse and validate the given command line arguments, passing
	the game information onto _game_.
2. Open the socket to connect to the server and stores the socket
	and connection address information in a _connection_ struct.
3. Open the log file and saves the pointer to the file for future use.
4. Store the start time to use for future log entries.
5. A _team_ struct is created and the Guide Agent and its known information
	is stored as the _team_'s guideAgent.
6. The Guide Agent sends its first GA\_STATUS to the Game Server with a "0"
	gameId to announce its presence in the game via )_sendGA\_STATUS_.
7. Wait and listen for the server to respond with a GAME\_STATUS
	to initialize the game stats and update the gameId, using _receiveMessage_.
8. While the game is not over:
	1. Listen for a message.
	2. If a message is received from the Game Server,
		1. Use _parseMessage_ to parse the message into a _message_ struct
			and store all of its respective fields in the struct.
		2. Loop over the function dispatch table to find the right function
			based on the message's opCode.
		3. If the opCode does not exist in the function dispatch table, print
			an "unknown opCode" message and ignore the message.
		4. If the opCode is "GAME\_OVER", break the while loop.
	3. Calculate the time since the beginning of the game.
	4. If that time is divisible by 30 (more or less every 30 seconds),
		send a GA\_STATUS message to the Game Server.
		1. If the time is divisible by 60 (more or less every minute), send the
			GA\_STATUS with a statusReq of 1, asking for a GAME\_STATUS update
			in return.
9. Free memory allocated by _game_ and return to _main_.
10. Free memory allocated in _main_ and exit with exit status returned by
	_game_.

##### Modularity

_Cohesion_

I consider this component of the Guide Agent to be sequentially cohesive.
For the most part, the functions have been factored out to perform one function
each, which was the goal, but due to the design of the Guide Agent it was 
necessary to have the _game_ function perform more than just one function and 
perform them in order.

`main`
main's only function is to parse and validate the command line arguments input
by the user.

`game`
game has a few functions that are linked together for better functionality.
It connects to the server, opens a log file, and then proceeds to use this
connection and file to receive messages from the server and call other 
functions to handle them (in turn logging the activity with the opened log
file). Its last function is, in the same while loop that listens for messages
(what I call the "game loop" because it runs until GAME\_OVER is received), 
the Guide Agent sends a GA\_STATUS message to the Game Server every 30 seconds.
Every 60 seconds, it sends a statusReq along with GA\_STATUS that requests 
a GAME\_STATUS update from the Game Server.

`sendGA\_STATUS` and `sendGA\_HINT`
These two functions perform one function that is fairly obvious from their 
function names; they both send a specific type of message to the Game Server.
In fact, both functions are structured very similarly, but they inductively
build the messages to have different structure, so I separated them into two
functions to make that functionality clear.

_Coupling_

This module is coupled through data structure coupling. There are no global
variables sharing data within the entire module, so the only data passed back
and forth is through the parameters of the functions. However, it is not 
implemented with simple-data coupling because the design of the module calls
for fairly complex data structures to be passed containing a lot of necessary
data to continue functioning.

#### Display.c

The display module handles the GUI part of the guide agent. It displays five windows in total. The map window will have field agents name on the top left, their current locations in the map, and a ASCII based map of the campus. The stats window will display the total number of krags in the game and the total nuber of krags claimed. The current string windown displays the current revealed string of the team. The clues window displays all the known clues to the team. The input window reads from stdin. When game ends, then it will display a game over 


##### Data Structures

The display module does not make use of a lot data structures. The only data structures used are set, hashtable, and team data structures. The sets are used in displaying clues in the clues window, and field agent and their locations in the map window. Because field agents themselves are a data strucutre, we need the `fieldAgent_t` struct in `team.h`. Last, the hashtable are used to display the game over screen. The hashtable, with keys being teamname and items being team struct, will be iterated to display every team's statistics.

##### Functions


**initialize_curses**

This function initializes the basic ncurses elemts like screen and color. User should call this function first before any other display function.

	void initialize_curses();

**createWin_I**

This function creates and returns a window given a height, width, and start x and y coordinates

	WINDOW *createWin_I(int height, int width, int starty, int startx);

**initializeWindows_I**

This function initialites the five windows of the game and print their window names on the top left of each window. These windows are: game, game status, string, clues, and input.

	void initializeWindows_I(void);

**updateMap_I**

This function updates the map with new info. For example, it calls `addPlayers_I` to display field agents and their location. It calls `loadMap` to display the campus map.

	void updateMap_I(set_t * fieldagents, FILE * fp);

**addPlayers_I**

This function adds the field agents name on the top left corner of the mpa window, and their renspective location (marked with *) on the map. Each player and their location mark have an individual color. 

	void addPlayers_I(set_t * fieldagents);

**loadMap**

This funtion loads an ASCII based map from a file and renders it in the game window.

	void loadMap(FILE *fp);

**updateString_I**

This function displays the current revealed string of the team to the string window

	void updateString_I(char * revealedString);

**updateClues_I**

This function outputs all the clues to the the clue window. If clues exceed the borders of the clue window, then those clues wont be displayed

	void updateClues_I(set_t * clues);

**updateTotalKrags_I**

This function displays the total krag number in the game. It is displayed in the stats window

	void updateTotalKrags_I(int totalKrags);

**updateKragsClaimed_I**

This function displays the number of krags claimed by the team. It is displayed in the stats window

	void updateKragsClaimed_I(int claimed);

**input_I**

This function reads from stdin until user inputs CONTROL D. When CONTROL D is inputted, then the function stops reading frmo stdin and returns the string to the caller.

	char * input_I(void);
	
**gameOver_I**

This function displays the game over stats to the screen. For every team, it will diplay a teamname, number of players, and number of krags claimed. This function should be called at the end of the game.
	
	void gameOver_I(hashtable * teams);
	
#####Modularity

In the display module, there is strong cohesion because all routines perform functions related to one specific general function, which is graphical user interface. Many of the routines have sequential cohesion. For examples, some routines retrieve info about a `WINDOW` and uses that info to perform some operations. Sometimes, there are some routines that have communicational cohesion. There is relatively weak coupling between the routines because all of them function autonomously. Most of the routines are just simple-data coupling.



## Field Agent

### Data Structures:

* Both the hardcoded list of player names and team names will be in the form of an array of strings (`char *[]`).  While they are both default of size 4, they are independent of each other (i.e. it is possible to change the list of player names to be of size 5 and have the list of team names be of size 3).
* The list of hints will be stored in a `set` data structure.  Each individual hint will be represented by a string, with a timestamp as the key (in string format).

### Functions:

**Main, Init, Deinit:**

```
int main(void)
```
runs `init()`, then `app_event_loop()`, and finally `deinit()`

```
static void init()
```
registers Bluetooth communication callbacks (`inbox_received_handler`, `inbox_dropped_callback`, `outbox_failed_callback`, and `outbox_sent_callback`), then creates a `join_game_window`, `select_player_window`, `select_team_window`, `in_game_window`, `claim_krag_window`, and `hints_log_window`.  For each of those windows, it sets the `.load` and `.unload` to their corresponding handlers (will discuss further below).  Finally, it will push the `join_game_window` onto the window stack.
```
static void deinit()
```
destroys all of the windows created in `init()`

**Join Game Menu:**

```
static void join_game_window_load(Window *window)
```
`.load` handler for `join_game_window`.  Creates a Menulayer called `join_game_layer`, and sets `.get_num_rows`, `.draw_row`, `.get_cell_height`, `.select_click` to their corresponding callbacks.  It adds this to the `join_game_window`.  Also, it runs `menu_layer_set_click_config_onto_window`, so that the user can interact with this particular layer.

```
static void join_game_window_unload(Window *window)
```
destroys the previously created `join_game_layer` Menulayer.

```
static uint16_t join_game_get_num_rows_callback(MenuLayer *menu_layer, uint16_t section_index, void *context)
```
callback for `get_num_rows` for `join_game_layer` Menulayer.  It returns the number of rows in the menu for the `join_game_layer`.

```
static void join_game_draw_row_callback(GContext *ctx, const Layer *cell_layer, MenuIndex *cell_index, void *context)
```
callback for `.draw_row` for `join_game_layer` Menulayer.  It draws the corresponding row in the menu, depending on which is requested.  Row 0 is the "Select Player" option, row 1 is the "Select Team" option, and row 2 is the "Join Game" option.

```
static int16_t join_game_get_cell_height_callback(struct MenuLayer *menu_layer, MenuIndex *cell_index, void *context)
```
callback for `.get_cell_height` in the `join_game_layer` Menulayer.  Returns the height for each cell in the menu.

```
static void join_game_select_callback(struct MenuLayer *menu_layer, MenuIndex *cell_index, void *context)
```
callback for `.select_click` for the `join_game_layer` Menulayer.  This handles each time the select (middle right) button on the pebble is pressed.  If it is pressed when it is on the "Select Player" option, it runs `select_player_window_push()` (see below), if it is pressed on the "Select Team" option, it runs `select_team_window_push()` (see below), and finally, if it is pressed on the "Join Game" option, then it attempts to connect to the game.  Upon success, it runs `in_game_window_push` to push `in_game_window` Window onto the window stack. It then subscribes to the `tick_timer_service` using `tick_timer_service_subscribe`, using `tick_callback` function (see below).  Otherwise, it runs `my_dialog_window_push` with the `type` being `error` and the `text` being `failed to connect to server`.

**Select Team Menu:**

```
static void select_team_window_push()
```
creates the `select_team_window` Window if it had not been created yet, and creates the corresponding WindowHandlers for `.load` and `.unload` (see below) for the `select_team_layer`.  Then it pushes the `select_team_window` onto the window stack using `window_stack_push`.

```
static void select_team_window_load(Window *window)
```
callback for `.load` for `select_team_window`. Creates the `select_team_layer` Menulayer, and gives sets the corresponding handlers for `.get_num_rows`, `.draw_row`, `.get_cell_height`, and `.select_click`.  Puts everything onto the `select_team_window`.

```
static void select_team_window_unload(Window *window)
```
callback for `.unload` for `select_team_window`. Destroys the `select_team_layer`.

```
static void select_team_select_callback(struct MenuLayer *menu_layer, MenuIndex *cell_index, void *callback_context)
```
callback for `select_click` for `select_team_layer` Menulayer.  This runs whenever the select (middle right) button on the Pebble is pressed.  If it is pressed on any option except for the "Submit" option, then it changes the static `selected_team` string to the corresponding team name.  On the other hand, if the select btton is pressed on the "Submit" option, then it first unloads the `select_team_window`, and then pops the `select_team_window` off of the window stack.

```
static int16_t select_team_get_cell_height_callback(struct MenuLayer *menu_layer, MenuIndex *cell_index, void *context)
```
callback for `.get_cell_height` for `select_team_layer` Menulayer. Returns the cell height of each cell in the `select_team_layer` Menulayer.

```
static void select_team_draw_row_callback(GContext *ctx, const Layer *cell_layer, MenuIndex *cell_index, void *context)
```
callback for `.draw_row` in `select_team_layer` Menulayer.  Draws a row for each  item inside the `team_names` array of `char *` (The initial values are "One", "Two", "Three", and "Four").  For each of those cells, it also draws an empty circle, which represents the radio_button (unless the option had been selected, in which case it is drawn as a filled in circle).  The last option in this Menulayer is "Submit", and this one does not have a radio button.

```
static uint16_t select_team_get_num_rows_callback(MenuLayer *menu_layer, uint16_t section_index, void *context)
```
callback for `.get_num_rows` for `select_team_layer` Menulayer.  Returns the number of rows in the `select_team_layer` Menulayer.  This is the number of items in the `team_names` array plus one (for the "Submit" option).

**Select Player Name Menu:**

```
static void select_player_window_push()
```creates the `select_player_window` Window if it has not been created yet, and creates the corresponding WindowHandlers for `.load` and `.unload` (see below) for the `select_player_layer`.  Then it pushes the `select_player_window` onto the window stack using `window_stack_push`.

```
static void select_player_window_load(Window *window)
```
handler for `.load` for `select_player_window` Window. creates the `select_player_layer` Menulayer, and gives sets the corresponding handlers for `.get_num_rows`, `.draw_row`, `.get_cell_height`, and `.select_click`.  Puts everything onto the `select_player_window`.

```
static void select_player_window_unload(Window *window)
```
callback for `.unload` for `select_player_window`. Destroys the `select_player_layer`.

```
static void select_player_select_callback(struct MenuLayer *menu_layer, MenuIndex *cell_index, void *callback_context)
```
callback for `select_click` for `select_player_layer` Menulayer.  This runs whenever the select (middle right) button on the Pebble is pressed.  If it is pressed on any option except for the "Submit" option, then it changes the static `selected_player` string to the corresponding team name.  On the other hand, if the select btton is pressed on the "Submit" option, first it unloads the `select_player_window`  then it pops the `select_player_window` off of the window stack.

```
static int16_t select_player_get_cell_height_callback(struct MenuLayer *menu_layer, MenuIndex *cell_index, void *context)
```
callback for `.get_cell_height` for `select_player_layer` Menulayer. Returns the cell height of each cell in the `select_player_layer` Menulayer. 

```
static void select_player_draw_row_callback(GContext *ctx, const Layer *cell_layer, MenuIndex *cell_index, void *context)
```
callback for `.draw_row` in `select_player_layer` Menulayer.  Draws a row for each  item inside the `player_names` array of `char *` (The initial values are "Paolo", "Tony", "Michael", and "Yifei").  For each of those cells, it also draws an empty circle, which represents the radio_button (unless the option had been selected, in which case it is drawn as a filled in circle).  The last option in this Menulayer is "Submit", and this one does not have a radio button.

```
static uint16_t select_player_get_num_rows_callback(MenuLayer *menu_layer, uint16_t section_index, void *context)
```
callback for `.get_num_rows` for `select_player_layer` Menulayer.  Returns the number of rows in the `select_player_layer` Menulayer.  This is the number of items in the `player_names` array plus one (for the "Submit" option).


**In Game Menu:**

```
static void in_game_window_push()
```
creates the `in_game_window` Window if it has not been created yet, and creates the corresponding WindowHandlers for `.load` and `.unload` (see below) for the `in_game_layer`.  Then it pushes the `in_game_window` onto the window stack using `window_stack_push`.

```
static void in_game_window_load(Window *window)
```
handler for `.load` for `in_game_window` Window. Creates the `in_game_layer` Menulayer, and gives sets the corresponding handlers for `.get_num_rows`, `.draw_row`, `.get_cell_height`, and `.select_click`.  Puts everything onto the `in_game_window`.

```
static void in_game_window_unload(Window *window)
```
callback for `.unload` for `in_game_window`. Destroys the `in_game_layer`.

```
static void in_game_select_callback(struct MenuLayer *menu_layer, MenuIndex *cell_index, void *callback_context)
```
callback for `select_click` for `in_game_layer` Menulayer.  This runs whenever the select (middle right) button on the Pebble is pressed.  It can select and of three options: "Claim", "Hint", and "Status".  If "Claim" is selected, then `claim_krag_window` is pushed to the window stack.  If "Hint" is selected, then `hints_log_window` is pushed to the window stack.  If "Status" is selected, then a status request is sent to the game_server, and the given information is presented in a `my_dialog_box`.  The `type` will be `status` and the `text` will be whatever information was gotten from the Game Server, in a string.

```static int16_t in_game_get_cell_height_callback(struct MenuLayer *menu_layer, MenuIndex *cell_index, void *context)
```
callback for `.get_cell_height` for `in_game_layer` Menulayer. Returns the cell height of each cell in the `in_game_layer` Menulayer. 

```
static void in_game_draw_row_callback(GContext *ctx, const Layer *cell_layer, MenuIndex *cell_index, void *context)
```
callback for `.draw_row` for `in_game_layer` Menulayer.  It draws the corresponding row in the menu, depending on which is requested.  Row 0 is the "Claim Krag" option, row 1 is the "Hints" option, and row 2 is the "Status" option.

```
static uint16_t in_game_get_num_rows_callback(MenuLayer *menu_layer, uint16_t section_index, void *context)
```
callback for `get_num_rows` for `in_game_layer` Menulayer.  It returns the number of rows in the menu for the `in_game_layer`.

**Claim Krag Pin Entry:**


```
static void claim_krag_window_push()
```
creates the `claim_krag_window` Window if it has not been created yet, and creates the corresponding WindowHandlers for `.load` and `.unload` (see below) for `claim_krag_layer`

```
static void claim_krag_window_load(Window *window)
```
handler for `.load` for `claim_krag_window` Window.  Creates the `claim_krag_layer` PinWindow, with the corresponding callback for `.pin_complete`.

```
static void claim_krag_window_unload(Window *window)
```
callback for `.unload` for `claim_krag_window`. Destroys the `claim_krag_layer`.

```
claim_krag_complete(PIN pin, void *context)
```
callback for `.pin_complete` for `claim_krag_layer` PinWindow.  It saves the pin as a static string, for future use by other functions.  Then, it runs `send_claim` now that it has this information (see below).  

```
claim_krag_draw(Layer *layer, GContext *ctx)
```
draws the Pin Entry Window designed to be used to claim krags.

**Hints Log Menu:**

```
static void hints_log_window_push()
```
creates the `hints_log_window` Window if it has not been created yet, and creates the corresponding WindowHandlers for `.load` and `.unload` (see below) for the `in_game_layer`.  Then it pushes the `in_game_window` onto the window stack using `window_stack_push`.

```
static void hints_log_window_load(Window *window)
```
handler for `.load` for `hints_log_window` Window. Creates the `hints_log_layer` Menulayer, and sets the corresponding handlers for `.get_num_rows`, `.draw_row`, `.get_cell_height`, and `.select_click`.  Puts everything onto the `hints_log_window`.

```
static void hints_log_window_unload(Window *window)
```
callback for `.unload` for `hints_log_window`. Destroys the `hints_log_layer`.

```
static void hints_log_select_callback(struct MenuLayer *menu_layer, MenuIndex *cell_index, void *callback_context)
```
callback for `select_click` for `in_game_layer` Menulayer.  This runs whenever the select (middle right) button on the Pebble is pressed.  Whenever it is pressed on any item, it displays the corresponding hint in a `my_dialog_window`, where the `type` is `hint`, and the `text` is whatever the text of the hint is (in string format).

```
static int16_t hints_log_get_cell_height_callback(struct MenuLayer *menu_layer, MenuIndex *cell_index, void *context)
```
callback for `.get_cell_height` for `hints_log_layer` Menulayer. Returns the cell height of each cell in the `hints_log_layer` Menulayer.

```
static void hints_log_draw_row_callback(GContext *ctx, const Layer *cell_layer, MenuIndex *cell_index, void *context)
```
callback for `.draw_row` for `hints_log_layer` Menulayer.  It draws the corresponding row in the menu, depending on which is requested.  Each row is the timestamp for its corresponding hint, all of which are found in the static `set` of hints.

```
static uint16_t hints_log_get_num_rows_callback(MenuLayer *menu_layer, uint16_t section_index, void *context)
```
callback for `get_num_rows` for `hints_log_layer` Menulayer.  It returns the number of rows in the menu for the `hints_log_layer`.  Equivalent to the size of the number of hints in the static `set` of hints.

**Dialog Boxes:**

Dialog boxes are windows that pop up with information to alert the user.  For this project, they have a ton of uses (errors, claiming krags [succesfully or unsuccesfully], hints, status, game over), so I will create a `my_dialog_window` module that will have capabilities to create any dialog window, given two strings as parameters: `type` and `text`.  `type` is title of the message ("Hint", "Error", "Claim", etc.), and `text` is the actual content of the message.

```
void my_dialog_window_push(char *p_type, char *p_text)
```
copies `p_type` and `p_text` into static (file-scope) `type` and `text` strings.  Creates static `my_dialog_window` Window, with the corresponding WindowHandlers for `.load`, `.unload`, and `.appear`.  Finally, pushes `my_dialog_window` to the window stack using `window_stack_push`.

```
void my_dialog_window_pop()
```
unloads `my_dialog_window`, and then pops it from the window stack using `window_stack_pop`.

```
static void window_load(Window *window)
```
handler for `.load` for `my_dialog_window` Window.  Adds `s_background_layer`, then `my_dialog_type_layer`, and finally `my_dialog_text_layer` to the `my_dialog_window` Window.  It sets all their text to either `text` or `type`, depending on which layer it is.  

```
static void window_unload(Window *window)
```
handler for `.unload` for `my_dialog_window` Window.  Destroys `s_background_layer`, `my_dialog_type_layer`, and `my_dialog_text_layer`.  Then it sets `my_dialod_window` to NULL, and finally frees both `type` and `text` strings.

```
static void window_appear(Window *window)
```
handler for `.appear` for `my_dialog_window` Window.  It makes the window appear in a cool, animated fashion, and settles in to the right place according to the bounds that were gathered. Also creates the handler for `.stopped` for static `s_appear_anim` Animation.

```
static void background_update_proc(Layer *layer, GContext *ctx)
```
Sets the background color to yellow if possible, otherwise to white.  Then it fills in the color.  Used by `window_load`.

```
static void anim_stopped_handler(Animation *animation, bool finished, void *context)
```
handler for `.stopped` for `s_appear_anim` Animation.  Simply returns NULL.

**Messaging:**


```
inbox_received_handler(DictionaryIterator *iterator, void *context)
```
AppMessage Bluetooth communication callback for `app_message_register_inbox_received`. Called when a message is received from the smartphone JS proxy (which passes along messages from the Game Server).  This identifies the type of message, and responds accordingly. Furthermore, this is where the `pebbleId` can be found and hence stored as a static variable for future use by other functions.  There are many possible messages to do, and thus many different outcomes this function could have.  Here they are: (If anything in any message is invalid, it is logged and ignored)
	- `GAME_STATUS`: create `my_dialog_window` with type `status` and pertinent information, from the opcode.
	- `GA_HINT`: create `my_dialog_window` with type `hint` and pertinint information, form the opcode
	- `SH_ERROR_DUPLICATE_PLAYERNAME`: create `my_dialog_window` with type `error`  and text `player name is already chosen, please select a different one`
	- `SH_CLAIMED`: create `my_dialog_window` with type `krag` and text `succesfully claimed krag!`
	- `SH_CLAIMED_ALREADY`: create `my_dialog_window` with type `krag` and text `krag has already been claimed`
	- `GS_RESPONSE`: Respond 
	- `GAME_OVER`: create `my_dialog_window` with type `game over` and the secret as text
	- `TEAM_RECORD`: create `my_dialog_window` with type `game over`, and the `text` being information from the `TEAM_RECORD` opcode.
	- `SH_ERROR_INVALID_MESSAGE`: log in app log
	- `SH_ERROR_INVALID_OPCODE`: log in app log
	- `SH_ERROR_INVALID_FIELD`: log in app log
	- `SH_ERROR_DUPLICATE_FIELD`: log in app log
	- `SH_ERROR_INVALID_GAME_ID`: disconnect from game, prompt user with In Game Menu.
	- `SH_ERROR_INVALID_TEAMNAME`: create `my_dialog_window` with type `error` and text `please select a different team`
	- `SH_ERROR_INVALID_PLAYERNAME`: create `my_dialog_window` with type `error` and text `please select and different player name`
	- `SH_ERROR_INVALID_ID`: disconnect from game, prompt user with In Game Menu
	- Anything else: `send_log` that the message was received, and do nothing more.

```
inbox_dropped_callback(AppMessageResult reason, void *context)
```
AppMessage Bluetooth communication callback for `app_message_register_inbox_dropped`. This means that the inbox was dropped for some reason (in order to get this, the message must have been received by the smartphone JS environment).  This is logged by the app as an error, and nothing else can be done.

```
outbox_failed_callback(DictionaryIterator *iterator, AppMessageResult reason, void *context)
```
AppMessage Bluetooth communication callback for `app_message_outbox_failed`. This means that the message send from the Pebble to the smartphone JS environment has failed to send, implying that perhaps the JS environment is not up and running.  This is logged by the app as an error, and it is noted that the JS environment is not running.  The user is prompted to turn on his Bluetooth on his smartphone and connect to the Pebble using a `my_dialog_box` of type `error`.

```
outbox_sent_callback(DictionaryIterator *iterator, void *context)
```
AppMessage Bluetooth communication callback for `app_message_outbox_sent. This implys that the smartphone JS environment is up and running.  It is logged by the app that the message has been succesfully sent, and it is noted that the smartphone JS environment is up and running.

```
static void send_location(bool status)`
```
First requests the current user location by use of `request_location`, then it formats that and other pertinent information to conform with the `FA_LOCATION` OpCode.  Hence, the `gameId`, `pebbleId`, `team` and `player` must already be present and recorded in corresponding static variables.  If they are not, then an error will be recored in the app log, and a message will not be sent.  It is sent by the `send_string` function.  (To better understand `request_location` and `send_string`, see below).  Finally, it takes a parameterized boolean input to know whether or not to request a status message as part of the sent message.  Note, the exception is `gameId`, if that does not exist, the function sends a value of zero instead.

```
static void send_claim()
```
runs after a claim code is submitted via the Claim Krag Pin Entry Window.  It requests the location via `request_location` (see below).  It formats this information with all other associated static information in order to send a `FA_CLAIM` message to the Game Server via `send_string`.  If any necesesary information is not stored within the corresponding static variables, an error is logged in the app log, and no message is sent.

```
static void send_log(char *text)
```
sends a log to the server that conforms with the `FA_LOG` opcode.  It is given a string input to serve as the `text` information.  It needs the pebbleId to already be stored in static information, if it is not, then no mesage is sent and an error is logged in the app log.

```
static void request_location()
```
function that requests the current location of the user from the smartphone.  Requests the location of the user, and then records it in two static strings, `latitude` and `longitude`.  This is called by `send_location`

```
static void send_string(char *string)
```
sends a string (from a parameter) to the Game Server (via the smartphone JS proxy).  It attempts to send the message, and records any errors that occured in doing so.


**Other:**


```
static void tick_callback(struct tm *tick_time, TimeUnits units_changed)
```
run `send_location` every 15 seconds (will check every tick to see if it is the 15th second aka tick).  It does this if the smartphone JS proxy environment is up and running.  If it is not, a `my_dialog_window` is created with type `error` and `text` turn on your smartphone's Bluetooth`.

```
static char **parse_opcode(char *string, char *delim)
```
parses the string opcod ethat is received, given a delimiter.  It is used by `inbox_received_handler` to both determine what type of opcode it is, as well as to extract the pertinent information.

**Modularity/Cohesion:**


I decided to make a module for dialog boxes, because they all essentially do the same thing.  They display some text.  In order to be able to differentiate between them, I added that they display the `type` string as a header in addition to the `text` string as their content.  Hence, I can use one module (`my_dialog_window`) to take care of every dialog window box that I will ever have to make.

On the other hand, I decided to keep menus individual from each other, and not a single module (or two, one for typical menus, and one for menus with radio buttons).  This is because all of these menus had different purposes, and different behaviors when interacted with, so it is difficult to generalize all of their behavior into a single module.  Some menus will manipulate strings, and others will pop other Windows onto the window stack.  Furthermore, their layouts are not necessarily the same either.  For example, one may decide that they want to add more possible player names for the `select_player_layer` Menulayer, but also decrease the number possible teams to join for the `select_team_layer`.  With my solution, this is easily doable.  Yet, there are some universal aspects of every menu.  For example, they all use the `JOIN_GAME_CELL_HEIGHT` macro to determine their cell height.

### Pseudocode:
1. Run `init()`
2. Run `app_event_loop()`
3. Respond and handle input as it comes (from either user ot Game Server inputs).  All of those functions are outlined above.
4. Run `deinit()`

## Game Server

### Data Structures
**Op Code Handler Function Table**

The Op Code handler function table is used to easily handle messages that the game server receives. When an op code is received, a for loop check the function table for the op code and calls the appropriate function to handle that message type.

```c
static const struct {

	const char *opCode;
	void (*func)(char *messagep, message_t *message, hashtable_t* teams, hashtable_t* krags, connection_t *connection, FILE *log);

} opCodes[] = {
	{"FA_CLAIM", FAClaimHandler},
	{"FA_LOG", FALogHandler},
	{"GA_STATUS", GAStatusHandler},
	{"GA_HINT", GAHintHandler},
	{"FA_LOCATION", FALocationHandler},
	{"GAME_STATUS", badOpCodeHandler},
	{"GS_AGENT", badOpCodeHandler},
	{"GS_CLUE", badOpCodeHandler},
	{"GS_SECRET", badOpCodeHandler},
	{"GS_RESPONSE", badOpCodeHandler},
	{"TEAM_RECORD", badOpCodeHandler},
	{"GAME_OVER", badOpCodeHandler},
	{NULL, NULL}
};
```

### Functions

**main**

The `main` function parses the command line arguments and then calls the `gameserver` function.

```c
int main(int argc, char* argv[])
```

**gameserver**

The `gameserver` function runs the game. It loads the krag files, opens and binds a socket to receive messages, and listens for messages calling the appropriate handler functions

```c
int gameserver(char* gameId, char* kff, char* sf, int port);
```

**FAClaimHandler**

Handles the `FA_CLAIM` messages by validating message, validating the krag ID and location, updating the krag and team structs, sending an `SH_CLAIMED` or `SH_CLAIMED_ALREADY` to the caller, updating the team's secret string, then ending the game if the string is complete and sending two more clues if the string is not complete.

```c
static void FAClaimHandler(char* gameId, char *messagep, message_t *message, hashtable_t* teams, hashtable_t* krags, connection_t *connection, FILE *log);
```

**FALogHandler**

Logs messages to the field agent log.

```c
static void FALogHandler(char* gameId, char *messagep, message_t *message, hashtable_t* teams, hashtable_t* krags, connection_t *connection, FILE *log);
```

**GAStatusHandler** 

Adds the team if it is new to the game, adds the Guide Agent if it is new to the game, updates the Guide Agent struct if not new, responds with `GAME_STATUS` if the agent is new or if status is requested, sends a `GS_AGENT` to the Guide Agent for every Field Agent on the team.

```c
static void GAStatusHandler(char* gameId, char *messagep, message_t *message, hashtable_t* teams, hashtable_t* krags, connection_t *connection, FILE *log);
```

**GAHintHandler**

Forwards the Guide Agent's hint to the Field Agent(s).

```c
static void GAHintHandler(char* gameId, char *messagep, message_t *message, hashtable_t* teams, hashtable_t* krags, connection_t *connection, FILE *log);
```

**FALocationHandler**

Adds the team if it is new to the game, adds the Field Agent if it is new to the game, updates the Field Agent with new location if not new, responds with `GAME_STATUS` if the agent is new or if status is requested.

```c
static void FALocationHandler(char* gameId, char *messagep, message_t *message, hashtable_t* teams, hashtable_t* krags, connection_t *connection, FILE *log);
```

**badOpCodeHandler**

Handles incorrect op codes by sending `SH_ERROR_INVALID_OPCODE` to inform them the op code is invalid for the server.

```c
static void badOpCodeHandler(char* gameId, char *messagep, message_t *message, hashtable_t* teams, hashtable_t* krags, connection_t *connection, FILE *log);
```

**validateMessageParse**

Checks if the message finished parsing with an error and sends an error message accordingly.

```c
static bool validateMessageParse(char* gameId, message_t* message, connection_t*, FILE* log);
```

**validateKrag**

Validates a that a krag has the correct Id for an unfound krag and player location is correct for that krag. Return 0 if valid, 1 if found already, 2 if invalid location.

```c
static int validateKrag(char* gameId, char* kragId, double latitude, double longitude, char* team, teamhashtable_t* teams, hashtable_t* krags);
```

**validateFAClaim**

Validates the message structure and members of an `FA_CLAIM` message. Returns true on success.

```c
static bool validateFAClaim(char* gameId, messsage_t* message, hashtable_t* teams, hashtable_t* krags);
```

**validateFALog**

Validates the message structure and members of an `FA_LOG` message. Returns true on success.


```c
static bool validateFALog(char* gameId, messsage_t* message, hashtable_t* teams, hashtable_t* krags); 
```

**validateGAStatus**

Validates the message structure and members of a `GA_STATUS` message. Returns true on success.

```c
static bool validateGAStatus(char* gameId, messsage_t* message, hashtable_t* teams, hashtable_t* krags);
```

**validateGAHint**

Validates the message structure and members of an `GA_HINT` message. Returns true on success.

```c
static bool validateGAHint(char* gameId, messsage_t* message, hashtable_t* teams, hashtable_t* krags);
```

**validateFALocation**

Validates the message structure and members of an `FA_CLAIM` message. Returns true on success.

```c
static bool validateFALocation(char* gameId, messsage_t* message, hashtable_t* teams, hashtable_t* krags);
```

**sendGameStatus**

Builds a message string for a `GAME_STATUS` and sends it using the network module. Returns true on success.

```c
static bool sendGameStatus(char* gameId, char* guideId, int numClaimed, int numKrags, connection_t* connection, FILE* log);
```

**forwardHint**

Forwards a `GA_HINT`message by sending it using the network module. Returns true on success.

```c
static bool forwardHint(char* hintMessage, connection_t* connection, FILE* log);
```

**sendAllGSAgents**

Builds a message string for a `GSAgent` for each Field Agent and sends them to the Guide Agent using the network module. Returns true on success.

```c
static bool sendAllGSAgents(char* gameId, char* team, hashtable_t* teams, connection_t* connection, FILE* log);
```

**sendClue**

Builds a message string for a `GS_CLUE` and sends it to the appropriate Field Agent(s) using the network module. Returns true on success.

```c
static bool sendClue(char* gameId, char* guideId, char* clue, double latitude, double longitude, connection_t* connection, FILE* log);
```

**sendSecret**

Sends the updated reveal string `GS_SECRET` message to the Guide Agent. Returns true on success.

```c
static bool sendSecret(char* gameId, char* guideId, char* secret, connection_t* connection, FILE* log);
```

**sendGameOver**

Sends a `GAME_OVER` command to all players. Returns true on success.

```c
static bool sendGameOver(char* gameId, hashtable_t* teams, FILE* log);
```

**sendTeamRecord**

Sends a team record to every team at the end of the game.

```c
static bool sendTeamRecord(char* gameId, hashtable_t* teams, FILE* log);
```

**sendResponse**

Sends a `GS_RESPONSE` message to a specified player. Returns true on success.

```c
static bool sendResponse(char* gameId, char* respCode, char* text, connection_t* connection, FILE* log);
```

### Pseudo Code

1. Parse command line args
	1. Iterate through all args and search for the keys `gameId=`, `kff=`, `sf=`, and `port=`
	2. Check for duplicates
	3. Validate the args
2. Call `gameserver` and store its exit value
	1. Initialize variables such as a hashtable `krags` and a hashtable `teams` as well as the server connection and a `gameInProgress` boolean
	2. Open the log file
	3. Call `startServer` using the `port` and store the connection it returns
	4. Call `readKrag` to load the `krags` hashtable with the `kff` file
	5. While `gameInProgress` is true
		1. Initialize a receiving address connection
		2. Receive a message with `receiveMessage`
		3. Parse the message wtih `parseMessage`
		4. Iterate through the op code function table `opCodes` and call the appropriate handler function
			* FA_CLAIM -> FAClaimHandler
				1. Logs the message
				2. Call `validateFAClaim` to validate message structure
				3. Call `validateKrag` to validate the krag ID, team, and location
				4. Update the krag and team structs
				5. Send `SH_CLAIMED` or `SH_CLAIMED_ALREADY` depending on step 2
				6. Updates and sends the team's secret string, sets `gameInProgress` to false if the string is completely revealed.
				7. Sends two clues to the GA if the string is not yet complete
			* FA_LOG -> FALogHandler
				1. Logs the message
				2. Calls `validateFALog` to valdiate message structure
				3. Logs the message using `logMessage`
			* GA_STATUS -> GAStatusHandler
				1. Logs the message
				2. Calls `validateGAStatus` to validate message structure
				3. Calls `addGuideAgent` which adds a team if it doesn't exist, adds a Guide Agent to the team if it doesn't exist, and does nothing if both exist.
				4. Calls `sendGameStatus` if the agent was new or if the GA_STATUS message requested an update
				5. Calls `sendAllGSAgents` to send Field Agent info to the Guide Agent
			* GA_HINT -> GAHintHandler
				1. Logs the message
				2. Calls `validateGAHint` to validate message structure
				3. Forwards the hint to the appropriate Field Agent(s) using `forwardHint`
			* FA_LOCATION -> FALocationHandler
				1. Logs the message
				2. Calls `validateFALocation` to validate message structure
				3. Calls `addNewFieldAgent` which adds a new team if it doesn't exist, adds the Field Agent if it doesn't exist, and does nothing if both exist
				4. Update the Field Agent struct with the new location
				5. Calls `sendGameStatus` to send the game status if the agent was new or if a status was requested
			* GAME_STATUS -> badOpCodeHandler
				1. Logs the message
				2. Sends `SH_ERROR_INVALID_OPCODE` to the caller to indicate bad op code
		5. Free the message and message string breaking from the `opCodes` loop
		6. Delete the receiving address connection
	6. Call `sendGameOver` to tell all players the game has ended
	7. Delete the `krags` hashtable and the `teams` hashtable
	8. Close the log file and return success code of 0

### Coupling and Cohesion
The Game Server is cohesive because most routines perform one function, or set of operations, on a specific set of data. For example, the validate functions perform validation on a single specific message. 

Another example is the function table that calls each handler. Each handler performs a very specific function to handle a specific message and is called via a structure that clearly passes data between functions via data-structure coupling. Many parameters are passed in these functions because the lower validation and send message functions require this data. In this way coupling allows functions with strong cohesion.


## Common - Network

### Data Structures
**Connection** 

```c
typedef struct connection {
	int socket;
	struct sockaddr* address;
} connection_t;
```

### Functions
**startServer**

Opens the server's socket and binds it to the correct IP and port. Returns a connection struct which holds the `int socket` and the `sockaddr* address`.

```c
connection_t* startServer(int port);
```
**openSocket**

Opens a socket for a port and host and returns a connection struct with the socket and address.

```c
connection_t* openSocket(int port, char* host);
```

**receiveMessage**

Receives a message from a socket. Takes a connection struct that has the socket and a `sockaddr*` to store who sent the message.

```c
char* receiveMessage(connection_t* connection);
```

**sendMessage**

Sends a message string through a socket to an address, both of which are passed via a connection struct.

```c
bool sendMessage(char* message, connection_t* connection);
```
**newConnection**

Creates a connection struct.

```c
connection_t* newConnection(int socket, sockaddr* address);
```

**deleteConnection**

Deletes a connection struct.

```c
void deleteConnection(connection_t* connection);
```

**closeSocket**

Closes a socket.

```c
void closeSocket(int comm);
```

## Common - Message

### Data Structures

##### Message

```
typedef struct message 
{
        char *opCode;
        char *respCode;
        char *kragId;
        char *gameId;
        char *guideId;
        char *pebbleId;
        int lastContact;
        double latitude;
        double longitude;
        int numPlayers;
        int numClaimed;
        int numKrags;
        int statusReq;
        char *player;
        char *team;
        char *hint;
        char *clue;
        char *secret;
        int errorCode;
} message_t;
```

### Functions

##### parseMessage

This function parses a given string into a message struct with the help of a
helper function, _parseHelper_, and stores the fields of a message into
the fields of the struct.

```
message_t * parseMessage(char *message);
```

##### parseHelper

This function is called by _parseMessage_ to actually do the parsing; it 
reads the name of the field and assigns the following value to the 
corresponding field of the message struct passed as a parameter. If there is
a duplicate field or an invalid opCode, the errorCode field of the message will
inform the Game Server of the type of error when the message struct is returned.

```
static int parseHelper(char *message, message_t *parsedMessage)
```

##### deleteMessage

This function is called to free all of the fields of the message struct and
then the message itself.

```
void deleteMessage(message_t *message);
```

## Common - Log

### Data Structures

##### connection

The log module utilizes the connection struct implemented in the `network.h`
module in order to get the IP address and port number info of the sender
or recipient (depending on the direction of the message).

### Functions

##### logMessage

This function logs a given message to the correct modules log file. It takes
that log file pointer, the message string, a direction (to or from), and
a connection struct as parameters. It uses the direction, a hard-coded "TO"
or "FROM" to tell if the message is inbound or outbound, then plugs this in
as part of the string to log. The components of the connection struct are also
used in this way, as the log format requires the IP address and the port number
as part of the message log entry.

```
void logMessage(FILE *file, char *message, char *direction, connection_t *connect);
```

## Common - TEAM

  The team module contains all the logic for creating teams, adding guide agents and field agents, and related functions. The module build a hashtable of teams where the key is the teamname and the item is a team struct. In the team struct itself, guide agent, and a set of field agents among other things are stored. To initialize the hashtable, call `initHash()`. Then, the hashtable is build by using `teamname` as the key and `team_t` structs as the iteams. In each team, there is a set of field agents, a set of krags, and a set of clues. In the field agents set, the key is the field agent `name` and the item is the `fieldAgent_t` struct. In the krags set, the key is the `kragID` and the item is the `krag_t` struct. In the clues set, the key is the `kragID` and the item is a string( which is the clue).  

### Data Structures
**Team** 

The team struct will hold all the essential elements that a team has. Each time will have a guide agent, a set of field agents, the current revealed string, the set of all the krags found so far, the two most recent clues, the set of all clues, and the number of claimed krags by the team. 

```c
typedef struct team {
  struct guideAgent * guideAgent;  //guide agent of the team
  set_t * FAset;      //set of field agents
  char * revealedString;  //the current releaved string of the team
  set_t * krags;     //set of all the krags the team has found
  char * recentClues[2];   //array of the two most recent clues
  set_t * clues;    //set of all the clues a team has
  int claimed;  //number of claimed krags of the team
} team_t;
```

**guideAgent**

The guide agent struct will hold all the essential elements that a guide agent has. Each team has only one guide agent. 

```c
typedef struct guideAgent { 
	char * guideID;  
 	char * name;      //name of the guide agent
 	char * gameID; 
 	connection_t * conn; //connection struct from network module   
} guideAgent_t;
```

**fieldAgent**

The field agent struct will hold all the essential elements that a field agent has. Each tame can have multiple field agents

```c
typedef struct fieldAgent {
	double latitude;  //latitude position of the field agent
	double longitude;  //longitude position of the field agent
	char * gameID;
	char pebbleID[9]; 
	connection_t * conn;  //conection struct form network module
	int lastContact;  //number of seconds since guide agent last heard from field agent
} fieldAgent_t;
```

### Functions
**getRevealedString**

This fucntion returns the revealed string of a team

```c
char * getRevealedString(char * teamname, hashtable_t * teamhash);
```
**initHash**

This function adds a field agent to a team. If the field agent is being added to a non-existing team, than a team will be created first, and then the field agent will be added to the team. In a normal situation, the function will return 0. If the user tries to add a field agent to a team that already has a field agent with the same name, then nothing happens and it returns 1.

	int addFieldAgent(char * name, char * pebbleID, char * teamname, char * gameID, connection_t * conn, hashtable_t * teamhash);

**addFieldAgent**

This function adds a field agent to a team. If the field agent is being added to a non-existing team, than a team will be created first, and then the field agent will be added to the team. In a normal situation, the function will return 0. If the user tries to add a field agent to a team that already has a field agent with the same name, then nothing happens and it returns 1.

	int addFieldAgent(char * name, char * pebbleID, char * teamname, char * gameID, connection_t * conn, hashtable_t * teamhash);

**addGuideAgent**

This function adds a guide agent to a team. If the guide agent is being added to a non-existing team, than a team will be created first, and then the guide agent will be added to the team. In a normal situation, the function will return 0. If the user tries to add a guide agent to a team that already
has a guide agent, then nothing happens and it returns 1.

	int addGuideAgent(char * guideID, char * teamname, char * name, char * gameID, connection_t * conn, hashtable_t * teamhash);
	
**addKrag**

This function adds a krag to the set of krags found by a team. This function should be called when a team find a krag. If the krag added is a new krag, then the krag will be added to the sts of krags found by a team and return 0. If the krags added has already been found by the team, then nothing is added and func returns 1.

	int addKrag(char * teamname, char * kragID, hashtable_t * kraghash, hashtable_t * teamhash );
	
**getGuideAgent**

This function returns the guide agent of the team

	guideAgent_t * getGuideAgent(char * teamname, hashtable_t * teamhash);

**getFieldAgent**

This function returns the field agent of the team

	fieldAgent_t * getFieldAgent(char * name, char * teamname, hashtable_t * teamhash);
	
**getAllFieldAgents**	
	
This function returns the set of all field agents in a team

	set_t * getAllFieldAgents(char * teamname, hashtable_t * teamhash);
	
**getGameIDGuidedA**

This function returns the gameID of a guide agent of a team

	char * getGameIDGuidedA( char * teamname, hashtable_t * teamhash);
	
	
**getGameIDFieldA**  

This function returns the gameID of a field agent of a team

	char * getGameIDFieldA(char * name, char * teamname, hashtable_t * teamhash);
	
	
**getKragsClaimed**

This function returns the number of krags claimed by the team.

	int getKragsClaimed ( char * teamname, hashtable_t * teamhash);
	
**getKrags**
	
This function returns the set of all the krags found by a team.
	
	set_t * getKrags(char * teamname, hashtable_t * teamhash);
	
**getClues**	

This function returns the set of all clues that a team has
	
	set_t * getClues(char * teamname, hashtable_t * teamhash);
	
**getClueOne**
	
This function returns the most recent clue that a team has

	char * getClueOne(char * teamname, hashtable_t * teamhash);
	
**getClueTwo**

This function returns the second-most recent clue that a team has

	char * getClueTwo(char * teamname, hashtable_t * teamhash);
	
**updateLocation**	
	
This function updates the location of a field agent. If the team does not exists or the field agent does not exists, then nothing is done.
	
	void updateLocation(char * name, char * teamname, double longitude, double latitude, hashtable_t * teamhash);
	
**incrementTime**
	
This function increments time since guide agent last heard from a field agent

	void incrementTime(char * name, char * teamname, hashtable_t * teamhash);
	
**getTime**	
	
This fucntions returns the time since guide agent last heard from a field agent

	int getTime(char * name, char * teamname, hashtable_t * teamhash);
	
**deleteTeamHash**	

This function frees memory of the hashtable and everything in it.
	
	void deleteTeamHash(hashtable_t * teamhash);
		
**newFieldAgent**

This function creates a new field agent.

	fieldAgent_t * newFieldAgent(char * gameID, char * pebbleID, connection_t * conn);
	
**newGuideAgent**	
	
This function creates a new guide agent

	guideAgent_t * newGuideAgent(char * guideID, char * name, char * gameID, connection_t * conn);
	
**newTeam**	
	
This function creates a new team
	
	team_t * newTeam(void);
	
**printTeams**	
	
This function prints all the teams and all of its members. It is helpful for testing

	void printTeams(hashtable_t * teamhash);
	
	
###Modularity

In the team module, there is strong cohesion because all routines perform functions related to one specific data strcuture, the team struct. Most of the routines have sequential cohesion, where they first retrieve some info or data structure from a team and perform some operations with it. Occasionally, there are some routines that have communicational cohesion. There is relatively strong coupling between the routines because all of them deal with the same types of data structure. For examples, one routines needs another routines to perform. The coupling is also strong because the team module uses some of the functions in the krag module.

## Common - KRAG

The krag module contains all the logic for saving krags to a hashtable, getting the secret string, revealing characters, and providing clues to krags. Given a filename, the module will read the file and store the krags in a hashtable where the key is the kragID and the item is a krag struct.

### Data Structures

**Krag**

The krag struct holds all the essential information of a krag. It includes langitude, longitude, the clue, and the number according to the order in the kragfile.

```c
typedef struct krag {  
  double latitude;   //latitude of the krag location
  double longitude;  //longitude of the krag location
  char clue[141];	//a string representing the clue to the krag
  int n;  //number according to the order in the kragfile       
} krag_t;
```

### Functions

**readKrag**

This functions reads a kragfile and creates a hashtable where the kragID is the key and the item is a krag struct. Caller is rensposible for freeing the pointer

	hashtable_t * readKrag(char * filename);
	
**getSecretString**	
	
This functions reads a secretfile and returns the secret string. Caller is rensposible for freeing the pointer

	char * getSecretString(char * filename);
	
**getSecretStringLen**	

This functions returns the lenght of the secret string

	int getSecretStringLen(char * string);
	
**revealCharacters**
	
This function reveal more characters to a team's current string. It takes the kragID for the krag found, teamname whose string is to be releaved, the secret string, the hashtable of the teams, and the hashtable of the krags. If a team calls the function for the first time, then this will set the current string of the team to be a series of '_' and set the number of claimed krags to zero. The function normally retuns 0. It only returns 1 when the team has claimed all krags. 

	int revealCharacters(char * kragID, char * teamname, char * secret, hashtable_t * teamhash, hashtable_t * kraghash);

**totalKrags**

This function computes the total number of krags. It takes the hashtable of krags as paramenter. If kraghash is NULL, then 0 is returned.

	int totalKrags(hashtable_t * kraghash);
	
**deleteKragHash**
	
This funtion deletes the krag struct

	void deleteKragHash(hashtable_t * kraghash);
	
**printKrags**
	
This function prints the kraghash with all its components.

	void printKrags(hashtable_t * kraghash);
	
**firstClue**
	
This function store a random first clue in a given team. This function should be called at the start of the game where a team is given one clue for a random krag.

	void firstClue(char * teamname, hashtable_t * kraghash, hashtable_t * teamhash);
	
**randomClue**

THis function stores two clues for two random krags that the team has not found yet. THis funciton should be called when a team find a krag.
	 
	void randomClue(char * teamname, hashtable_t * kraghash, hashtable_t * teamhash);
	
###Modularity
In the krag module, there is strong cohesion because all routines perform functions related to one specific data strcuture, the krag struct. Most of the routines have sequential cohesion, where they first retrieve some info or data structure from a krag and perform some operations with it, like revealing the characters of a krag. Sometimes, there are some routines that have communicational cohesion. There is relatively strong coupling between the routines because all of them deal with the same types of data structure. For examples, one routines needs another routines to perform. The coupling is also strong because the team module uses some of the functions in the team module.
