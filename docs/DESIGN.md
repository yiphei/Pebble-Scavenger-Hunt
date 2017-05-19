# KRAG Design
_Tony DiPadova, Michael Perezous, Paolo Takagi, Yifei Yan_

The KRAG game will be broken up into three components: Field Agent, Guide Agent, and Game Server, as well as a common library containing code used by all three.

---

## Field Agent
The Field Agent is a Pebble app that communicates with the Game Server and interacts with the player wearing the Pebble. Paolo will be the primary developer of the Field Agent.
 
### User Interface
The user interface for the Field Agent will be in the form of a pebble watchface.  The first screen will allow the user to select from a hardcoded list of player names and prompt them for the team name as well.  Then, the pebble will attempt to join a game using that given information.  Then, the “in-game” screen will consist of three options: Claim, Hints, and Status.  The “Claim” open will then prompt the user to enter a 4 digit hex code that corresponds to any krags that they find.  “Hints” provides the most recent hint that has been received from the Guide Agent.  Status will display the number of unclaimed krags remaining.  Also, the watch will vibrate and display a hint immediately when it is received.  Finally, when the game ends, the screen will display a “Game Over” screen that will say which team won the game.  At this point, the program will end after any user input.

### Inputs and Outputs

Inputs (Watch -- User):

* Selected player name
* Team name
* 4 digit hex code for claiming krags
* Display most recent hint
* Status request

Outputs (Watch -- User):

* Most recent hint
* Status
* “Game Over” screen
* Other alerts (hints, any errors, etc).
* Alerts pertaining to krags (successfully claimed, already claimed, failed)

The Following inputs and outputs consist of messages sent in the form `opCode=OPCODE|fieldName1=fieldValue1|fieldName2=fieldValue2`:

Inputs (From Game Server): 

* `GA_HINT` - Message sent from Guide Agent that contains textual hints for the Field Agent.
* `GAME_STATUS` - Message sent from Game Server as a response to the FA_LOCATION request from the Field Agent.
* `GAME_OVER` - Message signifies that a team has found all of the krags, and the game has ended.

Outputs (To Game Server):

* `FA_LOCATION` - Continually tell the Game Server the location of the pebble watch every 15 seconds.
* `FA_CLAIM` - Sends message to Game Server with location information and the 4 digit hex code that the Field Agent entered in order to claim a krag (if information is valid).
* `FA_LOG` - Sends informative messages to Game Server to keep pebble activities in logs.

### Functional Decomposition
* `main` - builds the watch face by running `init`, makes sure smartphone proxy is running, prompts the user for team and player name inputs, and tries to connect to the Game Server via unix proxy (aka joins the game).
* `init` - builds the initial watch face
* `deinit` - destroys the windows
* `game` - Upon successfully joining the game, displays the in-game watch face, and waits for inputs from both the Game Server and from the Pebble user.
* `gameOver` - Upon receiving the message indicating that the game is over, displays the team winner on the watch face and exits upon user input.
* `proxyRunning` - determines whether or not the JS environment on the smartphone is currently running.
* `getId` - gets the pebble id from the smartphone.
* `sendString` - sends a string to the smartphone over bluetooth connection, used by other functions.
* `displayHint` - displays the most recent hint (called when the user requests it and whenever a new hint is received).
* `getStatus` - displays the current status of the game, after requesting said status from the Game Server
* `sendLocation` - sends location to the Game Server.
* `sendLog` - sends a `FA_LOG` message to the Game Server.
* `claimKrag` - sends a `FA_CLAIM` message to Game Server with inputted `kragId`.  It then waits to receive the corresponding response code, and displays a corresponding message for the user.
* `handleMessage`: utilizes the common module to parse message input and determine message input type, handling it as is fit
* `makeAlert` creates an alert on the Pebble watchface, taking in string input for what the content of the alert will be.  After user input it will be destroyed.

### Data Structures
The Field Agent will make use of the Message data structure we will implement in the common module.

### Pseudo Code
1. Build watchface.
2. Check to see that JS environment (proxy) on smartphone is up and running.
3. Get the `pebbleId` from the smartphone.
4. Prompt user to select a player name from a preconfigured list.
5. Try to connect to the Game Server through Unix proxy (aka join the game)
6. While the game is not over:
    1. Send an updated location every 15 seconds to Game Server
    2. Listen to input from user, and handle it accordingly (i.e. changing the display or sending messages to the Game Server).
    3. Listen to input from Game Server, and handle it accordingly (i.e. changing the display, or sending new messages back to the Game Server).
7. Game has ended, display the winner, exit after any user input.


### Testing Plan

1. Test watchface build
2. Test connecting to smartphone proxy
3. Test getting `pebbleId` from smartphone
4. Test connecting to Game Server through Unix proxy
5. Test joining a game with valid and invalid input for team name.
6. Test receiving and handling input from user while in game.
7. Test receiving and handling input from Game Server while in game.
8. Test ending the game properly, and destroying all corresponding windows.
9. Integration test with the Guide Agent(s) and Field Agent(s) when all have been tested individually.

---

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

