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

### Functions

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