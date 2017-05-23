##Guide Agent
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
The guideagent.c component will leverage a _set_ to store the field agents on
its team, which themselves will be stored in a _fieldAgent_ struct that is 
defined in `team.h`.

To be able to coontinuously send and receive messages from the Game Server, the
Guide Agent will also leverage a _connection_ struct defined in `network.h`.
This data structure just stores the socket address and the port number so that
both are accessible from the return value of _openSocket_.

When a message is received, a _message_ struct is also leveraged. This is
returned by _parseMessage_ in the `network.h` module, storing all components
of a specific message into the structure with their respective types. This
can be used to easily extract opCodes and dispatch functions, then easily parse
and use specific components of a message in handling that message without
having to parse each message type in a different way.

A _team_ struct will also be used to store the guide agent's own information,
along with that of his own team's field agents. This implementation allows
for the abstraction of the modification of this data as well, by having 
functions to update the agents and access the agents' information without
hardcoding it. Due to common implementation across modules, a _hashtable_
is used to store this team (that way, the Game Server can also use these
functions when handling multiple teams at a time).


#### display.c