* `main`: parse and validate command line arguments
* `game`: informs the Game Server that it has joined the game, supplies its information, and calls the other functions to play the game until `GAME_OVER` is received
* `handleMessage`: utilizes the common module to parse message input and determine message input type, handling it as is fit
* `buildInterface`: builds the graphical interface and calls _updateInterface_ to update the interface periodically
* `updateInterface`: updates the graphical interface if there is anything to change
* `sendHint`: sends a custom, textual hint through the interface indirectly to the Field Agent; utilizes sendMessage


### Data Structures
The Guide Agent will make use of the Message data structure we will implement in the common library. 

### Pseudo Code
Parse and validate command line arguments
Open a socket to connect to the Game Server
Send `GA_STATUS` message to the Game Server
Build the graphical user interface
While the game is not over
Listen for messages from the Game Server
Check for “custom hint” textual input from user
If input exists, send custom hint to Game Server
Update graphical interface based on any received messages
Periodically update the Game Server of its status

### Testing Plan
Test opening a socket and joining a server.
Test incorrect message handling using hard-coded input.
Test correct message handling using hard-coded input.
Test graphical interface build. 
Test graphical interface update using hard-coded input.
Integration test with the Game Server and Field Agent(s) when all have been tested individually.

---

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
* `GS_AGENT` - The status of each Field Agent sent to the Guide Agent
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
* Network Module - Used for opening and closing sockets
* Krag Module - Used for reading files
* Team Module - Used for functions and structures regarding Field Agents, Guide Agents, and teams
* Log Module - Used for logging to file

### Data Structures
The Game Server makes use of the Message data structure to send, process, and receive messages and the krag data structure to handle krags.

### Pseudo Code

1. Parse args including files and IPs
2. Open socket to correct IP and port number
3. Load the Krag and Secret files and initialize game variables
4. While there are unfound krag files or the game hasn’t been forced to end, listen for messages from the Guide Agents and Field Agents. Each message will be validated before the following actions are taken:
    1. If the message is `GA_STATUS` from a new Guide Agent, create a new team and Guide Agent and send a `GAME_STATUS` message in response
    2. If the message is `FA_LOCATION` from a new Field Agent, allowing them to join their specified team, or creating a new team if that team doesn’t exist, send a `GAME_STATUS` response
    3. If the message is `FA_LOCATION`, update the Field Agent struct and send a `GAME_STATUS` message to the Field Agent if requested
    4. If the message is `GA_STATUS` message, update records for the Guide Agent and returns a `GAME_STATUS` if requested
    5. If the message is `FA_CLAIM` message, validate the location of the krag and update the structs sending a `SH_CLAIMED` or `SH_CLAIMED_ALREADY` response as appropriate. Update the team’s secret string copy and sent the Guide Agent the new copy. Send two `GS_CLUE`’s to the Guide Agent
If the message is `FA_LOG` message, log the Field Agents text to the log file
If the message is `GA_HINT`, forward the hint to the appropriate Field Agent(s)
5. When the game ends the server sends `GAME_OVER` to all of the agents
6. The server then sends a `TEAM_RECORD` to each agent
7. Close all sockets and free all memory

### Testing Plan
Test opening a socket and receiving connections.
Test incorrect message handling using hard-coded input.
Test correct message handling using hard-coded input.
Test for memory leaks during and after a simulated game using hard coded messages.
Integration test with the Guide Agent(s) and Field Agent(s) when all have been tested individually.

---

## Common Library

The common library will contain functions and structures to parse and handle messages. All three components will use this library in their code. The library will consists of three modules: a message module, a socket module, and a krag module. The common library will also include the libcs50 library, which includes data structures like set, counters, and hashtable.

### Message Module

The message module will handle messaging between the three components. The module will parse the messages and check for their correctness. 
Functions:

* `sendMessage` - Takes a message struct and sends it through the socket
* `getMessageType` - Returns the OpCode
* `deleteMessage` - Deletes a message struct
* `parseMessage` - Parses a message and creates a message struct
* `getMessageField` - Returns a specified message field

### Network Module

The network module will handle sockets of the three components. The module will support logging messages between the three components.
Functions: 

* `openSocket` - Opens socket to assigned proxy
* `closeSocket` - Closes the socket at the end of the game

### Krag Module

The krag module will handle krags. The module will support reading a krag file and build an appropriate data structure to store coordinates and clues for each krag. It will also read the secretfile and store it accordingly and easily accessible by other three components, and update the secret with more characters revealed when a krag is claimed. It will also support function like getting the clue for a krag. The module also includes a krag struct that will store the longitude, latitude, and clue in it. 
Functions: 

* `getClue` - Takes a team name as the parameter and returns a clue for a krag that the team has not claimed yet.
* `getSecretString` - Gets the secret string
* `getSecretStringLen` - Gets the length of the secret string
* `revealCharacters` - Takes a team’s current version of the secret string and a krag number and changes the secret string to reveal more letters based on the krag number
* `deleteKrag` - Deletes a krag struct
* `readKrag` - reads the krag file and build a data structure (hashtable) that stores each krag with its clue. Then, the function returns the data structure to the caller. 

### Log Module

The log module will handle writing to log files. It will keep track of current logs and make sure old files are not overwritten, etc.
Functions:

* `createLog` - Creates a log file
* `writeLog` - Writes to the log file

### Team Module

The teams module will store data about each Guide Agent and Field Agent.

* `team` - Represents team information like name, players, krags found, and current revealed string
* `fAgent` - Represents Field Agent information like name and location
* `gAgent` - Represents Guide Agent information like name
