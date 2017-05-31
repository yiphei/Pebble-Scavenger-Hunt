# Guide Agent

This module implements a Guide Agent for the KRAG game. The Guide Agent
is a component of a team, of which there is only 1, that has its own graphical
interface displaying the status of the game (current game statistics, current 
location and status of each player on the Guide's team, current secret string, 
and a list of known clues). This interface also allows for the Guide Agent to 
send custom textual hints to a chosen Field Agent on the team. The Guide Agent
must communicate with and throughthe Game Server via the game's protocol, 
listed in the requirement spec. Lastly, the Guide Agent logs its own activity
to a logfile for the duration of the game. 

### Compiling

To compile, just input `make`.
To clean, `make clean`.

Exit Status:

* 0: success
* 1: invalid argument count
* 2: duplicate argument
* 3: missing necessary argument
* 4: misinformed argument
	* ex. non-hexidecimal gameId, team/player name too long, non-integer port
* 5: could not connect to server
* 6: could not open log file
* 7: memory allocation error

### Components

The Guide Agent consists of two main components: the guideagent.c program that
handles the game logic itself, and the display.c module that implements
programs to initialize and update the Guide Agent's visual interface.

### guideagent.c Functions

##### main

Parses and validates all parameters and passes the parameters to the guide agent
if there are no errors.

##### game

Connects to the server using the network module functions, creates the set of 
field agents to keep track of its team, and starts to receive messages.
Will parse messages using the message.[ch] files and taking advantage of the
message struct, handle messages using a function dispatch table comparing 
opCodes, and periodically send messages using the network module as well
(specifically GA_STATUS and requests for the GS_STATUS).

##### sendGA_STATUS

Given the components of a GA\_STATUS message, this function will send a 
GA\_STATUS to the Game Server and log the message using the common
logMessage function from log.h. This is called every 30 second the Guide Agent has 
been running in the game and immediately after the Guide Agent joins the game.

##### sendGA_HINT

Given the components of a GA_HINT message, this function will send a GA\_HINT
to the Game Server and log the message using the common _logMessage_ function
from `log.h`. This is called whenever the Guide Agent user inputs a custom
textual hint to their interface.

##### handleMessage

Handles a message input from the Game Server based on the given opCode. Uses
the function dispatch table implemented to handle different types of messages.

##### handleHint

Handles parsing and sending user input as hints to the guide agent when there
is pending input at stdin.

##### Handler functions

The guideagent.c component includes a lot of handler functions for specific
message types. These are called by a function dispatch table declared at the 
beginning of the guideagent.c file.

##### Validate functions

The guideagent.c component also includes message validate functions that 
make sure everybody is following the rules of the protocol and the requirement
spec in terms of a message.

### display.h Functions
The display.h is the module that handles the GUI part of the guide agent. It displays five windows in total. The map window will have field agents name on the top left, their current locations in the map, and a ASCII based map of the campus. The stats window will display the total number of krags in the game and the total nuber of krags claimed. The current string windown displays the current revealed string of the team. The clues window displays all the known clues to the team. The input window reads from stdin. When game ends, then it will display a game over window.

#####initialize_curses
Initialize curses like screen and color. This function should be called first in order to call all other functions in the module. From life.c program of David Kotz.

#####createWin_I
This function creates and returns a window given a height, width, and start x and y coordinates. From NCURSES Programming HOWTO http://www.tldp.org/HOWTO/NCURSES-Programming-HOWTO/windows.html

#####initializeWindows_I
This function initialites the five windows of the game and print their window names on the top left of each window. These windows are: game, game status, string, clues, and input. This function should be called after `initialize_curses()`

#####updateMap_I
This function displays the campus map with the name of the players and their locatins on the map with different colors. 

#####addPlayers_I
This function adds the field agents name on the top left corner of the mpa window, and their renspective location (marked with *) on the map. Each player and their location mark have an individual color. If there are more than seven field agents, then the colors will repeated since there are only 7 different colors available. This is called by `updateMap_I`

#####addKrags_I
This function adds krags in the map window in their respective location. They are marked with the name "krag". This is called by `updateMap_I`

#####updateString_I
This function displays the current revealed string of the team to the string window

#####updateClues_I
This function outputs all the clues to the the clue window. If clues exceed the borders of the clue window, then those clues wont be displayed

#####updateTotalKrags_I
This function displays the total krag number in the game. It is displayed in the stats window

#####updateKragsClaimed_I
This function displays the number of krags claimed by the team. It is displayed in the stats window

#####input_I
This function reads from stdin until user inputs a carriage return. When carriage return is inputted, then the function stops reading frmo stdin and returns the string to the caller. Caller is rensposible to free the pointer returned.

#####gameOver_I
This function displays the game over stats to the screen. For every team, it will diplay the teamname, number of players, and number of krags claimed. This function should be called at the end of the game. Then, to terminate GUI, press any key.

### Assumptions

* The messages received will contain the message fields in the requirement spec
	or else will be ignored.
* The user will input teams and players that correspond to those coded for the 
	Pebble.
* GA_STATUS will, at most, every 35 seconds. Will likely be faster because its
	timing is also event-driven, as receiving user input or messages from the 
	socket prompts the Guide Agent to send its GA_STATUS quicker.
* The krags and field agents given will only appear on the interface if their
	latitudes and longitudes are within given limits on the campus map.
* There exists a writable directory "../logs" in which to open and write the 
	guideagent.log file.
* There exists a map file called `campusmap` in ASCII code of size 90x45 in the `guide-agent` directory
* The window terminal has to meet the following minimum size: 126 width and 52 height. If the terminal window does not meet the following minimum size, then the GUI wont function and display properly
* Assume that hint is no more than 140 characters

### Limitations

* Implementation with ncurses causes still reachable memory.
* The clues will disappear but at time the whole clue will not be overwritten,
	or a clue will overwrite part of another clue due to the way ncurses 
	handles displaying these strings.
* Because of the use of select and the implementation of an accumulator, 
	sending GA_STATUS can sometime get sporatic depending on how much
	input the Guide Agent is receiving at one time (which can sometimes
	correlate with the amount of field agents on the guide agent's team.
* Because there are only seven colors in ncurses, so the colors for the field agent players will repeated if there are more than seven field agents
* Only works with terminal window of size 126 width and 52 height
* In the user input window of the GUI, backspace is not supported
* The GUI only works with the map provided by the assignment