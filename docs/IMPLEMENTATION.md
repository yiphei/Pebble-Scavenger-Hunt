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

#### display.c
This component contains all functions and logic related to the graphical user
interface for the Guide Agent. It allows for abstracted use of the GUI by
`guideagent.c` to separate the display from the actual logic of the game for
the Guide Agent.

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
				1. Call `validateFAClaim` to validate message structure
				2. Call `validateKrag` to validate the krag ID, team, and location
				3. Update the krag and team structs
				4. Send `SH_CLAIMED` or `SH_CLAIMED_ALREADY` depending on step 2
				5. Updates and sends the team's secret string, sets `gameInProgress` to false if the string is completely revealed.
				6. Sends two clues to the GA if the string is not yet complete
			* FA_LOG -> FALogHandler
				1. Calls `validateFALog` to valdiate message structure
				2. Logs the message using `logMessage`
			* GA_STATUS -> GAStatusHandler
				1. Calls `validateGAStatus` to validate message structure
				2. Calls `addGuideAgent` which adds a team if it doesn't exist, adds a Guide Agent to the team if it doesn't exist, and does nothing if both exist.
				3. Calls `sendGameStatus` if the agent was new or if the GA_STATUS message requested an update
				4. Calls `sendAllGSAgents` to send Field Agent info to the Guide Agent
			* GA_HINT -> GAHintHandler
				1. Calls `validateGAHint` to validate message structure
				2. Forwards the hint to the appropriate Field Agent(s) using `forwardHint`
			* FA_LOCATION -> FALocationHandler
				1. Calls `validateFALocation` to validate message structure
				2. Calls `addNewFieldAgent` which adds a new team if it doesn't exist, adds the Field Agent if it doesn't exist, and does nothing if both exist
				3. Update the Field Agent struct with the new location
				4. Calls `sendGameStatus` to send the game status if the agent was new or if a status was requested
			* GAME_STATUS -> badOpCodeHandler
				1. Sends `SH_ERROR_INVALID_OPCODE` to the caller to indicate bad op code
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
