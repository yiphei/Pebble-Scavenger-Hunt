# KRAG Design
The KRAG game will be broken up into three components: Field Agent, Guide Agent, and Game Server, as well as a common library containing code used by all three.

## Field Agent
The Field Agent is a Pebble app that communicates with the Game Server and interacts with the player wearing the Pebble. Paolo will be the primary developer of the Field Agent.
 
### User Interface
The user interface for the Field Agent will be in the form of a pebble watchface.  The first screen will allow the user select from a hardcoded list of player names, and join a game using that given name.  Then, the “in-game” screen will consist of three options: Claim, Hints, and Status.  The “Claim open will then prompt the user to enter a 4 digit hex code that corresponds to the .  “Hints” provides the most recent hint that has been received from the Guide Agent.  Status will display the number of unclaimed krags remaining.  Also, the watch will vibrate and display a hint immediately when it is received.

### Inputs and Outputs
Inputs (Watch -- User):
Selected player name
Game to join
4 digit hex code for claiming krags
Display most recent hint
Status request

Outputs (Watch -- User):
Most recent hint
Status

Inputs (From Game Server): 
	* `GA_HINT` - Message sent from Game Agent that contains textual hints for the Field Agent.
	* `GAME_STATUS` - Message sent from Game Server as a response to the FA\_LOCATION request from the Field Agent.

The Following inputs and outputs consist of messages sent in the form `opCode=OPCODE|fieldName1=fieldValue1|fieldName2=fieldValue2`:

Outputs (To Game Server):
	* `FA_LOCATION` - Continually tell the Game Server the location of the pebble watch every 15 seconds.
	* `FA_CLAIM` - Sends message to game server with location information and the 4 digit hex code that the Field Agent entered in order to claim a krag (if information is valid).
	* `FA_LOG` - Sends informative messages to Game Server to keep pebble activities in logs.

### Functional Decomposition


### Data Structures
-The Field Agent will make use of the Message data structure we will implement in the common module. 
-It will also utilize an array of strings to hold 

### Pseudo Code


### Testing Plan


## Guide Agent
The Guide Agent is a Unix program that communicates with the Game Server and interacts with the player who acts as a guide. Michael and Yi Fei will be the primary developers of the Guide Agent.

### User Interface

The user interface of the Guide Agent will be split into two sections; it is initialized from the command line in the form:
```
./guideagent guideId=... team=... player = … host=... port=...
```

After initializing the Guide Agent, the Agent will have a visual interface displaying the game status to the Guide player (game statistics, locations of Field players, current secret string, and a list of known clues) and allowing the Guide player to input information (textual hints). 

### Inputs and Outputs
Input at module initialization comes from the command line. This input consists of the guideId, team, player, host, and port. The `guideId` provides a hexidecimal number for this specific Agent, the `team` provides the name of the team to which the Agent belongs, `player` is the name of the player, `host` is the host name of the server, and `port` provides the port number of the game server. 

Inputs and outputs consist of messages sent in the form `opCode=OPCODE|fieldName1=fieldValue1|fieldName2=fieldValue2`.

Below is a short description of each message type, for more specific definitions of the message types, see the Requirements spec.
Inputs:
	* `GS_STATUS` - Update on the game status received from the Game Server
	* `TEAM_RECORD` - Received upon game ending; tells Guide Agent final record of the team
	* `GAME_OVER` - Received to signify the end of the game
	* `GS_SECRET` - Received from the Game Server after the team successfully locates a krag, with some letters of the secret uncovered

Outputs:
	* `GA_STATUS` - The status of the guide agent
	* `GS_STATUS` (request) - Requests the game status from the Game Server
	* `GA_HINT` - Custom textual hint to indirectly send the Field Agent(s) through the Game Server
	* Log message - Logs all of its activity to a logfile

### Functional Decomposition
`main`: parse and validate command line arguments
`game`: informs the Game Server that it has joined the game, supplies its information, and calls the other functions to play the game until `GAME_OVER` is received
`sendMessage`: sends a message of a specific type to the Game Server
`handleMessage`: utilizes the common module to parse message input and determine message input type
`buildInterface`: builds the graphical interface and calls _updateInterface_ to update the interface periodically
`updateInterface`: updates the graphical interface if there is anything to change
`sendHint`: sends a hint through the interface indirectly to the Field Agent; utilizes sendMessage


### Data Structures
The Guide Agent will make use of the Message data structure we will implement in the common module. 

### Pseudo Code
Parse and validate command line arguments


