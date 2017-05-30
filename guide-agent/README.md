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

### Limitations

* Implementation with ncurses causes still reachable memory.