### Testing Plan


## Game Server
The Game Server is a server that communicates with all players, from all teams, to record the game’s progress and provide information to each of the players. Tony will be the primary developer of the Game Server.

### User Interface
The Game Server will be started with a command line call: `./gameserver gameId=... kff=... sf=... port=...` where `gameId` is a unique hexidecimal ID number, `kff` is the path to the KRAG file containing coordinates and clues for each KRAG, `sf` is the secret file path containing the secret string, and `port` is the port number for the Game Server.
The only other user input occurs if the user at the terminal decides to end the game early.

### Inputs and Outputs
Inputs and outputs consist of messages sent in the form `opCode=OPCODE|fieldName1=fieldValue1|fieldName2=fieldValue2`. Inputs and outputs will flow through a single UDP socket.
Below is a short description of each message type, for more specific definitions of the message types, see the Requirements spec.

Inputs:
	* `GAME_STATUS` (request) - Request from the Guide Agent for the Game Server status
	* `GA_STATUS` - The status of the Guide Agent
	* `GA_HINT` - Custom hint from the Guide Agent to be forwarded to the Field Agent
	* `FA_CLAIM` - Message from the Field Agent claiming a KRAG
	* `FA_LOCATION` - The location of the Field Agent, received every 15 seconds
	* `FA_LOG` - Request from Field Agent to log a message
Outputs:
* `GAME_STATUS` - The status of the game sent to the Guide Agent and Field Agent
* `GA_STATUS` (request) - Request for the Guide Agent status
* `GA_HINT` - The custom Guide Agent hint to forward to the Field Agent
* `GS_CLUE` - A clue from file about the location of a KRAG
* `GS_SECRET` - A portion of the secret string from file
* `GAME_OVER` - A message ending the game
* `GS_RESPONSE` - Error handling/messages of the Game Server sent to the Guide Agent and the Field Agents
* `TEAM_RECORD` - Statistics of the team sent to the Guide Agent and Field Agents
* Logs - The Game Server must output to the log files for all messages and errors

### Functional Decomposition
Functions within Game Server: 
	* `sendStatus` - Sends the Game Status
	* `sendResponse` - Sends a response
	* `requestGuideStatus` - Requests the status of the Guide Agent
	* `forwardHint` - Forwards a hint received from the Guide Agent to the Field Agent
	* `giveClue` - Reveals a clue to the agents
	* `giveSecretFrag` - Reveals a portion of the secret string to the agents
	* `gameOver` - Sends a game over message to the agents
	* `sendRecord` - Sends a team record to the agents
	* `log` - Logs information
Common Library:
	* Message Module - Used for constructing and handling messages
	* Socket Module - Used for opening and closing sockets
	* Krag Module - Used for reading files

### Data Structures
The Game Server makes use of the Message data structure to send, process, and receive messages and the krag data structure to handle krags.

### Pseudo Code

### Testing Plan

## Libraries

### Common
The common library will contain functions and structures to parse and handle messages. All three components will use this library in their code. The library will consists of three modules: a message module, a socket module, and a krag module.

The message module will handle messaging between the three components. The module will parse the messages and check for their correctness. 
Functions:
	* `sendMessage` - Takes a message struct and sends it through the socket
	* `getMessageType` - Returns the OpCode
	* `deleteMessage` - Deletes a message struct
	* `parseMessage` - Parses a message and creates a message struct
	* `getMessageField` - Returns a specified message field

The socket module will handle sockets of the three components. The module will support logging messages between the three components.
Functions: 
	* `openSocket` - Opens socket to assigned proxy

The krag module will handle krags. The module will support reading a krag file and build an appropriate data structure to store coordinates and clues for each krag. It will also read the secretfile and store it accordingly and easily accessible by other three components, and update the secret with more characters revealed when a krag is claimed. It will also support function like getting the clue for a krag.
Functions: 
	* `getKragById` - Creates a returns a krag structure after searching the krag file for an ID
* `getClue` - Gets a clue from a krag
* `getSecretString` - Gets the secret string
* `getSecretStringLen` - Gets the length of the secret string
* `revealCharacters` - Takes a team’s current version of the secret string and a krag number and changes the secret string to reveal more letters based on the krag number
* `deleteKrag` - Deletes a krag struct



The log module will handle writing to log files. It will keep track of current logs and make sure old files are not overwritten, etc.
Functions:
	* `createLog` - Creates a log file
	* `writeLog` - Writes to the log file


